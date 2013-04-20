/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Vovoid Media Technologies Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "_configuration.h"
#if defined(WIN32) || defined(_WIN64) || defined(__WATCOMC__)
  #include <windows.h>
  #include <conio.h>
#else
  //#include "wincompat.h"
#endif
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsx_float_array.h"
#include <fmod.h>
#include <fmod_errors.h>
#include "vsx_math_3d.h"
#include "fmod_holder.h"

bool fmod_init = false;

FMOD_SYSTEM           *fsystem;


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}



class vsx_module_sound_stream_play : public vsx_module {
  float d_time, v_time;
  // in
	vsx_module_param_int* play_mode;
	vsx_module_param_int* time_mode;
	vsx_module_param_int* spectrum_enabled;
	vsx_module_param_resource* filename;
	vsx_module_param_float* fx_level;
	// out
	vsx_module_param_float* vu_l_p;
	vsx_module_param_float* vu_r_p;
	vsx_module_param_float* octaves_l_0_p;
	vsx_module_param_float* octaves_l_1_p;
	vsx_module_param_float* octaves_l_2_p;
	vsx_module_param_float* octaves_l_3_p;
	vsx_module_param_float* octaves_l_4_p;
	vsx_module_param_float* octaves_l_5_p;
	vsx_module_param_float* octaves_l_6_p;
	vsx_module_param_float* octaves_l_7_p;
	vsx_module_param_float* octaves_r_0_p;
	vsx_module_param_float* octaves_r_1_p;
	vsx_module_param_float* octaves_r_2_p;
	vsx_module_param_float* octaves_r_3_p;
	vsx_module_param_float* octaves_r_4_p;
	vsx_module_param_float* octaves_r_5_p;
	vsx_module_param_float* octaves_r_6_p;
	vsx_module_param_float* octaves_r_7_p;
	vsx_module_param_float_array* sample_l_p;
	vsx_module_param_float_array* sample_r_p;
	vsx_module_param_float_array* spectrum_p;
	vsx_module_param_float_array* octave_spectrum_p;
	vsx_module_param_float* st;
	// internal
  vsx_float_array spectrum;
  vsx_string cur_filename;
  int i_state;
  int wave_pos;
  FMOD_CREATESOUNDEXINFO exinfo;
  FMOD_SOUND            *sound;
  FMOD_CHANNEL          *channel;
  FMOD_RESULT            result;
  int                    key, driver, numdrivers, count;
  unsigned int           version;
  float *fft;
  vsx_engine_float_array full_pcm_data_l;
  vsx_engine_float_array full_pcm_data_r;
public:
  vsx_float_array sample_l;
  vsx_float_array sample_r;
  vsx_float_array sample_l_int;
  vsx_float_array sample_r_int;


void module_info(vsx_module_info* info)
{
  info->output = 1;

  info->identifier = "system;sound;stream_play";
#ifndef VSX_NO_CLIENT
  info->description = "Plays a stream (mp3/wav/xm).";
  info->in_param_spec = "filename:resource,\
time_mode:enum?async|sync&help=`If sync is set, the position of the\nsong will be that of the engine time\n- useful for demos and presentations.\nIf async, it ignores the engine time\nand you can play songs independent from the time.`,\
play_mode:enum?play|pause,\
spectrum_enabled:enum?no|yes,\
fx_level:float\
";
  info->out_param_spec = "\
vu:complex{\
vu_l:float,\
vu_r:float\
},\
octaves:complex{\
  left:complex{\
    octaves_l_0:float,\
    octaves_l_1:float,\
    octaves_l_2:float,\
    octaves_l_3:float,\
    octaves_l_4:float,\
    octaves_l_5:float,\
    octaves_l_6:float,\
    octaves_l_7:float\
  },\
  right:complex{\
    octaves_r_0:float,\
    octaves_r_1:float,\
    octaves_r_2:float,\
    octaves_r_3:float,\
    octaves_r_4:float,\
    octaves_r_5:float,\
    octaves_r_6:float,\
    octaves_r_7:float\
  }\
},\
  spectrum:float_array,sample_l:float_array,sample_r:float_array";
  info->component_class = "output";
#endif
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  fft = new float[512];
  // special for controlling the system time
  st = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"_st");
  st->set(-1.0f);

  play_mode = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"play_mode");
  play_mode->set(1);
  time_mode = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"time_mode");
  spectrum_enabled = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"spectrum_enabled");
  filename = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
  filename->set("");
	fx_level = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"fx_level");
	fx_level->set(1.0f);
  //////////////////
	vu_l_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"vu_l");
	vu_r_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"vu_r");
	vu_l_p->set(0);
	vu_r_p->set(0);
  octaves_l_0_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_l_0");
  octaves_l_1_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_l_1");
  octaves_l_2_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_l_2");
  octaves_l_3_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_l_3");
  octaves_l_4_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_l_4");
  octaves_l_5_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_l_5");
  octaves_l_6_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_l_6");
  octaves_l_7_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_l_7");
  octaves_r_0_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_r_0");
  octaves_r_1_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_r_1");
  octaves_r_2_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_r_2");
  octaves_r_3_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_r_3");
  octaves_r_4_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_r_4");
  octaves_r_5_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_r_5");
  octaves_r_6_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_r_6");
  octaves_r_7_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_r_7");
  octaves_l_0_p->set(0);
  octaves_l_1_p->set(0);
  octaves_l_2_p->set(0);
  octaves_l_3_p->set(0);
  octaves_l_4_p->set(0);
  octaves_l_5_p->set(0);
  octaves_l_6_p->set(0);
  octaves_l_7_p->set(0);
  octaves_r_0_p->set(0);
  octaves_r_1_p->set(0);
  octaves_r_2_p->set(0);
  octaves_r_3_p->set(0);
  octaves_r_4_p->set(0);
  octaves_r_5_p->set(0);
  octaves_r_6_p->set(0);
  octaves_r_7_p->set(0);
  spectrum_p = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"spectrum");
  sample_l_p = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"sample_l");
  sample_r_p = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"sample_r");
  spectrum.data = new vsx_array<float>;
  for (int i = 0; i < 512; ++i) spectrum.data->push_back(0);
  spectrum_p->set_p(spectrum);

  //printf("initializing sample data\n");
  sample_l.data = new vsx_array<float>;
  sample_r.data = new vsx_array<float>;
  sample_l_int.data = new vsx_array<float>;
  sample_r_int.data = new vsx_array<float>;
  //printf("fmod data %d\n", sample_l.data);
  for (int i = 0; i < 512; ++i) {
  	sample_l.data->push_back(1);
  	sample_r.data->push_back(1);
  }
  for (int i = 0; i < 2048; ++i) {
    sample_l_int.data->push_back(1);
    sample_r_int.data->push_back(1);
  }
  //printf("fmod data %d\n", sample_l.data);
  sample_l_p->set_p(sample_l);
  sample_r_p->set_p(sample_r);
  wave_pos = 0;
  channel = 0;
  //printf("spectrum size0: %d\n",spectrum.data->size());
}

bool init() {
  //printf("fmod module init\n");
  //channel = -1;
  //FSOUND_Stream_SetBufferSize(1000);

  i_state = VSX_ENGINE_STOPPED;
  return true;
}

void on_delete() {
  delete spectrum.data;
  if (sound) FMOD_Sound_Release(sound);
}

void run() {
  engine->param_float_arrays[2] = &full_pcm_data_l;
  engine->param_float_arrays[3] = &full_pcm_data_r;
  if (!fmod_init) {
    //printf("Initializing fmod...\n");
    result = FMOD_System_Create(&fsystem);
    ERRCHECK(result);

    result = FMOD_System_GetVersion(fsystem, &version);
    ERRCHECK(result);
    FMOD_System_SetOutput(fsystem, FMOD_OUTPUTTYPE_ALSA);
    //FMOD_System_SetSoftwareFormat(fsystem,48000,FMOD_SOUND_FORMAT_PCM16,2,0,FMOD_DSP_RESAMPLER_LINEAR);
    ERRCHECK(result);
    result = FMOD_System_SetDriver(fsystem, 0);
    ERRCHECK(result);
    result = FMOD_System_Init(fsystem, 32, FMOD_INIT_NORMAL, NULL);
    ERRCHECK(result);
    fmod_init = true;
  }
  //printf("fmod_%d\n",__LINE__);
  if (cur_filename != filename->get() || !loading_done) {
    cur_filename = filename->get();
    if (cur_filename != "")
    {
    //  printf("fmod_%d\n",__LINE__);
      if (!loading_done) {
        loading_done = true;
      } else {
        FMOD_Sound_Release(sound);
        sound = 0;
      }
      //printf("opening stream: %s\n",cur_filename.c_str());
      //printf("fmod LOADING stream_%d\n",__LINE__);
      vsxf_handle *fp;
      if ((fp = engine->filesystem->f_open(cur_filename.c_str(), "rb")) == NULL)
        return;
//      printf("fmod_%d\n",__LINE__);
      //printf("fmod %d\n",__LINE__);
      unsigned long fsize = engine->filesystem->f_get_size(fp);
      char* fdata = new char[fsize+1];
      unsigned long bread = engine->filesystem->f_read(fdata, engine->filesystem->f_get_size(fp), fp);
  //    printf("fmod_%d\n",__LINE__);
      //printf("fmod %d\n",__LINE__);
      if (bread == fsize) {
        //printf("fmod_%d\n",__LINE__);
        memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
        exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
        //printf("fmod_%d\n",__LINE__);
        //printf("fmod %d\n",__LINE__);
        exinfo.length = fsize;
        //printf("fmod_%d\n",__LINE__);
        result = FMOD_System_CreateSound(fsystem, fdata, FMOD_2D | FMOD_OPENMEMORY | FMOD_SOFTWARE | FMOD_ACCURATETIME, &exinfo, &sound);
        //printf("fmod_%d\n",__LINE__);

        // find length of sample in bytes
        unsigned int byte_length;
        FMOD_Sound_GetLength(sound, &byte_length, FMOD_TIMEUNIT_PCMBYTES);
        //printf("song length in bytes: %d\n", byte_length);
        void* sample_data;
        void* sample_data_ext;
        unsigned int len1, len2;
        FMOD_Sound_Lock(
          sound,
          0,
          byte_length,
          &sample_data,
          &sample_data_ext,
          &len1,
          &len2
        );
        if (len1 == byte_length)
        {
          //printf("length is matching! \n");
          signed short* sample_data_short = (signed short*)sample_data;
          size_t k = byte_length / sizeof(signed short);
          size_t index = 0;
          float divisor = 1.0f / 32768.0f;
          for (size_t i = 0; i < k; i+=2)
          {

            full_pcm_data_l.array[index] = sample_data_short[i  ] * divisor;
            full_pcm_data_r.array[index] = sample_data_short[i+1] * divisor;
            //printf("%f ", full_pcm_data_l.array[index]);
            index++;
          }
          //printf("index reached: %d\n", index);
        }

        FMOD_Sound_Unlock(
            sound,
            sample_data,
            sample_data_ext,
            len1,
            len2
        );





        ERRCHECK(result);
     //   printf("fmod_%d\n",__LINE__);
        //printf("fmod %d\n",__LINE__);
        if (time_mode->get() == 0) {
       //   printf("fmod_%d\n",__LINE__);
          result = FMOD_System_PlaySound(fsystem, FMOD_CHANNEL_FREE, sound, 0, &channel);
          //printf("fmod_%d\n",__LINE__);
          //printf("fmod %d\n",__LINE__);
          ERRCHECK(result);
          //printf("fmod_%d\n",__LINE__);
          //channel = FSOUND_Stream_PlayEx(FSOUND_FREE, stream, NULL, TRUE);
          //FSOUND_SetPaused(channel,FALSE);
        }
      }
    }
  }

  if (time_mode->get() == 0) {
    switch (play_mode->get()) {
      case 0:
        //printf("fmod %d\n",__LINE__);
        FMOD_Channel_SetPaused(channel,false);
        //printf("fmod %d\n",__LINE__);
      break;
      case 1:
        //printf("fmod %d\n",__LINE__);
        FMOD_Channel_SetPaused(channel,true);
        //printf("fmod %d\n",__LINE__);
      break;
    }
  } else
  if (time_mode->get() == 1) {
    // sync mode
    if (engine->state != -1)
    if (i_state != engine->state) {
      //printf("fmod %d\n",__LINE__);
      i_state = engine->state;
      //printf("engine_state: %d\n", engine->state);
      if (i_state == VSX_ENGINE_STOPPED) {
        //printf("fmod %d\n",__LINE__);
        FMOD_Channel_SetPaused(channel, true);
        //printf("fmod %d\n",__LINE__);
      }
      else
      if (i_state == VSX_ENGINE_PLAYING) {
        if (!channel)
        {
          //printf("fmod %d\n",__LINE__);
          result = FMOD_System_PlaySound(fsystem, FMOD_CHANNEL_FREE, sound, 0, &channel);
          ERRCHECK(result);
          //printf("fmod %d\n",__LINE__);
        }
        //printf("fmod %d\n",__LINE__);
        FMOD_Channel_SetPaused(channel,false);
        //printf("fmod %d\n",__LINE__);
      }
    }
    if (i_state == VSX_ENGINE_STOPPED && engine->dtime != 0) {
      st->set(-1.0f);
      if (channel) {
        //printf("fmod %d\n",__LINE__);
        if (engine->vtime == 0)
        FMOD_Channel_SetPosition(channel, (int)round(((engine->vtime)*1000.0)),FMOD_TIMEUNIT_MS );
//        FSOUND_Stream_SetTime(stream, (int)round(((engine->vtime)*1000.0)));
        else
        FMOD_Channel_SetPosition(channel, (int)round(((engine->vtime)*1000.0)),FMOD_TIMEUNIT_MS );
      }
    }
    if (i_state == VSX_ENGINE_PLAYING) {
      //printf("fmod %d channel: %d\n",__LINE__,channel);
      unsigned int chpos;
      if (FMOD_Channel_GetPosition(channel, &chpos, FMOD_TIMEUNIT_MS) == FMOD_OK)
      {
        //printf("fmod %d %d\n",__LINE__,chpos);
        float ft = (float)chpos;
        //printf("channel pos: %f\n",ft);
        if (ft != 0.0f)
        st->set((ft)/1000.0f);
        else
        st->set(-1.0f);
      }
    }
  }

  //printf("fmod %d\n",__LINE__);
  result = FMOD_System_GetSpectrum(fsystem, (float*)&fft[0],512, 0, FMOD_DSP_FFT_WINDOW_RECT);
  ERRCHECK(result);
  //float* fft = FSOUND_DSP_GetSpectrum();
  //normalize_fft((float*)&fft,spectrum);
  //printf("fmod %d\n",__LINE__);
  //printf("fmod data %d\n", sample_l.data);
  FMOD_System_GetWaveData(fsystem, sample_l_int.data->get_pointer(),2048, 0);
  //printf("fmod %d\n",__LINE__);
  FMOD_System_GetWaveData(fsystem, sample_r_int.data->get_pointer(),2048, 1);
  int i = 0;

  int wpos;
    wpos = 256 * (wave_pos%7);
  while (i < 512) {
    //printf("wpos: %d %d",wpos,i);
    //fflush(stdout);
    (*(sample_l.data))[i] = (*(sample_l_int.data))[wpos];
    (*(sample_r.data))[i] = (*(sample_r_int.data))[wpos];
    //sample_r.data[i] = sample_r_int.data[wpos];
    wpos++;
    i++;
  }
  wave_pos++;
  //printf("fmod %d\n",__LINE__);
  int j = 0;
  int k = 0;
  float sum = 0.0f;
  float vusum = 0.0f;
  for (int i = 0; i < 512; ++i) {
  	sum += (*(spectrum.data))[i];
  	vusum += (*(spectrum.data))[i];
  	++j;
  	if (j == 64) {
  		j = 0;
  		sum /= 64;
  		sum *= fx_level->get();
  		switch (k) {
  			case 0:
  				octaves_l_0_p->set(sum);
  				octaves_r_0_p->set(sum);
  				break;
  			case 1:
  				octaves_l_1_p->set(sum);
  				octaves_r_1_p->set(sum);
  				break;
  			case 2:
  				octaves_l_2_p->set(sum);
  				octaves_r_2_p->set(sum);
  				break;
  			case 3:
  				octaves_l_3_p->set(sum);
  				octaves_r_3_p->set(sum);
  				break;
  			case 4:
  				octaves_l_4_p->set(sum);
  				octaves_r_4_p->set(sum);
  				break;
  			case 5:
  				octaves_l_5_p->set(sum);
  				octaves_r_5_p->set(sum);
  				break;
  			case 6:
  				octaves_l_6_p->set(sum);
  				octaves_r_6_p->set(sum);
  				break;
  			case 7:
  				octaves_l_7_p->set(sum);
  				octaves_r_7_p->set(sum);
  				break;
  		}
  		++k;
  	}
  }

	vusum /= 512;
	vusum *= fx_level->get();
	vu_l_p->set(vusum);
	vu_r_p->set(vusum);

  spectrum_p->set_p(spectrum);
  sample_l_p->set_p(sample_l);
  sample_r_p->set_p(sample_r);
//  FSOUND_SAMPLE* samp = FSOUND_GetCurrentSample(channel);
//  if (samp)
//  printf("sample length: %d\n",FSOUND_Sample_GetLength(samp));
}
};

//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************

class vsx_module_sound_sample_play : public vsx_module {
  float d_time, v_time;
  // in
  vsx_module_param_float* trigger;
  vsx_module_param_float* pitch;
  vsx_module_param_resource* filename;
  // out
  // internal
  vsx_string cur_filename;
  int i_state;
  bool first_play; // to find out base frequency
  float base_frequency;
  FMOD_CREATESOUNDEXINFO exinfo;
  FMOD_SOUND            *sound;
  FMOD_CHANNEL          *channel;
  FMOD_RESULT            result;
  int                    key, driver, numdrivers, count;
  float                    prev_trigger;
  unsigned int           version;

public:
  vsx_float_array sample_l;
  vsx_float_array sample_r;

void module_info(vsx_module_info* info)
{
  info->output = 1;

  info->identifier = "sound;sample_trigger";
#ifndef VSX_NO_CLIENT
  info->description = "Plays a sample, possibly multiple times (mp3/wav/xm etc.)";
  info->in_param_spec = "filename:resource,\
trigger:float,\
pitch:float\
";
  info->component_class = "output";
#endif
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  first_play = true;
  trigger = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"trigger");
  pitch = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"pitch");
  pitch->set(0);
  filename = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
  filename->set("");
  prev_trigger = 0;
  channel = 0;
}

bool init() {
  return true;
}

void on_delete() {
}

void run() {
  if (!fmod_init)
  {
    printf("Initializing fmod...\n");
    result = FMOD_System_Create(&fsystem);
    ERRCHECK(result);

    result = FMOD_System_GetVersion(fsystem, &version);
    ERRCHECK(result);
    //FMOD_System_SetOutput(fsystem, FMOD_OUTPUTTYPE_PULSEAUDIO);
//    FMOD_System_SetDSPBufferSize(fsystem,2048,8);
    //FMOD_System_SetDSPBufferSize(fsystem,256,8);

    ERRCHECK(result);
    result = FMOD_System_SetDriver(fsystem, 0);
    ERRCHECK(result);
    result = FMOD_System_Init(fsystem, 32, FMOD_INIT_NORMAL, NULL);
    ERRCHECK(result);
    fmod_init = true;
  }

  if (cur_filename != filename->get() || !loading_done) {
    cur_filename = filename->get();
    if (cur_filename != "") {
      if (!loading_done) {
        loading_done = true;
      } else {
        FMOD_Sound_Release(sound);
        sound = 0;
      }
      //printf("opening stream: %s\n",cur_filename.c_str());
      vsxf_handle *fp;
      if ((fp = engine->filesystem->f_open(cur_filename.c_str(), "rb")) == NULL)
        return;
      //printf("fmod %d\n",__LINE__);
      unsigned long fsize = engine->filesystem->f_get_size(fp);
      char* fdata = new char[fsize+1];
      unsigned long bread = engine->filesystem->f_read(fdata, engine->filesystem->f_get_size(fp), fp);
      //printf("fmod %d\n",__LINE__);
      if (bread == fsize) {
        memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
        exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
        //printf("fmod %d\n",__LINE__);
        exinfo.length = fsize;
        result = FMOD_System_CreateSound(fsystem, fdata, FMOD_OPENMEMORY | FMOD_SOFTWARE | FMOD_CREATESAMPLE, &exinfo, &sound);
        //if (FMOD_OK != result)
        //{
//          printf("FMOD failed System_CreateSound: %d\n",res);
          //exit(0);
        //}
        ERRCHECK(result);
        //printf("fmod %d\n",__LINE__);
      }
    }
  }
  if (sound != 0)
  if (prev_trigger <= 0.0 && trigger->get() > 0.0)
  {
    //FMOD_CHANNEL *chan = 0;
    result = FMOD_System_PlaySound(fsystem, FMOD_CHANNEL_FREE, sound, 0, &channel);
    if (first_play) {
      FMOD_Channel_GetFrequency(channel,&base_frequency);
    }
   /* float pitch_val = pitch->get();
    if (pitch_val > 1.0f) pitch_val = 1.0f;
    if (pitch_val < -1.0f) pitch_val = -1.0f;
    float new_freq = base_frequency + base_frequency*0.2f + 1 * pitch_val*base_frequency;
    FMOD_Channel_SetFrequency(channel, new_freq);*/
    //printf("fmod %d\n",__LINE__);
    ERRCHECK(result);
    //channel = FSOUND_Stream_PlayEx(FSOUND_FREE, stream, NULL, TRUE);
    //FSOUND_SetPaused(channel,FALSE);
  }
  if (channel)
  {
    float pitch_val = pitch->get();
    if (pitch_val > 1.0f) pitch_val = 1.0f;
    if (pitch_val < -1.0f) pitch_val = -1.0f;
    if (pitch_val > 0.0f) pitch_val *= 1.5f;
    float new_freq = base_frequency + 0.6 * pitch_val*base_frequency;
    FMOD_Channel_SetFrequency(channel, new_freq);
  }
  prev_trigger = trigger->get();
}
};




//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module, void* args);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules();
}


vsx_module* create_new_module(unsigned long module, void* args) {
  switch (module) {
    case 0: return (vsx_module*)(new vsx_module_sound_stream_play);
    case 1: return (vsx_module*)(new vsx_module_sound_sample_play);
  }
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_sound_stream_play*)m; break;
    case 1: delete (vsx_module_sound_sample_play*)m; break;
  }
}


unsigned long get_num_modules() {
  return 2;
}


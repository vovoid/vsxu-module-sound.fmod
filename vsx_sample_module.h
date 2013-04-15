/**
* @author Dinesh Manajipet, Vovoid Media Technologies - Copyright (C) 2012-2020
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


#include "vsx_math_3d.h"
#include "vsx_param.h"
#include "vsx_module.h"

class vsx_sample_module : public vsx_module
{
  vsx_module_param_float* param1;
  vsx_module_param_float* param2;
  vsx_module_param_float* result;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "sample;module";
    info->description = "Adds two input floats. And Returns an output float.";
    info->in_param_spec =
        "param1:float,"
        "param2:float"
    ;
    info->out_param_spec = "result:float";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    param1 = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param1");
    param2 = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param2");

    result = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result");

    param1->set(0);
    param2->set(0);
    result->set(0);

    loading_done = true;
  }

  void run()
  {
    result->set(param1->get()+param2->get());
  }

};

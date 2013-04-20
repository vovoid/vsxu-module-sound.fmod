vsxu Fmod module
==================

This project provides 2 VSXu modules:

  - stream play
  - sample trigger


Install (linux):
---

    git clone git://github.com/vovoid/vsxu-module-sound.fmod.git
    cd vsxu-module-sound.fmod
    mkdir build
    cd build
    cmake ..
    make
    make install

This automatically installs the plugin to the vsxu plugins directory.

You may need to run the "make install" command with administrator privileges.

However, if you have compiled vsxu locally (which is a better option), 
you might want to set PKG_CONFIG_PATH to override the installed vsxu in
/usr/lib/vsxu

Add this to your ~/.bash_aliases (or your other favourite bash startup script):

    export PKG_CONFIG_PATH='/home/jaw/vsxu-dev/build/linux64/install/lib/pkgconfig'

This will make cmake find the VSXu installation in the correct directory.
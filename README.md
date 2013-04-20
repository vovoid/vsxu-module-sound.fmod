vsxu sample module
==================

Sample Module For VSXu.

All it does is take 2 float params for input and outputs a result = sum(inputs)

Install:
---

    git clone git://github.com/vovoid/vsxu-module.git
    cd vsxu-module
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
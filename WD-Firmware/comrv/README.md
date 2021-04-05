
# Prerequisites

- Install scons build system
    ```
    $ sudo apt-get scons
    ```
- Download latest llvm (executables)
         [LLVM-Debian download link](https://wdc.box.com/shared/static/f9mn68lddc91ssw9vumj3hp9pdko3zd7.gz)
  See [Build llvm](#build-llvm) section as an alternative to using the prebuilt llvm version

# Configure comrv build

Open the filw  ```<path-to-comrv-directory>/build/SConstruct``` and comment/uncomment relevant comrv features under  ```Env['PUBLIC_DEF']```

# Build comrv library
    $ cd <path-to-comrv-directory>/build
    $ scons llvmpath=<path-to-llvm-directory> gccpath=<path-to-gcc-directory>
# Build llvm
TBD

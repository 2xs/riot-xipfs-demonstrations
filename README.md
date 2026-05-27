# XiPFS demonstrations.

This repository collects all XiPFS demonstrations source code.

## Pre-requisites.

### ARM GCC toolchain.

Please install the [bare-metal ARM Gnu toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) (`arm-none-eabi`) according to your OS and the target board.  
The tools must be available system-wide, either by creating aliases/symbolic links or by adding the binaries path to your PATH environment variable.

### FAE format.

Start by cloning the main branch of [FAE format git repository](https://github.com/2xs/fae_format/tree/master) and make `CRT0` and `libfae.a` components.

```
$ ls
riot_xipfs_demonstrations
$ git clone https://github.com/2xs/fae_format.git
fae_format  riot_xipfs_demonstrations
$ cd fae_format
$ git switch master
$ make
$ ls build/
crt0.elf crt0.fae crt0.o libfae.a stdriot.o
$ cd ..
```

For further details and in-deep coverage of FAE format, you can also read the [README.md](https://github.com/2xs/fae_format/blob/master/README.md) and [GETTING_STARTED.md](https://github.com/2xs/fae_format/blob/master/GETTING_STARTED.md) documents.

## directories.mk file.

To produce FAE files, XiPFS demonstrations need to know the location of FAE format directory in the host filesystem.  
At the root of XiPFS demonstrations folder, please create a `directories.mk` file next to existing `vars.mk` file.  
Edit this file to provide the expected Makefile variable `FAE_DIRECTORY_PATH` with the actual FAE format path in the filesystem.

For example :  
```
$ ls
fae_format riot_xipfs_demonstrations
$ echo FAE_DIRECTORY_PATH=/absolute/path/fae_format > riot_xipfs_demonstrations/directories.mk
$ cat riot_xipfs_demonstrations/directories.mk
FAE_DIRECTORY_PATH=/absolute/path/fae_format
```

## vars.mk file.

This file is located at the root of XiPFS demonstrations directory, and has two roles.

First, by including the aforementioned `directories.mk` file, it deduces expected paths/filenames from the `FAE_DIRECTORY_PATH` variable :  
- The path to `build_fae.py`, which creates a FAE file from an ELF one,
- The path to FAE format's C headers,
- The path to FAE format's binary components, namely `CRT0` and `libfae.a`.

All of this comes with checks to ensure that requirements are fulfilled.

The second role is to define all the common variables for demonstrations' Makefile, and especially **GCC compilation flags**.

`vars.mk` file should not need to be edited.

## Build demonstrations.

After these preliminary steps have been completed, you should be able to build demonstrations by regular means.

```
$ cd riot_xipfs_demonstrations/01-main
$ make
```

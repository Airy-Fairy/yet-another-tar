# YAT - Yet Another TAR

## Requirements
- C++ compiler with C++17
- Cmake

## Building
Building with cmake for Windows (Visual Studio) and Linux.

#### Windows
From Native Tools Command Prompt for VS:
```sh
> mkdir build
> cd build
> cmake -G "Visual Studio 15 2017 Win64" ..\src # Yours VS and Arch
> msbuild /p:Configuration=Release
```
Now you can find **yat.exe** in the build\\Release.

#### Linux
In terminal:
```sh
$ mkdir build
$ cd build
$ cmake ../src/
$ make
```
Now you can find **yat** in the build directory.

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

## Usage
In cmd/terminal:
 `yat -axli (input_path | archive) [-o (output_path | archive)]`

##### Available options:
 - **-a**   collect input data and put it into yat archive
 - **-x**   extract data from yat archive into output path
 - **-l**   list yat archive data
 - **-i**   insert data into existing yat archive
 - **-o**   output path or yat archive path for inserting (-i)
 - **-h**   get help (these options above)

##### Examples
 - Archive: `yat -a /input/path -o /archive/output/path`
 - Extract: `yat -x /path/to/archive.yat -o /output/path`
 - List: `yat -l /path/to/archive.yat`
 - Insert: `yat -i /input/path -o /path/to/archive.yat`

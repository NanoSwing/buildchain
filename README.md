# Buildchain
Buildchain is a tool for generating a makefile from parameters in a config file.

## Usage
- -g - Generate a .buildconfig template. Pass a name after to change output file from `.buildconfig`.
- -o - Change the output file from the default `Makefile`.
- -i - Change the input config file from the default `.buildconfig`.

Example of `.buildchain` file
```text
compiler     = clang;
standard     = c17;
type         = bin;
mode         = debug;
output       = bin/foo;
search_depth = 1;
libs         = somecoollib;
lib_dirs     = /path/to/library/location;
src_dirs     = src;
obj_dir      = obj;
include_dirs = include;
```

## Config file
- compiler     - What compiler is used.
- standard/std - C standard.
- type         - Type of output. Either **bin** for a binary or **lib** for a library.
- mode         - Either release or debug mode. Release, r, debug and d are all valid options.
- output       - Path to output file, including directory.
- search_depth - How many subdirectories to search with looking for source files.
- libs         - A list of libraries separated by commas.
- lib_dirs     - A list of directories containing one or multiple libraries separated by commas.
- src_dirs     - A list of directories containing source files (.c). They will be recursivly searched through with a depth of search_depth.
- obj_dir      - Directory where all .o and .d files get placed.
- include_dirs - A list of directories containing header files.

## Building
```bash
$ git clone https://github.com/NanoSwing/buildchain.git
$ cd buildchain
$ make
```

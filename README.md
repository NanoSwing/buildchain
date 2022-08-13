# Buildchain
Buildchain is a tool for generating a makefile from parameters in a config file.

Makefile functions:
- build (default) - Compiles all .c files into .o files and compiles them into either a binary or a library.
- clean           - Deletes all .o, .d, tests, and the generated binary or library.
- test_*          - Replace * with a .c name in tests_dir to generate test binary.
- test_all        - Compiles all tests and runs them.

## Usage
If no options are specified it will try to read '.buildchain' and output to 'Makefile'.

- -h file - Prints program usage.
- -g file - Generates a config file. If file is not specified it will default to .buildchain.
- -o file - Spcify an onput file. If file is not specified it will default to 'Makefile'.
- -i file - Spcify an input file. If file is not specified it will default to '.buildchain'.

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
tests_dir    = tests;
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
- tests_dir    - Directory where the command 'make test_abc' will compile tests_dir/abc.c into tests_dir/test_abc(.exe)

## Building
```bash
$ git clone https://github.com/NanoSwing/buildchain.git
$ cd buildchain
$ make
```

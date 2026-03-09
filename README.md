# tree

![Build Status](https://github.com/monakaibrahim-cmyk/tree/actions/workflows/build.yml/badge.svg)

A command-line utility that displays the contents of directories in a hierarchical, tree-like format.

## Building

**WSL**
```
1. git clone https://github.com/monakaibrahim-cmyk/tree.git
2. cd tree
3. mkdir build
4. cmake -DCMAKE_TOOLCHAIN_FILE=../mingw-toolchain.cmake -B build
5. make -C Build
```

**WINDOWS**
```
1. git clone https://github.com/monakaibrahim-cmyk/tree.git
2. cd tree
3. mkdir build
4. cmake -B build
5. cmake --build build
```

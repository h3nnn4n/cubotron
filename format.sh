#!/bin/bash

# Required because .c files are detected as C and .h as Cpp
clang-format -i --assume-filename=file.c src/*.c
clang-format -i --assume-filename=file.c src/*.h

echo "Formatting complete!"

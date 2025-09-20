#!/bin/bash

if [ "$1" = "--check" ]; then
    echo "Checking formatting..."
    
    all_good=true
    for file in src/*.c src/*.h; do
        if [ -f "$file" ]; then
            formatted=$(clang-format --assume-filename=file.c "$file")
            if ! diff -q <(echo "$formatted") "$file" > /dev/null 2>&1; then
                echo "File $file is not properly formatted"
                all_good=false
            fi
        fi
    done
    
    if [ "$all_good" = true ]; then
        echo "All files are properly formatted!"
        exit 0
    else
        echo "Some files are not properly formatted."
        exit 1
    fi
else
    echo "Formatting files..."
    clang-format -i --assume-filename=file.c src/*.c
    clang-format -i --assume-filename=file.c src/*.h
    echo "Formatting complete!"
fi

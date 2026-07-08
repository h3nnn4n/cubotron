#!/bin/bash

# Try to find clang-format (prefer version 20, fall back to default)
if command -v clang-format-20 &> /dev/null; then
    CLANG_FORMAT=clang-format-20
elif command -v clang-format &> /dev/null; then
    CLANG_FORMAT=clang-format
else
    echo "Error: clang-format not found"
    exit 1
fi

if [ "$1" = "--check" ]; then
    echo "Checking formatting..."

    all_good=true
    for file in src/*.c src/*.h; do
        if [ -f "$file" ]; then
            formatted=$($CLANG_FORMAT --assume-filename=file.c "$file")
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
    $CLANG_FORMAT -i --assume-filename=file.c src/*.c
    $CLANG_FORMAT -i --assume-filename=file.c src/*.h
    echo "Formatting complete!"
fi

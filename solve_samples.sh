#!/bin/sh

for cube in $(cat sample_cubes.txt)
do
    ./cubotron --solve $cube
done

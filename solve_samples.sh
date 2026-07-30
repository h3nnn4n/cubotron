#!/bin/sh

for cube in $(cat sample_cubes.txt)
do
    ./cubotron --solve $cube
done

echo "Solving 2x2 samples..."
for cube in $(cat sample_cubes_2x2.txt)
do
    ./cubotron --puzzle 2x2 --solve $cube
done

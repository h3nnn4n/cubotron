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

echo "Solving 2x2 scrambles (easy to hard)..."
./cubotron --puzzle 2x2 --solve-scramble "U R F U'" --max-depth=27
./cubotron --puzzle 2x2 --solve-scramble "R U F R U" --max-depth=27
./cubotron --puzzle 2x2 --solve-scramble "U R F U R F U R" --max-depth=27
./cubotron --puzzle 2x2 --solve-scramble "F R U F R U F R U" --max-depth=27
./cubotron --puzzle 2x2 --solve-scramble "R U R F U R U F R U R F" --max-depth=27

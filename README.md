# Cubotron

[![Memory Leak check](https://github.com/h3nnn4n/cubotron/actions/workflows/heap-check.yml/badge.svg)](https://github.com/h3nnn4n/cubotron/actions/workflows/heap-check.yml)
[![Benchmarks Test](https://github.com/h3nnn4n/cubotron/actions/workflows/run.yml/badge.svg)](https://github.com/h3nnn4n/cubotron/actions/workflows/run.yml)
[![Sample Solves Test](https://github.com/h3nnn4n/cubotron/actions/workflows/solves.yml/badge.svg)](https://github.com/h3nnn4n/cubotron/actions/workflows/solves.yml)
[![Unit Tests](https://github.com/h3nnn4n/cubotron/actions/workflows/tests.yml/badge.svg)](https://github.com/h3nnn4n/cubotron/actions/workflows/tests.yml)
[![clang-format](https://github.com/h3nnn4n/cubotron/actions/workflows/clang-format-check.yml/badge.svg)](https://github.com/h3nnn4n/cubotron/actions/workflows/clang-format-check.yml)
[![clang-tidy](https://github.com/h3nnn4n/cubotron/actions/workflows/clang-tidy.yml/badge.svg)](https://github.com/h3nnn4n/cubotron/actions/workflows/clang-tidy.yml)
[![cppcheck](https://github.com/h3nnn4n/cubotron/actions/workflows/cppcheck.yml/badge.svg)](https://github.com/h3nnn4n/cubotron/actions/workflows/cppcheck.yml)
[![cpplint](https://github.com/h3nnn4n/cubotron/actions/workflows/cpplint.yml/badge.svg)](https://github.com/h3nnn4n/cubotron/actions/workflows/cpplint.yml)


A simple Rubik's cube two stage solver. Does not solve to optimality, but gives
very fast solutions. On average solutions are of length 22 and takes 0.03
seconds to be found on a macbook pro 2020.

# Usage

Clone this repo, run `git submodule update --init --recursive` and run
`make build` to build the application.

Calling `./cubotron` with no args will build the prunning and move tables the
first time it is called. The tables are cached to disk. Optionally,
`--rebuild-tables` can be passed to force a rebuild of the tables.

To actually solve a cube, call `./cubotron --solve
DUDUUUDBUFRFRRBRDUBLLUFDUBFBDDFDLUFFRBLFLFBRRLLBRBDRLL`, where the long string
is the cube representation at the facelet level. The string has the 9 cube facelets
in the order U1-U9, R1-R9, F1-F9, D1-D9, L1-L9, B1-B9.

```
             |------------|
             |-U1--U2--U3-|
             |------------|
             |-U4--U5--U6-|
             |------------|
             |-U7--U8--U9-|
|------------|------------|------------|------------|
|-L1--L2--L3-|-F1--F2--F3-|-R1--R2--R3-|-B1--B2--B3-|
|------------|------------|------------|------------|
|-L4--L5--L6-|-F4--F5--F6-|-R4--R5--R6-|-B4--B5--B6-|
|------------|------------|------------|------------|
|-L7--L8--L9-|-F7--F8--F9-|-R7--R8--R9-|-B7--B8--B9-|
|------------|------------|------------|------------|
             |-D1--D2--D3-|
             |------------|
             |-D4--D5--D6-|
             |------------|
             |-D7--D8--D9-|
             |------------|
```

For example, a completely solved cube is represented by the string
`UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB`.

It is possible to set `--max-depth MAX_DEPTH` to set an upper bound on the
solution length. Any values equal or greater than 20 should finish in a couple
of seconds. 19 or lower will take significantly longer, and also runs into the
possibility of never finding a solution (i.e. optimal solution is 19 moves but
`MAX_DEPTH` is 18). For reference, the
`DUDUUUDBUFRFRRBRDUBLLUFDUBFBDDFDLUFFRBLFLFBRRLLBRBDRLL` scramble solves in
0.03s with 22 moves, 0.03s with 21 moves, 10s with 20 moves and 4~hours for 19
moves. The solution with length 18 took 35 hours.

See [this](http://kociemba.org/cube.htm) for more information.

Running `./cubotron --benchmarks` will solve as many cube as possible in 5
seconds, then solve 100 sample cubes (taken from the Cube Explorer), and
finally try to do as many moves as possible in 1 seconds. The throughput of
each operation is given. Also, a log file is generated under `stats/log.csv`
with some data points. A jupyter notebook is provided with some basic plots
to access performance.

# Credits

Kociemba's [website](http://kociemba.org/cube.htm) and
[github](https://github.com/hkociemba/RubiksCube-TwophaseSolver).

# License

See [LICENSE](LICENSE).

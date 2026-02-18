#!/bin/bash
set -e

# Single source of truth for all CI checks
# Used by: GitHub Actions, local development, Makefile

IMAGE="cubotron-ci"
DOCKERFILE="Dockerfile.ci"

ensure_image() {
    if [ -z "$(docker images -q $IMAGE 2> /dev/null)" ]; then
        echo "Building Docker image locally..."
        docker build -f $DOCKERFILE -t $IMAGE .
    fi
}

run_cmd() {
    ensure_image
    docker run --rm -v "$(pwd):/workspace" $IMAGE bash -c "$1"
}

case "$1" in
    build)
        run_cmd "make clean && make"
        ;;

    test)
        run_cmd "make; [ -d cache ] || ./cubotron --rebuild-tables; make test"
        ;;

    solve)
        run_cmd "make; ./cubotron --solve DUDUUUDBUFRFRRBRDUBLLUFDUBFBDDFDLUFFRBLFLFBRRLLBRBDRLL --max-depth=27 --n-solutions=2 --move-blacklist='U U2 U'"
        ;;

    solve-samples)
        run_cmd "make; ./solve_samples.sh"
        ;;

    benchmark)
        run_cmd "make; [ -d cache ] || ./cubotron --rebuild-tables; ./cubotron --benchmark-fast"
        ;;

    cpplint)
        run_cmd "cpplint --filter=-build/include_subdir,-readability/nolint,-whitespace/line_length,-whitespace/comments,-readability/casting,-build/header_guard,-runtime/arrays src/*.c"
        ;;

    cppcheck)
        run_cmd "cppcheck --enable=all --suppress=unusedFunction --suppress=missingIncludeSystem --suppress=staticFunction --suppress=nullPointerOutOfMemory --suppress=nullPointerOutOfResources --suppress=constParameterCallback --suppress=normalCheckLevelMaxBranches --suppress=unmatchedSuppression --std=c11 --language=c --error-exitcode=1 src/*.c"
        ;;

    clang-format)
        run_cmd "./format.sh --check"
        ;;

    format)
        run_cmd "./format.sh"
        ;;

    heapcheck-solve)
        run_cmd "git config --global --add safe.directory /workspace; git submodule update --init --recursive; make gperftools; HEAPCHECK=normal ./cubotron --solve DUDUUUDBUFRFRRBRDUBLLUFDUBFBDDFDLUFFRBLFLFBRRLLBRBDRLL --max-depth=21 --n-solutions=5"
        ;;

    heapcheck-solve-blacklist)
        run_cmd "git config --global --add safe.directory /workspace; git submodule update --init --recursive; make gperftools; HEAPCHECK=normal ./cubotron --solve DUDUUUDBUFRFRRBRDUBLLUFDUBFBDDFDLUFFRBLFLFBRRLLBRBDRLL --max-depth=27 --n-solutions=5 --move-blacklist='U U2 U'"
        ;;

    heapcheck-benchmark)
        run_cmd "git config --global --add safe.directory /workspace; git submodule update --init --recursive; make gperftools; HEAPCHECK=normal ./cubotron --benchmark-fast"
        ;;

    lint-all)
        "$0" cpplint
        "$0" cppcheck
        "$0" clang-format
        echo "All linters passed!"
        ;;

    all)
        "$0" build
        "$0" test
        "$0" solve
        "$0" solve-samples
        "$0" benchmark
        "$0" lint-all
        "$0" heapcheck-solve
        "$0" heapcheck-solve-blacklist
        "$0" heapcheck-benchmark
        echo "All CI checks passed!"
        ;;

    *)
        echo "Usage: $0 {build|test|solve|solve-samples|benchmark|cpplint|cppcheck|clang-format|format|heapcheck-solve|heapcheck-solve-blacklist|heapcheck-benchmark|lint-all|all}"
        exit 1
        ;;
esac

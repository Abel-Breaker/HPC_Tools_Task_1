#!/bin/sh
clear

# module load cesga/2020 intel/2021.3.0

set -e

make rebuild CC=gcc MODE=release EXTRA_CFLAGS="-O3 -march=native -g3 -fno-omit-frame-pointer -ffast-math "
# -fopt-info-vec-all


#./build/program 50000 300
#
perf record -e cycles,instructions,cache-misses,branches,branch-misses -g ./build/program 2000 2000

#perf record  -g ./build/program 50000 300

#perf stat -B -d ./build/program 2000 2000

#####
#make rebuild CC=gcc MODE=release EXTRA_CFLAGS="-O3 -march=native -g3 -fno-omit-frame-pointer -mno-avx512f"

#valgrind --tool=cachegrind ./build/program 2000 2000
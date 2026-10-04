#!/bin/sh
clear

# module load cesga/2020 intel/2021.3.0

set -e

make rebuild CC=gcc MODE=debug # EXTRA_CFLAGS="-O2 -march=native"

./build/program 200 20
#!/bin/sh
clear

# module load cesga/2020 intel/2021.3.0

set -e

make rebuild CC=gcc

./build/program 20000 50
#!/bin/bash/
CUR_DIR="$(pwd)"
PATH2SRC="$CUR_DIR/../../src"
LIBS="-fopenmp"
OUTPUTDIR="."
MAIN_FILE="$PATH2SRC/omp_hello.cpp"

g++ -std=c++0x -o $OUTPUTDIR/openmp_test $MAIN_FILE $LIBS

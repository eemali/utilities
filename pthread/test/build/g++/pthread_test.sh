#!/bin/bash/
CUR_DIR="$(pwd)"
PATH2SRC="$CUR_DIR/../../src"

OUTPUTDIR="."
MAIN_FILE="$PATH2SRC/pthread_test.cpp"
LIBS="-pthread"
g++ -std=c++0x -o $OUTPUTDIR/pthread_test $MAIN_FILE $LIBS

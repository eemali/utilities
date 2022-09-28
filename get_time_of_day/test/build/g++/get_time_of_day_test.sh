#!/bin/bash/
CUR_DIR="$(pwd)"
PATH2UTILITY="$CUR_DIR/../../.."
PATH2SRC="$CUR_DIR/../../src"

UTILITY_SRCS=""
UTILITY_SRCS="$UTILITY_SRCS $PATH2UTILITY/src/gettimeofday.cpp"

ALL_SRCS=""
ALL_SRCS="$ALL_SRCS $UTILITY_SRCS"

INCLUDEPATH=""
LIBPATH=""
LIBS=""
OUTPUTDIR="."
MAIN_FILE="$PATH2SRC/get_time_of_day_test.cpp"

g++ -std=c++0x -o $OUTPUTDIR/get_time_of_day_test $ALL_SRCS $MAIN_FILE $INCLUDEPATH $LIBS -Wl,-rpath=$LIBPATH

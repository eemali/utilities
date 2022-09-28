#!/bin/bash/
CUR_DIR="$(pwd)"
PATH2SRC="$CUR_DIR/../../src"

INCLUDEPATH=""
INCLUDEPATH="$INCLUDEPATH -I $CUR_DIR/../../../install/nlohmann_json/include"
INCLUDEPATH="$INCLUDEPATH -I $CUR_DIR/../../../install/xsimd/include"
INCLUDEPATH="$INCLUDEPATH -I $CUR_DIR/../../../install/xtensor/include"
INCLUDEPATH="$INCLUDEPATH -I $CUR_DIR/../../../install/xtensor-io/include"
INCLUDEPATH="$INCLUDEPATH -I $CUR_DIR/../../../install/xtl/include"

LIBPATH=""
LIBS=""
OUTPUTDIR="."
MAIN_FILE="$PATH2SRC/xtensor_test.cpp"

g++ -o $OUTPUTDIR/xt_test $ALL_SRCS $MAIN_FILE $INCLUDEPATH $LIBS -Wl,-rpath=$LIBPATH 

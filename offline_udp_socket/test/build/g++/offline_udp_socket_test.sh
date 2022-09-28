#!/bin/bash/
CUR_DIR="$(pwd)"
PATH2NETWORK="$CUR_DIR/../../.."
PATH2SRC="$CUR_DIR/../../src"

NETWORK_SRCS=""
NETWORK_SRCS="$NETWORK_SRCS $PATH2NETWORK/src/cOfflineUDPSocket.cpp"

ALL_SRCS=""
ALL_SRCS="$ALL_SRCS $NETWORK_SRCS"

INCLUDEPATH=""
LIBPATH=""
LIBS=""
OUTPUTDIR="."
MAIN_FILE="$PATH2SRC/offline_udp_socket_test.cpp"

g++ -std=c++0x -o $OUTPUTDIR/offline_udp_socket_test $ALL_SRCS $MAIN_FILE $INCLUDEPATH $LIBS -Wl,-rpath=$LIBPATH

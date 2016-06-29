#!/bin/bash

export PPATH=`pwd`

mkdir -p "$PPATH/libs/$1"

export LIBPATH="$PPATH/libs/$1"

if [ -f $LIBPATH/libosmscout.a ] ; 
then
echo "Library already compiled"
else
cd libosmscout 
make clean && ./configure --disable-shared --enable-static && make
cp ./src/.libs/*.a "$LIBPATH/"
cd ..
fi
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$PPATH/libosmscout
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PPATH/libosmscout/src/.libs


# compile libosmscoutmap

if [ -f $LIBPATH/libosmscoutmap.a ] ; 
then
echo "Library already compiled"
else
echo "----- STARTING GENERATION OF libosmscoutmap -----"
cd libosmscout-map
make clean && ./configure --disable-shared --enable-static && make
cp ./src/.libs/*.a "$LIBPATH/"
cd ..
fi
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$PPATH/libosmscout-map
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PPATH/libosmscout-map/src/.libs


# compile libosmscout-map-qt
if [ -f $LIBPATH/libosmscoutmapqt.a ] ; 
then
echo "Library already compiled"
else
echo "----- STARTING GENERATION OF libosmscoutmapQT -----"
cd libosmscout-map-qt
make clean && ./configure --disable-shared --enable-static && make
cp ./src/.libs/*.a "$LIBPATH/"
cd ..
fi
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$PPATH/libosmscout-map-qt
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PPATH/libosmscout-map-qt/src/.libs

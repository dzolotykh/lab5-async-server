#!/bin/bash
SERVER_DIRECTORY=$1;
cd $SERVER_DIRECTORY;
mkdir -p build;
cp -r resources/resources build;
cp resources/amogus.txt build/amogus.txt;
cd build;
cmake -DCMAKE_BUILD_TYPE=Debug ..;
cmake --build .;
./async-server;
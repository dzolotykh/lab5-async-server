#!/bin/bash
SERVER_DIRECTORY=$1;
cd $SERVER_DIRECTORY;
mkdir -p build;
cp -r graphgen/resources build;
cd build;
cmake -DCMAKE_BUILD_TYPE=Debug ..;
cmake --build .;

#!/bin/bash

sudo apt install build-essential cmake pkg-config libspeex-dev libgsm1-dev libasound2-dev uuid-dev libsrtp2-dev

cd ..

if [ ! -d third_party/pjproject ]; then
git clone https://github.com/pjsip/pjproject.git third_party/pjproject
cat resources/config_site.h > third_party/pjproject/pjlib/include/pj/config_site.h
cd third_party/pjproject
./configure --enable-epoll CFLAGS="-fPIC" CXXFLAGS="-fPIC"
make dep
make -j$(nproc)
sudo make install
fi

if [ -d build ]; then
        rm -rf build
fi

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
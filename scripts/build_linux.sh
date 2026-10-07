#!/bin/bash

sudo apt install build-essential cmake pkg-config libspeex-dev libgsm1-dev libasound2-dev uuid-dev libsrtp2-dev

cd ..

if [ ! -d third_party/pjproject ]; then
git clone https://github.com/pjsip/pjproject.git third_party/pjproject
cd third_party/pjproject
./configure --enable-epoll CFLAGS="-fPIC" CXXFLAGS="-fPIC"
cat << EOF > pjlib/include/pj/config_site.h
#define PJSUA_MAX_ACC 20000
#define PJSUA_MAX_CALLS 10000

#define PJSIP_MAX_TSX_COUNT (32768-1)
#define PJSIP_MAX_DIALOG_COUNT (32768-1)

#define PJ_IOQUEUE_MAX_HANDLES 32768

#define PJSUA_MAX_CONF_PORTS 12000
EOF
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

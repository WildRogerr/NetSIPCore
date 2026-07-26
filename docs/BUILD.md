# Building NetSIPCore

This document describes how to build NetSIPCore from source.

---

## Requirements

### Compiler and build tools

### Windows

Required:

* Visual Studio 2022
* MSVC v143
* CMake 3.20+
* Windows SDK

### Linux

Required:

* GCC 11+
* CMake 3.20+
* Make or Ninja

Additional packages:

```bash
sudo apt install \
    build-essential \
    cmake \
    libspeex-dev \
    libgsm1-dev \
    libasound2-dev \
    uuid-dev \
    libsrtp2-dev
```

---

## Dependencies

NetSIPCore uses:

* PJSIP / PJSUA2
* C++17 standard
* PJPROJECT built as static libraries

The PJSIP source code must be located at:

```
third_party/pjproject
```

Before building NetSIPCore, PJPROJECT must be configured and compiled.

Example:

```bash
cd third_party/pjproject

./configure \
    --enable-shared=no

make dep
make
```

After successful compilation, static libraries should be available:

```
pjlib/lib
pjlib-util/lib
pjmedia/lib
pjnath/lib
pjsip/lib
third_party/lib
```

---

## Build

Clone repository:

```bash
git clone https://github.com/WildRogerr/NetSIPCore.git
cd NetSIPCore
```

Create build directory:

```bash
cmake -B build
```

Build:

```bash
cmake --build build --config Release
```

---

## Linux build

Linux build requires linking against PJPROJECT static libraries.

The build system expects libraries:

```
libpjsua2
libpjsua
libpjsip
libpjsip-ua
libpjsip-simple
libpjmedia
libpjmedia-codec
libpjmedia-audiodev
libpjmedia-videodev
libpjnath
libpjlib-util
libpj

libgsmcodec
libspeex
libilbccodec
libg7221codec
libresample
libsrtp
libwebrtc
libyuv
```

They are generated after building PJPROJECT.

---

## Output files

The executable will be generated as:

### Windows

```
build/Release/NetSIPCore.exe
```

### Linux

```
build/NetSIPCore
```

---

## Runtime files

### Windows

The executable requires Microsoft runtime libraries:

```
vcruntime140.dll
msvcp140.dll
ucrtbase.dll
```

Install:

* Microsoft Visual C++ Redistributable 2015–2022

Download:

https://learn.microsoft.com/cpp/windows/latest-supported-vc-redist

---

## Linux runtime dependencies

Linux requires:

* ALSA
* pthread
* UUID
* dynamic loader libraries

Install:

```bash
sudo apt install \
    libasound2 \
    uuid-runtime
```

---

## Microphone permissions

If microphone access is blocked by antivirus software, endpoint protection, or operating system security settings:

* allow microphone access for NetSIPCore
* add the executable to trusted applications

Otherwise audio transmission may not work.

---

## Project structure

Expected third-party layout:

```
NetSIPCore
│
├── src
│
├── third_party
│   └── pjproject
│       ├── pjlib
│       ├── pjlib-util
│       ├── pjmedia
│       ├── pjnath
│       ├── pjsip
│       └── third_party
│
└── CMakeLists.txt
```

---

## Notes

NetSIPCore communicates only through its TCP JSON interface.

The executable has no GUI.

The application uses PJSUA2 API from PJPROJECT for SIP signaling and RTP audio handling.

# Building NetSIPCore

This document describes how to build NetSIPCore from source.

---

## Requirements

### Compiler

Windows

- Visual Studio 2022
- MSVC v143
- CMake 3.20+

Linux

- GCC 11+
- CMake 3.20+
- Make or Ninja

---

## Dependencies

NetSIPCore uses:

- PJSIP / PJSUA2
- C++17

---

## Build

Clone repository

```bash
git clone https://github.com/WildRogerr/NetSIPCore.git
cd NetSIPCore
```

Configure

```bash
cmake -B build
```

Build

```bash
cmake --build build --config Release
```

The executable will be generated as:

Windows

```
build/Release/NetSIPCore.exe
```

Linux

```
build/NetSIPCore
```

---

## Runtime files

The executable requires:

Windows

```
vcruntime140.dll
msvcp140.dll
ucrtbase.dll
```

These libraries are installed automatically with:

- Microsoft Visual C++ Redistributable 2015–2022

Download:

https://learn.microsoft.com/cpp/windows/latest-supported-vc-redist

---

## Microphone permissions

If microphone access is blocked by antivirus software, endpoint protection, or Windows security software:

- add NetSIPCore.exe to the trusted applications list
- allow microphone access for the application

Otherwise audio transmission may not work.

---

## Notes

NetSIPCore communicates only through its TCP JSON interface.

The executable has no GUI.
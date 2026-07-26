# Installation

This document describes how to install and run NetSIPCore.

---

# Supported platforms

NetSIPCore currently supports:

- Windows (x64)
- Linux (x64)

---

# Windows

## Requirements

- Windows 10 or newer
- Microsoft Visual C++ Redistributable 2015–2022 (x64)

Download:

https://learn.microsoft.com/cpp/windows/latest-supported-vc-redist

Without the Visual C++ Runtime, Windows may report errors such as:

```
The program can't start because VCRUNTIME140.dll is missing.
```

or

```
MSVCP140.dll was not found.
```

Installing the Visual C++ Redistributable resolves these errors.

---

## Files

A standalone Windows distribution contains:

```
NetSIPCore.exe
```

Run the executable directly:

```
NetSIPCore.exe
```

When using the official Python SDK, the required Windows binary is included automatically with the Python package.

No manual binary installation is required.

---

## Firewall

On first launch Windows Firewall may ask whether NetSIPCore is allowed to communicate over the network.

Allow access for private networks.

NetSIPCore requires network access for:

- SIP communication
- RTP audio streams

---

## Antivirus software

Some antivirus software may restrict:

- microphone access
- audio playback
- RTP traffic
- execution of unknown applications

If microphone access or RTP playback does not work correctly, add **NetSIPCore.exe** (or your application) to the trusted applications list of your security software.

---

# Linux

## Requirements

Typical runtime dependencies:

- glibc
- libstdc++
- pthread
- PJSIP runtime dependencies

No Visual C++ Runtime is required.

---

## Files

A standalone Linux distribution contains:

```
NetSIPCore
```

Make the executable runnable:

```bash
chmod +x NetSIPCore
```

Run:

```bash
./NetSIPCore
```

When using the official Python SDK, the required Linux binary is included automatically with the Python package.

No manual binary installation is required.

---

## Firewall

If your firewall blocks SIP or RTP traffic, allow the required ports.

Default SIP traffic:

```
UDP 5060
```

RTP ports depend on the SIP server configuration.

---

# SIP requirements

NetSIPCore requires access to a SIP server.

Example:

```
SIP Server
    |
    |
    +-- UDP 5060
    |
    +-- RTP Audio
```

Required:

- SIP server address
- username
- password
- network connectivity

---

# Audio requirements

Supported audio devices depend on the platform.

Windows:

- WASAPI

Linux:

- ALSA

---

# Default TCP control interface

NetSIPCore listens on:

```
127.0.0.1:4890
```

This interface is used by the official Python SDK.

Communication is performed using JSON commands over TCP.

---

# Verifying installation

Run NetSIPCore.

A successful startup should produce output similar to:

```
SIPCore initialized
TCP server started
```

The engine is now ready to receive JSON commands.

---

# Troubleshooting

## Visual C++ Runtime missing

Install:

Microsoft Visual C++ Redistributable 2015–2022 (x64)

Download:

```
https://learn.microsoft.com/cpp/windows/latest-supported-vc-redist
```

---

## No microphone

Check:

- Windows Privacy Settings
- Antivirus trusted applications
- Audio device availability
- Correct audio backend configuration

---

## No sound

Check:

- speaker is not muted
- output device is available
- SIP call is established
- RTP stream is active

---

## Cannot register SIP account

Verify:

- SIP server address
- username
- password
- network connectivity
- firewall settings

---

## TCP connection refused

Ensure that:

- NetSIPCore is running
- TCP port 4890 is available
- no other process uses the same port

---

# Python SDK

The official Python SDK is available separately.

Install:

```bash
pip install netsip
```

The Python package includes the required NetSIPCore native binary automatically.

Supported platforms:

- Windows x64
- Linux x64

The SDK communicates with NetSIPCore using:

```
TCP 127.0.0.1:4890
```

---

# Next steps

See:

```
:contentReference[oaicite:0]{index=0}
```

for the TCP JSON protocol.

See:

```
:contentReference[oaicite:1]{index=1}
```

for the Python SDK documentation.

See:

```
:contentReference[oaicite:2]{index=2}
```

for example applications.
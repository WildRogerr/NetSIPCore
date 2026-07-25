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

A typical Windows distribution contains:

```
NetSIPCore.exe
```

If you are using the Python SDK, the executable should be placed next to the SDK module as described in the Python package documentation.

---

## Firewall

On first launch Windows Firewall may ask whether NetSIPCore is allowed to communicate over the network.

Allow access for private networks.

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

Typical dependencies:

- glibc
- libstdc++
- pthread
- PJSIP runtime dependencies

No Visual C++ Runtime is required.

---

## Permissions

Make the executable runnable:

```bash
chmod +x NetSIPCore
```

---

## Firewall

If your firewall blocks SIP or RTP traffic, allow the required ports.

---

# SIP requirements

NetSIPCore requires access to a SIP server.

Example:

```
SIP Server
    │
    ├── UDP 5060
    └── RTP
```

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

---

# Verifying installation

Run NetSIPCore.

A successful startup should produce output similar to:

```
CONNECTING...
TCP connected
```

The engine is now ready to receive JSON commands.

---

# Troubleshooting

## Visual C++ Runtime missing

Install:

Microsoft Visual C++ Redistributable 2015–2022 (x64)

---

## No microphone

Check:

- Windows Privacy Settings
- Antivirus trusted applications
- Audio device availability

---

## No sound

Check:

- speaker is not muted
- output device is available
- RTP stream is established

---

## Cannot register SIP account

Verify:

- SIP server address
- username
- password
- network connectivity
- firewall

---

## TCP connection refused

Ensure that:

- NetSIPCore is running
- no other process is using TCP port 4890

---

# Next steps

See:

```
docs/JSON.md
```

for the TCP protocol.

See:

```
docs/API.md
```

for the Python SDK.

See:

```
examples/
```

for example applications.
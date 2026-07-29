# NetSIPCore

Cross-platform SIP engine based on **PJSUA2** with a TCP JSON API and an official Python SDK.

Current version:
1.0.1

Supported platforms:

- Windows x64
- Linux x64

NetSIPCore is designed for developers who need a programmable SIP engine that can be easily integrated into automation systems, testing environments, desktop applications, or custom VoIP software.

---

## Features

- Multiple SIP account registration
- Multiple simultaneous calls
- Incoming and outgoing calls
- RTP WAV audio playback
- Automatic conversation mode
- Speaker mute / unmute
- Microphone mute / unmute
- TCP JSON API
- Official Python SDK
- Windows support
- Linux support

---

## Architecture

```
                Application
                     │
                     │
     ┌───────────────┴───────────────┐
     │                               │
Python SDK                     Any language
(NetSIP)                       TCP Client
     │                               │
     └───────────────┬───────────────┘
                     │
             JSON over TCP
                     │
                     ▼
               NetSIPCore
                     │
                     ▼
                  PJSUA2
                     │
                     ▼
                   PJSIP
                     │
                     ▼
                 SIP Server
```

NetSIPCore is completely independent of the Python SDK.

Any programming language capable of sending and receiving JSON over TCP can control the SIP engine.

---

## Repository structure

```
src/
    NetSIPCore source code

python/
    Official Python SDK

examples/
    Example applications

docs/
    Documentation

LICENSE
README.md
```

---

## Building

Build instructions are available in:

```
docs/BUILD.md
```

---

## Installation

Installation instructions for Windows and Linux are available in:

```
docs/INSTALL.md
```

---

## Python SDK

The official Python SDK is located in:

```
python/
```

After installation:

```python
from netsip import SIPManager
```

See:

```
python/examples/
```

for usage examples.

---

## TCP JSON API

NetSIPCore is controlled using a newline-delimited JSON protocol over TCP.

Protocol documentation:

```
docs/JSON.md
```

---

## API Documentation

Python SDK documentation:

```
docs/API.md
```

---

## Documentation

| Document | Description |
|----------|-------------|
| BUILD.md | Building NetSIPCore |
| INSTALL.md | Installation and runtime requirements |
| API.md | Python SDK API |
| JSON.md | TCP JSON protocol |
| ARCHITECTURE.md | Internal architecture |

---

## License

NetSIPCore is distributed under the GNU General Public License version 2.

See the LICENSE file for details.
# NetSIP Python SDK

Python SDK for controlling the NetSIPCore SIP engine.

NetSIP is a Python interface for building SIP applications using the native NetSIPCore engine.

The SDK provides asynchronous control of SIP accounts, calls, audio devices and RTP audio playback.

---

## Features

* SIP account registration
* SIP account disconnection
* Incoming and outgoing calls
* Call answering and hang up
* Speaker control
* Microphone control
* WAV audio playback into RTP stream
* Automatic conversation mode
* NetSIPCore log access


---

## Architecture

The Python SDK is a controller for the NetSIPCore native SIP engine.

```
Python Application
        |
        |
        | JSON over TCP
        |
        v
NetSIPCore
        |
        v
PJSUA2
        |
        v
PJSIP
```

The SDK does not communicate with PJSIP directly.

All SIP operations are performed by NetSIPCore.


---

## Requirements

* Python 3.10+
* NetSIPCore binary
* SIP server compatible with PJSIP


The Python package communicates with NetSIPCore using:

```
TCP 127.0.0.1:4890
```


---

# Installation

## Install from PyPI

```bash
pip install netsip
```


## Install from source

```bash
git clone https://github.com/WildRogerr/NetSIPCore.git

cd NetSIPCore/python

pip install .
```


---

# NetSIPCore binary

The Python SDK requires the NetSIPCore SIP engine.

The native engine is distributed separately.

Download binaries from:

```
https://github.com/WildRogerr/NetSIPCore/releases
```


Supported platforms:

* Windows x64
* Linux x64


The SDK automatically searches for NetSIPCore in the package directory:

```
netsip/
    __init__.py
    netsip.py
    NetSIPCore.exe
```


---

# Windows requirements

Windows builds require Microsoft Visual C++ Runtime libraries.

Required files:

```
vcruntime140.dll
msvcp140.dll
ucrtbase.dll
```


Install Microsoft Visual C++ Redistributable 2015-2022:

```
https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist
```


---

# Microphone permissions

Some security software can block microphone access.

If microphone access is unavailable:

1. Add NetSIPCore executable to trusted applications.
2. Allow microphone access in Windows privacy settings.
3. Restart the application.


Windows:

```
Settings
 → Privacy & security
 → Microphone
 → Allow desktop apps
```


---

# Quick start

```python
import asyncio

from netsip import SIPManager


async def main():

    sip = SIPManager()


    await sip.subscriber_registration(
        server="192.168.1.10",
        number="1001",
        password="password"
    )


    if sip.registration_state:

        print("Registered")


        await sip.call(
            "1001",
            "1002"
        )


        await asyncio.sleep(10)


        await sip.hang_up(
            "1001"
        )


    sip.close_sipcore()



asyncio.run(main())
```


---

# API

## Create manager

```python
sip = SIPManager()
```

Creates NetSIPCore process and initializes communication.


---

## Registration

Register SIP account:

```python
await sip.subscriber_registration(
    server,
    number,
    password
)
```


Disconnect account:

```python
await sip.subscriber_disconnect(
    number
)
```


---

## Calls

Make outgoing call:

```python
await sip.call(
    number,
    remote_number
)
```


Answer incoming call:

```python
await sip.answer(
    number
)
```


Hang up:

```python
await sip.hang_up(
    number
)
```


---

## Audio control

Speaker:

```python
await sip.mute(number)

await sip.unmute(number)
```


Microphone:

```python
await sip.micoff(number)

await sip.micon(number)
```


---

# WAV playback

Send WAV audio into an active RTP call:

```python
await sip.send_audio(
    number,
    wav_file
)
```


The path should be absolute.

Example:

```python
from pathlib import Path


wav_file = Path(
    "audio/message.wav"
).resolve()


await sip.send_audio(
    "1001",
    str(wav_file)
)
```


Windows example:

```python
from pathlib import Path


wav_file = Path(
    r"C:\Audio\message.wav"
).resolve()


await sip.send_audio(
    "1001",
    str(wav_file)
)
```


---

# Automatic conversation mode

NetSIP supports automatic WAV playback during active calls.


Available files:

```python
sip.wf_a
sip.wf_b
sip.wf_c
```


Enable:

```python
sip.wf_a = "audio/message.wav"

sip.autospeak = True
```


Disable:

```python
sip.autospeak = False
```


---

# Client state

Registered accounts are available in:

```python
sip.clients
```


Example:

```python
sip.clients["1001"]
```


Possible fields:

```
server
username
current_state
audio_state
remote_number
```


Call states:

```
registered
incoming
ringing
confirmed
streaming
disconnected
```


---

# Logs

NetSIPCore logs are available:

```python
sip.sipcore_log
```


Example:

```python
for line in sip.sipcore_log:
    print(line)
```


---

# Examples

Example application:

```
examples/simplephone.py
```


The example demonstrates:

* SIP registration
* outgoing calls
* incoming calls
* RTP WAV playback
* speaker control
* microphone control
* automatic conversation mode


---

# Documentation

NetSIPCore documentation:

```
docs/
 ├── API.md
 ├── JSON.md
 ├── BUILD.md
 └── ARCHITECTURE.md
```


---

# License

NetSIP Python SDK is distributed under the GNU General Public License version 2.

See:

```
LICENSE
```

for the full license text.
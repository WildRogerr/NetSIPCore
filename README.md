NetSIPCore

Cross-platform SIP engine based on PJSUA2 with Python SDK.

Features

• Multiple SIP accounts
• Multiple simultaneous calls
• RTP WAV playback
• Automatic microphone/speaker control
• TCP JSON API
• Python SDK
• Windows
• Linux

Architecture

Python
     │
     │ JSON over TCP
     ▼
NetSIPCore
     │
     ▼
PJSUA2
     │
     ▼
PJSIP

Repository structure

src/
    SIP engine

python/
    Python SDK

docs/
    Protocol documentation

Building

...

Python

pip install netsip

Quick example

from netsip import SIPManager

...

Documentation

See docs/API.md
See docs/JSON.md

License

GPLv2
# NetSIPCore Architecture

NetSIPCore separates SIP processing from application logic.

```
Application
      │
      │ JSON
      ▼
TCP Server
      │
      ▼
Command Parser
      │
      ▼
SIPCore
      │
      ▼
PJSUA2
      │
      ▼
PJSIP
```

---

# Components

## TCP Server

Responsibilities

- accepts client connection
- receives JSON commands
- sends state updates

---

## Command Parser

Responsibilities

- parses JSON
- validates commands
- dispatches actions

---

## SIPCore

Responsible for

- SIP accounts
- call management
- media control
- audio playback

---

## SIPAccount

Represents one SIP account.

Responsibilities

- registration
- registration state callbacks
- incoming call callbacks

---

## SIPCall

Represents one active call.

Responsibilities

- call state
- media state
- RTP playback
- mute/unmute

---

## Audio Player

Responsible for

- WAV playback
- RTP transmission
- playback completion callbacks

---

# Threading Model

Main thread

- PJSUA2
- SIP processing

TCP thread

- JSON communication

Audio thread

- automatic playback

These threads communicate through thread-safe queues and callbacks.

---

# State Flow

```
registration

↓

registered

↓

call

↓

ringing

↓

confirmed

↓

streaming

↓

disconnected
```

---

# Design Goals

- lightweight
- cross-platform
- simple TCP API
- language independent
- reusable from Python, C#, Java, Go, Rust, etc.
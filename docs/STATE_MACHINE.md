# NetSIPCore State Machine

This document describes the SIP account and call state transitions reported by NetSIPCore.

The `state` field is included in every status message sent by the engine.

---

# Registration

```
          registration
                │
                ▼
         registering
                │
                ▼
          registered
```

After a successful registration, the account is ready to make or receive calls.

---

# Outgoing Call

```
registered
     │
     │ call
     ▼
 ringing
     │
     ▼
confirmed
     │
     ▼
streaming
     │
     ▼
disconnected
     │
     ▼
registered
```

---

# Incoming Call

```
registered
     │
     ▼
 incoming
     │
     │ answer
     ▼
confirmed
     │
     ▼
streaming
     │
     ▼
disconnected
     │
     ▼
registered
```

If the incoming call is rejected or the remote party hangs up before the call is answered:

```
incoming
    │
    ▼
disconnected
    │
    ▼
registered
```

---

# State Descriptions

## registering

A SIP REGISTER request has been sent.

The account is waiting for a response from the SIP server.

---

## registered

The SIP account has been successfully registered.

Outgoing and incoming calls are now available.

---

## incoming

An incoming INVITE request has been received.

The application may answer or reject the call.

---

## ringing

An outgoing INVITE has been sent.

The remote endpoint is ringing.

---

## confirmed

The call has been established.

Media negotiation has completed successfully.

---

## streaming

RTP media is active.

Audio can now be:

- transmitted
- received
- played from WAV files

---

## disconnected

The call has ended.

Possible reasons include:

- local hangup
- remote hangup
- rejected call
- call failure
- network error

After this state the account automatically returns to:

```
registered
```

and is ready for the next call.

---

# Audio State

Each state message also contains:

```
audio_state
```

Possible values:

| Value | Description |
|--------|-------------|
| stop | No RTP playback |
| play | RTP WAV playback active |

---

# Remote Number

The state message also contains:

```
remote
```

Examples:

```
1002
```

or

```
sip:1002@pbx.company.com
```

This field identifies the remote party associated with the current call.

---

# Example

Typical outgoing call sequence:

```
registering

↓

registered

↓

ringing

↓

confirmed

↓

streaming

↓

disconnected

↓

registered
```

Typical incoming call sequence:

```
registered

↓

incoming

↓

confirmed

↓

streaming

↓

disconnected

↓

registered
```
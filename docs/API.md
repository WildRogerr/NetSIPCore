# NetSIPCore API

NetSIPCore exposes a TCP JSON API.

Default endpoint:

```
127.0.0.1:4890
```

Messages are encoded as UTF-8 JSON.

Each JSON object must end with:

```
\n
```

---

# Commands

## registration

Registers a SIP account.

Required fields

| Field | Type |
|--------|------|
| command | string |
| server | string |
| proxy | string |
| username | string |
| auth_username | string |
| password | string |

Example

```json
{
    "command":"registration",
    "server":"pbx.company.com",
    "proxy":"server:port",
    "username":"1001",
    "auth_username":"1001@pbx.company.com",
    "password":"secret"
}

If proxy is omitted or empty, the account is registered without a SIP proxy.

If auth_username is omitted or empty, the account is registered using the username as the auth_username.
```

---

## disconnect

Disconnects an account.

```json
{
    "command":"disconnect",
    "username":"1001"
}
```

---

## call

Starts outgoing call.

```json
{
    "command":"call",
    "username":"1001",
    "remote":"1002"
}
```

---

## answer

Answers incoming call.

```json
{
    "command":"answer",
    "username":"1001"
}
```

---

## hangup

Terminates current call.

```json
{
    "command":"hangup",
    "username":"1001"
}
```

---

## mute

Disables audio input or output for the current call.

Fields

| Field | Values |
|--------|--------|
| device | speaker / microphone |

Example

```json
{
    "command":"mute",
    "username":"1001",
    "device":"speaker"
}
```

---

## unmute

Enables audio input or output for the current call.

```json
{
    "command":"unmute",
    "username":"1001",
    "device":"microphone"
}
```

---

## send_audio

Starts RTP playback.

```json
{
    "command":"send_audio",
    "username":"1001",
    "audio_path":"C:\\Audio\\hello.wav"
}
```

---

## destroy

Stops NetSIPCore.

```json
{
    "command":"destroy"
}
```
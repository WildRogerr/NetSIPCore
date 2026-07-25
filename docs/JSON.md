# JSON Protocol

NetSIPCore communicates using newline-delimited JSON (NDJSON).

Each request and response occupies exactly one line.

Example

```
{"command":"registration",...}
{"command":"call",...}
```

---

# Responses

NetSIPCore continuously reports account and call state.

Example

```json
{
    "username":"1001",
    "state":"registered",
    "audio_state":"stop",
    "remote":""
}
```

---

# Fields

## username

Current SIP account.

Example

```
1001
```

---

## state

Possible values

| State | Description |
|--------|-------------|
| registering | Registration started |
| registered | Registration successful |
| incoming | Incoming INVITE |
| ringing | Remote endpoint ringing |
| confirmed | Call established |
| streaming | RTP streaming |
| disconnected | Call finished |

---

## audio_state

Possible values

```
stop
play
```

---

## remote

Remote SIP URI or number.

Examples

```
1002
```

```
sip:1002@pbx.company.com
```

---

# Transport

Protocol

```
TCP
```

Encoding

```
UTF-8
```

Delimiter

```
newline
```

---

# Error handling

Malformed JSON is ignored.

Unknown commands are ignored.

Network disconnection automatically causes NetSIPCore to wait for a new client connection.
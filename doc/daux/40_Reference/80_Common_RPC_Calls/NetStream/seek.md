title: seek
--------------------------

A message from the client to tell the server an offset to seek to in the stream.

## Syntax ##

```
String: "seek"
Number: 0
Null
Number: ... // Timestamp
```

## Parameters ##
#### field[0] ####
> **Type:** String
>
> The name of this command, which in this case is "seek".

#### field[1] ####
> **Type:** Number
>
> The sequence number for this command, which is always 0.

#### field[2] ####
> **Type:** Null
>
> The command information object for this call, which doesn't exist and is always null.

#### field[3] ####
> **Type:** Number
>
> The timestamp, in milliseconds, to seek to in the playlist.
>
> Example value: 10203


## Return Value ##
On success, an `onStatus` message is returned with the `NetStream.Seek.Notify` code.

Returns an `_error` message on failure.

## Remarks ##
This command follows standard [NetStream calling conventions.](/Reference/Common_RPC_Calls/NetStream/index.html)

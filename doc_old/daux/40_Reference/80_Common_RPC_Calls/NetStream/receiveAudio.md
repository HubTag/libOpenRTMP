title: receiveAudio
--------------------------

Indicates whether audio should be sent to the client or not.

## Syntax ##

```
String: "receiveAudio"
Number: 0
Null
Bool:   ... // Flag
```

## Parameters ##
#### field[0] ####
> **Type:** String
>
> The name of this command, which in this case is "receiveAudio".

#### field[1] ####
> **Type:** Number
>
> The sequence number for this command, which is always 0.

#### field[2] ####
> **Type:** Null
>
> The command information object for this call, which doesn't exist and is always null.

#### field[3] ####
> **Type:** Boolean
>
> Whether or not the server should send audio.
>
> Example value: true


## Return Value ##
If the flag is false, then this command does not return anything.

Otherwise, the response is two `onStatus` messages with `NetStream.Seek.Notify` and `NetStream.Play.Start` codes.

## Remarks ##
This command follows standard [NetStream calling conventions.](/Reference/Common_RPC_Calls/NetStream/index.html)

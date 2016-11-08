title: createStream
--------------------------

A NetConnection RPC sent by the client to request that the server create a new message stream for the publishing of audio, video, and metadata. Such publishing should only occur over message streams created with `createStream`.

## Syntax ##

```
String: "createStream"
Number: ... // Sequence Number
Object: ... // Command Parameters
```

## Parameters ##
#### field[0] ####
> **Type:** String
>
> Indicates the command name. In this case, "createStream"

#### field[1] ####
> **Type:** Number
>
> The sequence number of the command.

#### field[2] ####
> **Type:** Object
>
> An optional object containing command parameters. Usually null.

## Return Value ##
```
String: "_result" or "_error"
Number: ... // Sequence Number
Object: ... // Command Info
Number: ... // Stream ID
```

#### response[0] ####
> **Type:** String
>
> A string indicating the type of result, either "_result" to indicate success, or "_error" to indicate an error.

#### response[1] ####
> **Type:** Number
>
> The sequence number of the call which this is a response to.

#### response[2] ####
> **Type:** Object
> 
> Command info included with the response. Usually null.

#### response[3] ####
> **Type:** Number or Object
> 
> If successful, this is the ID of the message stream which was created. Otherwise, it's an error information object.


title: publish
--------------------------

Sent by the client to indicate the desire to publish a named stream.

## Syntax ##

```
String: "publish"
Number: 0
Null:
String: ... // Stream Name
String: ... // Stream Type
```

## Parameters ##
#### field[0] ####
> **Type:** String
>
> Indicates the name of the procedure to call, in this case, "publish".

#### field[1] ####
> **Type:** Number
>
> The sequence number of the command. In this case, should always be 0.

#### field[2] ####
> **Type:** Null
>
> A command info object, which doesn't exist for this command, so is always null.

#### field[3] ####
> **Type:** String
>
> The name of the stream to publish. This is synonymous with "stream key" or "playpath".
>
> Example value: "live_123192312419"

#### field[4] ####
> **Type:** String
>
> The type of stream this is, usually "live", "recorded", or "append". See stream types.
>
> Example value: "live"

## Return Value ##
Responds with an `onStatus` code of `NetStream.Publish.Start` if successful.

## Remarks ##
This command follows standard [NetStream calling conventions.](/Reference/Common_RPC_Calls/NetStream/index.html)

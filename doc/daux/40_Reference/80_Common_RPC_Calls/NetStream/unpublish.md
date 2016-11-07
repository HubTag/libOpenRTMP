title: unpublish
--------------------------

Used to indicate to the server that publishing is complete.

## Syntax ##

```
String: "unpublish"
Number: 0
Null
String: ... // Stream Name
```

## Parameters ##
#### field[0] ####
> **Type:** String
>
> The name of this command, which in this case is "releaseStream".

#### field[1] ####
> **Type:** Number
>
> The sequence number for this command, which is always 0.

#### field[2] ####
> **Type:** Null
>
> The command information object for this call, which doesn't exist and is always null.

#### field[3] ####
> **Type:** String
>
> The name of the stream to stop publishing.
>
> Example value: "live_123122314"

## Return Value ##
An `onStatus` message with a code of `NetStream.Unpublish.Success` on success.

## Remarks ##
This command follows the [NetStream calling conventions.](/Reference/Common_RPC_Calls/NetStream/index.html)

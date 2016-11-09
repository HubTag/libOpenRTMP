title: FCUnpublish
--------------------------

Used to release the name of a stream after publishing.

## Syntax ##

```
String: "FCUnpublish"
Number: 0
Null
String: ... // Stream Name
```

## Parameters ##
#### field[0] ####
> **Type:** String
>
> The name of this command, which in this case is "FCUnpublish".

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
> The name of the stream to release.
>
> Example value: "live_123122314"

## Return Value ##
A modified `onStatus`. Instead of being called `onStatus`, it's called `onFCUnpublish`. The code is `NetStream.Unpublish.Success` on success.

## Remarks ##
This command follows slightly modified [NetStream calling conventions](/Reference/Common_RPC_Calls/NetStream/index.html), as noted in `Return Value`.

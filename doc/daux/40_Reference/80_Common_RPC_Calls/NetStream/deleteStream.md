title: deleteStream
--------------------------

Requests the destruction of a message stream.

## Syntax ##

```
String: "deleteStream"
Number: 0
Null
Number: ... // Stream ID
```

## Parameters ##
#### field[0] ####
> **Type:** String
>
> The name of this command, which in this case is "deleteStream".

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
> The message stream do delete.
>
> Example value: 1


## Return Value ##
This command does not return anything.



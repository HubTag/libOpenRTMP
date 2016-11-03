title: rtmp_message_type_t
--------------------------

Contains names for message classes which are sent over an RTMP chunk stream.


## Syntax ##

```c
typedef enum {
    //Chunk Control Messages            //Page 18, §5.4
    RTMP_MSG_SET_CHUNK_SIZE = 1,        //Page 19, §5.4.1
    RTMP_MSG_ABORT = 2,                 //Page 19, §5.4.2
    RTMP_MSG_ACK = 3,                   //Page 20, §5.4.3
    RTMP_MSG_WIN_ACK_SIZE = 5,          //Page 20, §5.4.4
    RTMP_MSG_SET_PEER_BWIDTH = 6,       //Page 21, §5.4.5

    //User Control Messages             //Page 23, §6.2
    RTMP_MSG_USER_CTL = 4,              //Page 23, §6.2

    //RTMP Command Messages             //Page 23, §7
    RTMP_MSG_AUDIO = 8,                 //Page 26, §7.1.4
    RTMP_MSG_VIDEO = 9,                 //Page 26, §7.1.5
    RTMP_MSG_AGGREGATE = 22,            //Page 26, §7.1.6
    RTMP_MSG_AMF3_DAT = 15,             //Page 24, §7.1.2
    RTMP_MSG_AMF3_SO = 16,              //Page 24, §7.1.3
    RTMP_MSG_AMF3_CMD = 17,             //Page 24, §7.1.1
    RTMP_MSG_AMF0_DAT = 18,             //Page 24, §7.1.2
    RTMP_MSG_AMF0_SO = 19,              //Page 24, §7.1.3
    RTMP_MSG_AMF0_CMD = 20              //Page 24, §7.1.1
} rtmp_message_type_t;
```

## Members ##

#### RTMP_MSG_SET_CHUNK_SIZE ####
> A chunk control message indicating a new chunk size which the sender will use from now on.
> 
> Defined in §5.4.1 on page 19

#### RTMP_MSG_ABORT ####
> A chunk control message that requests the peer abort processing of a message prematurely.
> 
> Defined in §5.4.2 on page 19

#### RTMP_MSG_ACK ####
> A chunk control message which acknowledges the number of bytes received so far.
> 
> Defined in §5.4.3 on page 20

#### RTMP_MSG_WIN_ACK_SIZE ####
> A chunk control message indicating the window size of the sender.
> 
> Defined in §5.4.4 on page 20

#### RTMP_MSG_SET_PEER_BWIDTH ####
> A chunk control message indicating to the peer that the sender wishes the peer to change their window size.
> 
> Defined in §5.4.5 on page 21

#### RTMP_MSG_USER_CTL ####
> Indicates that this is a user control message.
> 
> Defined in §6.2 on page 23

#### RTMP_MSG_AUDIO ####
> An RTMP command message which indicates that the payload contains audio.
> 
> Defined in §7.1.4 on page 26

#### RTMP_MSG_VIDEO ####
> An RTMP command message which indicates that the payload contains video.
> 
> Defined in §7.1.5 on page 26

#### RTMP_MSG_AGGREGATE ####
> An RTMP command message which indicates that this is an aggregate RTMP message.
> Aggregate RTMP messages are currently not supported.
> 
> Defined in §7.1.6 on page 26

#### RTMP_MSG_AMF3_DAT ####
> Indicates that the payload of this message is AMF3 information.
> 
> Defined in §7.1.2 on page 24

#### RTMP_MSG_AMF3_SO ####
> An RTMP command message which indicates that the payload is a serialized shared object in AMF3 format.
> 
> Defined in §7.1.3 on page 24

#### RTMP_MSG_AMF3_CMD ####
> An RTMP command message which indicates that the payload is a RPC call in AMF3 format.
> 
> Defined in §7.1.1 on page 24

#### RTMP_MSG_AMF0_DAT ####
> An RTMP command message which indicates that the payload is AMF0 information.
> 
> Defined in §7.1.2 on page 24

#### RTMP_MSG_AMF0_SO ####
> An RTMP command message which indicates that the payload is a serialized shared object in AMF0 format.
> 
> Defined in §7.1.3 on page 24

#### RTMP_MSG_AMF0_CMD ####
> An RTMP command message which indicates that the payload is a RPC call in AMF0 format.
> 
> Defined in §7.1.1 on page 24


## Remarks ##

The message types are all transmitted through a single field of the RTMP chunk header. However, there are three distinct types of messages: **chunk control messages**, **user control messages**, and **RTMP command messages**. There is an unfortunate bit over overlap between two arguably independent protocols when talking about RTMP. There is RTMP itself, and there is the RTMP chunk stream protocol, which is designed to basically be a part of RTMP. However, the one message field is used to send messages for both RTMP and the underlying RTMP chunk stream protocol.

**Chunk control messages** are used to control aspects of the RTMP chunk stream underlying an active RTMP stream.

**User control messages** are not really currently implemented.

**RTMP command messages** are used to send actual RTMP data over the RTMP chunk stream, such as audio/video content, remote procedure calls, AMF data, and so on.


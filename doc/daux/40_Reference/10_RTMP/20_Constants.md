title: Constants (rtmp/rtmp_constants.h)
--------------------------------------

This file provides numerous constant values and enumerations, both for the RTMP specification, and for internal use. 

### General Library Definitions ###



### Library Event Types ###

These are part of the `rtmp_event_t` enumeration.

|Name|Purpose|
|:-:|:--|
|`RTMP_EVENT_CONNECT_SUCCESS`|A connection was made and a valid handshake transpired. Communication may commence.|
|`RTMP_EVENT_CONNECT_FAIL`|A connection was not able to be made, or the handshake failed.|
|`RTMP_EVENT_CLOSED`|The connection was closed.|
|`RTMP_EVENT_INTERRUPTED`|The connection was interrupted.|
|`RTMP_EVENT_FAILED`|The RTMP stream failed in some unexpected way.|
|`RTMP_EVENT_FILLED`|The output buffer was filled with at least one byte.|
|`RTMP_EVENT_EMPTIED`|The input buffer had at least one byte removed.|
|`RTMP_EVENT_REFRESH`|Called at regular intervals.|

### Callback Return Statuses ###

These are part of the `rtmp_cb_status_t` enumeration.

|Name|Meaning|
|:-:|:--|
|`RTMP_CB_CONTINUE`|The callback completed successfully. Continue processing the stream.|
|`RTMP_CB_ERROR`|The callback suffered an error. Abort processing this message. This message is not removed from the input buffer.|
|`RTMP_CB_DEFER_PAUSE`|The callback wishes to abort processing this message, and pause the processing on this stream until further notice. This message is not removed from the input buffer.|
|`RTMP_CB_ABORT`|Abort the stream entirely.|

### Library Errors ###

These are part of the `rtmp_err_t` enumeration.

|Name|Severity|Meaning|
|:-:|:--|:--|
|`RTMP_ERR_NONE`|N/A|No error occured.|
|`RTMP_ERR_DIVERGENCE_METER_ERROR`|Low|The value of a window acknowledgement indicates that the recipient has more data than you sent. Are they in the future?|
|`RTMP_ERR_POLL_FAIL`|Low|The underlying network polling mechanism aborted unexpectedly.|
|`RTMP_ERR_NOT_READY`|Low|The stream has not completed the handshake process yet.|
|`RTMP_ERR_PAUSE`|Low|The stream is paused.|
|`RTMP_ERR_AGAIN`|Low|The data could not be completely written into the output buffer.|
|`RTMP_ERR_ERROR`|Medium|A generic error. Usually returned because of callbacks which return `RTMP_CB_ERROR`.|
|`RTMP_ERR_DNS_FAIL`|Medium|A domain name failed to resolve.|
|`RTMP_ERR_BAD_WRITE`|Medium|An attempt to write information (usually AMF data) has failed for some reason.|
|`RTMP_ERR_BAD_READ`|Medium|An attempt to read information (usually AMF data) has failed for some reason.|
|`RTMP_ERR_INVALID`|Medium|Invalid data found in the stream. This is often fatal, though sometimes not.|
|`RTMP_ERR_FATAL`|Fatal|A generic fatal error. Fatal errors indicate some unreconcileable problem with local or global state.|
|`RTMP_ERR_CONNECTION_FAIL`|Fatal|The underlying transport layer has failed in some fashion.|
|`RTMP_ERR_OOM`|Fatal|Insufficient memory to perform the request.|
|`RTMP_ERR_INADEQUATE_CHUNK`|Fatal|The chunk cache is full. This means that more than `RTMP_STREAM_CACHE_MAX` chunk IDs are in use.|
|`RTMP_ERR_ABORT`|Fatal|The stream is aborting for some reason, usually as a result of a callback.|

### Bandwidth Limits ###

These are part of the `` enumeration.

|Name|Purpose|Defined In|
|:-:|:--|:--|
|`RTMP_LIMIT_HARD`|The peer should limit output bandwidth to the accompanying value.|§5.4.5|
|`RTMP_LIMIT_SOFT`|The peer should limit output bandwidth to the accomanying value, unless the existing limit is smaller.|§5.4.5|
|`RTMP_LIMIT_DYNAMIC`|If the previous limit was hard, treat this message as also being hard. Otherwise, ignore this message.|§5.4.5|

### Chunk Stream Message Types ###

These are part of the `rtmp_message_type_t` enumeration.

##### Chunk Control Messages #####

|Name|Purpose|Defined In|
|:-:|:--|:--|
|`RTMP_MSG_SET_CHUNK_SIZE`|A request indicating a new chunk size which the sender will use from now on.|§5.4.1|
|`RTMP_MSG_ABORT`|A request to abort processing of a message prematurely.|§5.4.2|
|`RTMP_MSG_ACK`|A message acknowledging the number of bytes received so far.|§5.4.3|
|`RTMP_MSG_WIN_ACK_SIZE`|A message indicating the window size of the sender.|§5.4.4|
|`RTMP_MSG_SET_PEER_BWIDTH`|A message indicating to the peer that the sender wishes the peer to change their window size.|§5.4.5|

##### User Control Messages #####

|Name|Purpose|Defined In|
|:-:|:--|:--|
|`RTMP_MSG_USER_CTL`|Indicates that this is a user control message.|§6.2|

##### Chunk Control Messages #####

|Name|Purpose|Defined In|
|:-:|:--|:--|
|`RTMP_MSG_AUDIO`|Indicates that this is an audio message.|§7.1.4|
|`RTMP_MSG_VIDEO`|Indicates that this is a video message.|§7.1.5|
|`RTMP_MSG_AGGREGATE`|Indicates that this is an aggregate RTMP message.|§7.1.6|
|`RTMP_MSG_AMF3_DAT`|Indicates that the payload of this message is AMF3 information.|§7.1.2|
|`RTMP_MSG_AMF3_SO`|Indicates that the payload of this message is a serialized shared object in AMF3 format.|§7.1.3|
|`RTMP_MSG_AMF3_CMD`|Indicates that the payload of this message is a RPC call in AMF3 format.|§7.1.1|
|`RTMP_MSG_AMF0_DAT`|Indicates that the payload of this message is AMF0 information.|§7.1.2|
|`RTMP_MSG_AMF0_SO`|Indicates that the payload of this message is a serialized shared object in AMF0 format.|§7.1.3|
|`RTMP_MSG_AMF0_CMD`|Indicates that the payload of this message is a RPC call in AMF0 format.|§7.1.1|


### Media Support Flags ###

These are part of the `rtmp_support_codec_snd_t` enumeration.

|Name|Meaning|Defined In|
|:-:|:--|:--|
|`RTMP_SUPPORT_SND_NONE`|Raw sound|§7.2.1.1|
|`RTMP_SUPPORT_SND_ADPCM`|ADPCM|§7.2.1.1|
|`RTMP_SUPPORT_SND_MP3`|MP3|§7.2.1.1|
|`RTMP_SUPPORT_SND_AAC`|Advanced Audio Coding|§7.2.1.1|
|`RTMP_SUPPORT_SND_NELLY`|NellyMoser at 5, 11, 22, and 44kHz|§7.2.1.1|
|`RTMP_SUPPORT_SND_NELLY8`|NellyMoser at 8kHz|§7.2.1.1|
|`RTMP_SUPPORT_SND_NELLY16`|NellyMoser at 16kHz|§7.2.1.1|
|`RTMP_SUPPORT_SND_G711A`|G711A|§7.2.1.1|
|`RTMP_SUPPORT_SND_G711U`|G711U|§7.2.1.1|
|`RTMP_SUPPORT_SND_SPEEX`|Speex audio|§7.2.1.1|
|`RTMP_SUPPORT_SND_INTEL`|Not used|§7.2.1.1|
|`RTMP_SUPPORT_SND_UNUSED`|Not used|§7.2.1.1|
|`RTMP_SUPPORT_SND_ALL`|All compatible codecs|§7.2.1.1|

These are part of the `rtmp_support_codec_vid_t` enumeration.

|Name|Meaning|Defined In|
|:-:|:--|:--|
|`RTMP_SUPPORT_VID_UNUSED`|Not used|§7.2.1.1|
|`RTMP_SUPPORT_VID_JPEG`|Not used|§7.2.1.1|
|`RTMP_SUPPORT_VID_SORENSON`|Sorenson flash video|§7.2.1.1|
|`RTMP_SUPPORT_VID_HOMEBREW`|V1 screen sharing|§7.2.1.1|
|`RTMP_SUPPORT_VID_VP6`|On2 video|§7.2.1.1|
|`RTMP_SUPPORT_VID_VP6ALPHA`|On2 video with alpha|§7.2.1.1|
|`RTMP_SUPPORT_VID_HOMEBREWV`|V2 screen sharing|§7.2.1.1|
|`RTMP_SUPPORT_VID_H264`|Advanced Video Coding|§7.2.1.1|
|`RTMP_SUPPORT_VID_ALL`|All compatible codecs|§7.2.1.1|

These are part of the `rtmp_support_func_vid_t` enumeration.

|Name|Meaning|Defined In|
|:-:|:--|:--|
|`RTMP_SUPPORT_VID_CLIENT_SEEK`|Indicates that the player can perform frame-accurate seeks.|§7.2.1.1|

### Object Encoding Version ###

These are part of the `rtmp_amf_version_t` enumeration.

|Name|
|:-:|
|`RTMP_AMF0`|
|`RTMP_AMF3`|

### Shared Object Events ###

These are part of the `rtmp_so_evt_t` enumeration.

|Name|Purpose|Defined In|
|:-:|:--|:--|
|`RTMP_SO_EVT_USE`|||
|`RTMP_SO_EVT_RELEASE`|||
|`RTMP_SO_EVT_REQ_CHANGE`|||
|`RTMP_SO_EVT_CHANGE`|||
|`RTMP_SO_EVT_SUCCESS`|||
|`RTMP_SO_EVT_SENDMESSAGE`|||
|`RTMP_SO_EVT_STATUS`|||
|`RTMP_SO_EVT_CLEAR`|||
|`RTMP_SO_EVT_REMOVE`|||
|`RTMP_SO_EVT_REQ_REMOVE`|||
|`RTMP_SO_EVT_USE_SUCCESS`|||

### User Control Message Events ###

These are part of the `rtmp_usr_evt_t` enumeration.

|Name|Purpose|Defined In|
|:-:|:--|:--|
|`RTMP_USR_EVT_STREAM_BEGIN`|||
|`RTMP_USR_EVT_STREAM_EOF`|||
|`RTMP_USR_EVT_STREAM_DRY`|||
|`RTMP_USR_EVT_SET_BUF_LEN`|||
|`RTMP_USR_EVT_STREAM_IS_REC`|||
|`RTMP_USR_EVT_PING_REQ`|||
|`RTMP_USR_EVT_PING_RES`|||

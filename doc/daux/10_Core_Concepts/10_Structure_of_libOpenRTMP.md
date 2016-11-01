The General Structure of libOpenRTMP
========================

LibOpenRTMP is structured in multiple layers, which build on top of each other. A rough hierarchy of layers looks like:

|AMF|Description|
|:-:|--|
|`amf_decode` `amf_encode`|Provides basic encoding and decoding facilities for AMF data|
|`amf_object`|Provides a logical abstraction for storing, reading, and manipulating AMF data|

|RTMP|Description|
|:-:|--|
|`rtmp_chunk_flow`|Provides functions for reading and writing bits to and from binary streams directly.|
|`rtmp_chunk_conn`|Provides facilities for establishing a connection and managing the protocol settings, as well as sending and receiving chunks/messages.|
|`rtmp_chunk_assembler`|Provides an intermediate layer for fully assembling chunks into messages prior to processing.|
|`rtmp_stream`|Provides higher level constructs for event driven callbacks, and RPC command and response flow.|
|`rtmp_client` `rtmp_server` `rtmp_app`|Provides boilerplate for basic live stream server applications.|
|`rtmp`|Moves data between `rtmp_streams` and the network.|

Additionally, there are a few support functions which the library makes use of on many levels

|Util|Description|
|:-:|--|
|`algorithm.h`|Provides some generic void* algorithms for searching through data.|
|`ieee754_double`|Provides the means for reading and writing IEEE 754 double precision floating point numbers.|
|`memutil`|Provides some endianness flipping functions and some string functions.|
|`parseurl`|An almost-RFC-compliant utility for parsing URLs.|
|`ringbuffer`|A basic ringbuffer implementation.|
|`vec`|A grotesquely complicated set of macros for creating and manipulating dynamic arrays.|
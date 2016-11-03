This document assumes that the reader has foundational knowledge of basic internet terms such as `multiplexing`, `protocol`, `port`, `packet`, and `internet address`; as well as the contents of [RFC2119](https://www.ietf.org/rfc/rfc2119.txt).

### RTMP Terms ####

|Term|Definition|
|:--|:--|
|Real Time Messaging Protocol (RTMP) | Adobe's binary protocol intended to coherently transmit audiovisual content over the network in close to real time.|
|Action Message Format (AMF) | Adobe's binary storage format for heterogenous data. AMF is currently available in two versions, AMF0 and AMF3.|
|Message Stream | A logical channel of communication through which messages flow.|
|Message Stream ID | An ordinal identifier for a message stream.|
|Chunk | In RTMP parlance, a chunk is a fragment of a message. A chunk has a fixed maximum size (negotiated upon connecting), wheras messages can be much longer than a chunk. Chunks are assembled into whole messages by the recipient, and timestamps ensure that the chunks are assembled in the correct order, even when interleaved.|
|Chunk Stream | A logical channel of communication through which chunks from.|
|Chunk Stream ID | An ordinal identifier for a chunk stream.|
|Remote Procedure Call (RPC) | A request over a connection which indicates that the sender wishes to invoke some functionality on the recipient machine.|

### Live Streaming Terms ###

|Term|Definition|
|:--|:--|
|Playpath | The name of a stream. When broadcasting, this is the stream key. When consuming, this is the stream name.|

### libOpenRTMP Terms ###

|Term|Definition|
|:--|:--|
|AMF Object | An instance of the `amf_object` abstraction which provides an interface to read and write AMF-like data, along with serialization and deserialization routines.|
|App | A collection of callbacks associated with a URL path sent when connecting to a server. in `rtmp://localhost/foo`, the app is `foo`.|
|App List | A collection of apps associated with different URL paths when connecting to a server.|
|Chunk Conn | An instance of the `rtmp_chunk_conn` abstraction which provides the lowest level set of operations for RTMP connections.|
|Stream | An instance of the `rtmp_stream` abstraction which provides some RPC interface code, as well as some higher level event driven callback functionality.|
|Client | An instance of the `rtmp_client` abstraction which provides several specific wrappers around various RPC calls.|
|Server | An instance of the `rtmp_server` abstraction which provides several specific wrappers around various RPC calls, as well as handling for common streaming functionality.|
|Manager | An instance of the `rtmp` abstraction which marshals data between the network and RTMP Streams contained by Clients or Servers.|

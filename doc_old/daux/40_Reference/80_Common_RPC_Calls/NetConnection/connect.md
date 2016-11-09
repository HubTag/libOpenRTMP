title: connect
--------------------------

A NetConnection RPC sent by a client to request a connection to the server.

## Syntax ##

```
String: "connect"
Number: 1
Object: {
String:     app:            ... // App Name
String:     flashver:       ... // Flash Version
String:     swfUrl:         ... // SWF URL
String:     tcUrl:          ... // Server URL
Bool:       fpad:           ... // Proxied
Number:     audioCodecs:    ... // Supported Audio Codecs
Number:     videoCodecs:    ... // Supported Video Codecs
Number:     videoFunction:  ... // Supported Video Functions
String:     pageUrl:        ... // Page URL
Number:     objectEncoding: ... // AMF Encoding
        }
Object: Optional information

```

## Parameters ##
#### field[0] #### 
> **Type:** String
>
> The name of the command, in this case, "connect".

#### field[1] ####
> **Type:** Number
>
> The sequence number. Should always be set to 1 for this call.

#### field[2] ####
> **Type:** Object
>
> An object containing arguments to pass with this function.
> 
> Most parameters are optional, however a server may deny access if certain parameters are absent.
> 
> #### field[2]["app"] ####
> > **Type:** String
> >
> > The name of the server application to request connection to.
> >
> > Example value: "broadcast"
> > 
> > #### field[2]["flashver"] ####
> > **Type:** String
> >
> > The flash player version. This is only really useful when talking to Flash Media Server.
> >
> > Example value: "FMSc/1.0"
> 
> #### field[2]["swfUrl"] ####
> > **Type:** String
> >
> > This is supposed to be the URL of the SWF file which is originating the connection. 
> >
> > Example value: "https://testsite.com/data/player.swf"
> 
> #### field[2]["tcUrl"] ####
> > **Type:** String
> >
> > The URL of the server that the client is connecting to.
> >
> > Example value: "rtmp://testsite.com/broadcast"
> 
> #### field[2]["fpad"] ####
> > **Type:** Boolean
> >
> > Indicates if a proxy is being used.
> >
> > Example value: false
> 
> #### field[2]["audioCodecs"] ####
> > **Type:** Number
> >
> > Indicates audio codec support for this client. See [`rtmp_support_codec_snd_t`](/Reference/RTMP/Types_and_Constants/rtmp_support_codec_snd_t) for possible values.
> >
> > Example value: `RTMP_SUPPORT_SND_AAC`
> 
> #### field[2]["videoCodecs"] ####
> > **Type:** Number
> >
> > Indicates video codec support for this client. See [`rtmp_support_codec_vid_t`](/Reference/RTMP/Types_and_Constants/rtmp_support_codec_vid_t) for possible values.
> >
> > Example value: `RTMP_SUPPORT_VID_H264`
> 
> #### field[2]["videoFunction"] ####
> > **Type:** Number
> >
> > Indicates video codec support for this client. See [`rtmp_support_func_vid_t`](/Reference/RTMP/Types_and_Constants/rtmp_support_func_vid_t) for possible values.
> >
> > Example value: `RTMP_SUPPORT_VID_CLIENT_SEEK`
> 
> #### field[2]["pageUrl"] ####
> > **Type:** String
> >
> > The URL of the webpage from which the SWF was loaded. 
> >
> > Example value: "http://testsite.com/cool_vid.html"
> 
> #### field[2]["objectEncoding"] ####
> > **Type:** Number
> >
> > A number, either 0 or 3, indicating the version of AMF to use.
> >
> > Example value: 0

#### field[3] ####
> **Type:** Object
>
> An optional object which may include additional parameters.

## Return Value ##
A server may return something along the lines of the following. However, the specification makes no guarantees as to the structure of a response.

```
String: "_result" or "_error"
Number: 1
Object: {
String:     fmsVer:       ... // Flash Media Server Version
Number:     capabilities: ... // Server Capabilities
Number:     mode:         ... // Mode
        }
Object: {
String:     level: "status"
String:     code:        ... // Status Code
String:     description: ... // Description
Object:     data: {
String:         string:  ... // Etc. Data
            }
        }
```

#### response[0] ####
> **Type:** String
>
> A string indicating the type of result, either "_result" to indicate success, or "_error" to indicate an error.

#### response[1] ####
> **Type:** Number
>
> The sequence number of the originating command. In the case of `connect`, it's always 1.

#### response[2] ####
> **Type:** Object
>
> A set of values to describe properties of the connection.
> 
> #### response[2]["fmsVer"] ####
> > **Type:** String
> >
> > Indicates the version of Flash Media Server that is providing this connection.
> >
> > Example value: "FMS/0,0,0,0"
> 
> #### response[2]["capabilities"] ####
> > **Type:** Number
> >
> > Indicates the capabilities of the server. This falls outside of the spec, and as such is not really documented here.
> >
> > Example value: 0x1F
> 
> #### response[2]["mode"] ####
> > **Type:** Number
> >
> > Indicates the mode of the connection. This falls outside of the spec, and as such is not really documented here.
> >
> > Example value: 1

#### response[3] ####
> **Type:** Object
> 
> A set of values to describe the response from the server.
>
> #### response[3]["level"] ####
> > **Type:** String
> >
> > The level of alert that this response is indicating. For `connect`, it's almost always "status".
> >
> > Example value: "status"
>
> #### response[3]["code"] ####
> > **Type:** String
> >
> > A NetConnection status code, either "NetConnection.Connect.Success" or "NetConnection.Connect.Rejected".
> >
> > Example value: "NetConnection.Connect.Success"
>
> #### response[3]["description"] ####
> > **Type:** String
> >
> > A textual description of the result.
> >
> > Example value: "Connection accepted."
>
> #### response[3]["data"] ####
> > **Type:** Object
> >
> > An object containing metadata about the connection.
> > 
> > #### response[3]["data"]["string"] ####
> > > **Type:** String
> > > 
> > > A string containing information about the connection. In some cases, this is the numeric part of FMS Version.
> > > 
> > > Example value: "0,0,0,0"

## Remarks ##
Many of the arguments indicate flash-centric terms, such as having an originating page or URL from which a SWF was loaded. In many modern cases, there is no need to supply such information, as there frequently is no such information to provide. Most servers will not balk if the information is omitted.

It's always worth noting again that not all servers will give the same response, it is simply one possible response of many.

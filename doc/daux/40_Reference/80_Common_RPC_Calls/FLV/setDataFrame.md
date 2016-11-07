title: @setDataFrame / onMetaData
--------------------------------

Sent by the client in an AMF data message to indicate FLV data pertaining to the stream's metadata.

## Syntax ##
```
String: "@setDataFrame"
String: "onMetaData"
Assoc:  {
Number:     width:           ... // Frame Width
Number:     height:          ... // Frame Height
Number:     videodatarate:   ... // Video Bitrate
Number:     framerate:       ... // Video Framerate
Number:     audiodatarate:   ... // Audio Bitrate
Number:     audiosamplerate: ... // Audio Sample Rate
Number:     audiochannels:   ... // Audio Channels
Bool:       stereo:          ... // Audio Is Stereo
String:     videocodecid:    ... // Video Codec
String:     audiocodecid:    ... // Audio Codec
        } 
```

## Parameters ##
#### field[0] ####
> **Type:** String
> 
> Indicates the type of data. In this case, "@setDataFrame".

#### field[1] ####
> **Type:** String
> 
> Indicates the name of the data, in this case we want to indicate that this is FLV metadata with "onMetaData".

#### field[2] ####
> **Type:** Associative, either Object or Array
> 
> Indicates the actual contents of the data frame.
>
> #### field[2]["width"] ####
> > **Type:** Number
> >
> > Indicates the width of the video frame.
> >
> > Example value: 1920
>
> #### field[2]["height"] ####
> > **Type:** Number
> >
> > Indicates the height of the video frame.
> >
> > Example value: 1080
>
> #### field[2]["videodatarate"] ####
> > **Type:** Number
> >
> > Indicates the video bitrate in kbps.
> >
> > Example value: 2500
>
> #### field[2]["framerate"] ####
> > **Type:** Number
> >
> > Indicates the framerate of the video stream.
> >
> > Example value: 30
>
> #### field[2]["audiodatarate"] ####
> > **Type:** Number
> >
> > Indicates the audio bitrate in kbps.
> >
> > Example value:128
>
> #### field[2]["audiosamplerate"] ####
> > **Type:** Number
> >
> > Indicates the audio sample rate.
> >
> > Example value: 44100
>
> #### field[2]["audiochannels"] ####
> > **Type:** Number
> >
> > Indicates the number of audio channels in the stream.
> >
> > Example value: 2
>
> #### field[2]["stereo"] ####
> > **Type:** Boolean
> >
> > Indicates whether the audio in the stream is stereo.
> >
> > Example value: true
>
> #### field[2]["videocodecid"] ####
> > **Type:** String or Number
> >
> > This value indicates the video codec used by the stream. This can be a string or a number.
> > 
> > Supported values are:
> > * "H264" - h.264 encoded video
> > * "h264" - h.264 encoded video
> > * "avc1" - h.264 encoded video
> > * 0 - RGB video frames
> > * 1 - Run-length encoded video
> > * 2 - Sorenson h.263
> > * 3 - Screen 1
> > * 4 - On2 TrueMotion VP6
> > * 5 - VP6 with alpha
> > * 6 - Screen2
> > * 7 - h.264 encoded video
> > * 8 - ITU H.263
> > * 9 - MPEG-4 ASP
> >
> > Example value: "avc1"
>
> #### field[2]["audiocodecid"] ####
> > **Type:** String or Number
> >
> > This value indicates the audio codec used by the stream. This can be a string or a number.
> > 
> > Supported values are:
> > * ".mp3" - MP3 encoded audio
> > * "mp4a" - AAC encoded audio
> > * 0 - Native endian PCM encoded audio
> > * 1 - ADPCM encoded audio
> > * 2 - MP3 encoded audio
> > * 3 - Little endian PCM encoded audio
> > * 4 - 16kHz NellyMoser encoded audio
> > * 5 - 8kHz NellyMoser encoded audio
> > * 6 - Other NellyMoser encoded audio
> > * 7 - G711A / a-law encoded audio
> > * 8 - G711U / u-law encoded audio
> > * 10 - AAC encoded audio
> > * 11 - Speex encoded audio
> >
> > Example value: "mp4a"
>


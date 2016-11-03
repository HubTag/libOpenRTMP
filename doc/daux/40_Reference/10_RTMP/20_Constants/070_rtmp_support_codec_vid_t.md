title: rtmp_support_codec_vid_t
--------------------------

A set of bitflags which are used to indicate to the peer what sort of video codecs are supported.


## Syntax ##

```c
typedef enum {
    RTMP_SUPPORT_VID_UNUSED     = 0x0001,
    RTMP_SUPPORT_VID_JPEG       = 0x0002,
    RTMP_SUPPORT_VID_SORENSON   = 0x0004,
    RTMP_SUPPORT_VID_HOMEBREW   = 0x0008,
    RTMP_SUPPORT_VID_VP6        = 0x0010,
    RTMP_SUPPORT_VID_VP6ALPHA   = 0x0020,
    RTMP_SUPPORT_VID_HOMEBREWV  = 0x0040,
    RTMP_SUPPORT_VID_H264       = 0x0080,
    RTMP_SUPPORT_VID_ALL        = 0x00FF
} rtmp_support_codec_vid_t;
```

## Members ##

#### RTMP_SUPPORT_VID_UNUSED ####
> Not used
> 
> Defined in §7.2.1.1 on page 32

#### RTMP_SUPPORT_VID_JPEG ####
> Not used
> 
> Defined in §7.2.1.1 on page 32

#### RTMP_SUPPORT_VID_SORENSON ####
> Indicates support for Sorenson flash video compression
> 
> Defined in §7.2.1.1 on page 32

#### RTMP_SUPPORT_VID_HOMEBREW ####
> Indicates support for V1 screen sharing
> 
> Defined in §7.2.1.1 on page 32

#### RTMP_SUPPORT_VID_VP6 ####
> Indicates support for On2 video compression
> 
> Defined in §7.2.1.1 on page 32

#### RTMP_SUPPORT_VID_VP6ALPHA ####
> Indicates support for On2 video with alpha compression
> 
> Defined in §7.2.1.1 on page 32

#### RTMP_SUPPORT_VID_HOMEBREWV ####
> Indicates support for V2 screen sharing
> 
> Defined in §7.2.1.1 on page 32

#### RTMP_SUPPORT_VID_H264 ####
> Indicates support for Advanced Video Coding
> 
> Defined in §7.2.1.1 on page 32

#### RTMP_SUPPORT_VID_ALL ####
> Indicates support for all compatible codecs
> 
> Defined in §7.2.1.1 on page 32

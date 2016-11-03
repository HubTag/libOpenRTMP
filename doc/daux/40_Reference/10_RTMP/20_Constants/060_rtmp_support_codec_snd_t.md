title: rtmp_support_codec_snd_t
--------------------------

A set of bitflags which are used to indicate to the peer what sort of audio codecs are supported.


## Syntax ##

```c
typedef enum {
    RTMP_SUPPORT_SND_NONE       = 0x0001,
    RTMP_SUPPORT_SND_ADPCM      = 0x0002,
    RTMP_SUPPORT_SND_MP3        = 0x0004,
    RTMP_SUPPORT_SND_INTEL      = 0x0008,
    RTMP_SUPPORT_SND_UNUSED     = 0x0010,
    RTMP_SUPPORT_SND_NELLY8     = 0x0020,
    RTMP_SUPPORT_SND_NELLY      = 0x0040,
    RTMP_SUPPORT_SND_G711A      = 0x0080,
    RTMP_SUPPORT_SND_G711U      = 0x0100,
    RTMP_SUPPORT_SND_NELLY16    = 0x0200,
    RTMP_SUPPORT_SND_AAC        = 0x0400,
    RTMP_SUPPORT_SND_SPEEX      = 0x0800,
    RTMP_SUPPORT_SND_ALL        = 0x0FFF
} rtmp_support_codec_snd_t;
```

## Members ##

#### RTMP_SUPPORT_SND_NONE ####
> Indicates support for Raw sound
> 
> Defined in §7.2.1.1 on page 31

#### RTMP_SUPPORT_SND_ADPCM ####
> Indicates support for ADPCM audio compression
> 
> Defined in §7.2.1.1 on page 31

#### RTMP_SUPPORT_SND_MP3 ####
> Indicates support for MP3 audio compression
> 
> Defined in §7.2.1.1 on page 31

#### RTMP_SUPPORT_SND_INTEL ####
> Not used
> 
> Defined in §7.2.1.1 on page 31

#### RTMP_SUPPORT_SND_UNUSED ####
> Not used
> 
> Defined in §7.2.1.1 on page 31

#### RTMP_SUPPORT_SND_NELLY8 ####
> Indicates support for NellyMoser audio compression at 8kHz
> 
> Defined in §7.2.1.1 on page 31

#### RTMP_SUPPORT_SND_NELLY ####
> Indicates support for NellyMoser audio compression at 5, 11, 22, and 44kHz
> 
> Defined in §7.2.1.1 on page 31

#### RTMP_SUPPORT_SND_G711A ####
> Indicates support for G711A audio compression
> 
> Defined in §7.2.1.1 on page 31

#### RTMP_SUPPORT_SND_G711U ####
> Indicates support for G711U audio compression
> 
> Defined in §7.2.1.1 on page 31

#### RTMP_SUPPORT_SND_NELLY16 ####
> Indicates support for NellyMoser audio compression at 16kHz
> 
> Defined in §7.2.1.1 on page 31

#### RTMP_SUPPORT_SND_AAC ####
> Indicates support for Advanced Audio Coding
> 
> Defined in §7.2.1.1 on page 31

#### RTMP_SUPPORT_SND_SPEEX ####
> Indicates support for Speex audio compression
> 
> Defined in §7.2.1.1 on page 31

#### RTMP_SUPPORT_SND_ALL ####
> Indicates support for all compatible audio compression codecs
> 
> Defined in §7.2.1.1 on page 31

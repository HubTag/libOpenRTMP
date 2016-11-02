title: Types (rtmp/rtmp_types.h)
------------------------------------------

This header brings in a few standard libraries, networking libraries, and some defines and typedefs, mostly for consistency's sake. This brings in all of `stdbool.h`, `stdint.h`, and `stddef.h`. It also defines some common names from C++, namely:

|Name|Definition|
|:-:|:--|
|`byte`|`uint8_t`|
|`nullptr`|`((void*)0x0)`|
|`rtmp_time_t`|`size_t`|
|`rtmp_sock_t`|`int` unless on Win32, then `SOCKET` |

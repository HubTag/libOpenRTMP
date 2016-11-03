## Syntax ##
```c
#include<openrtmp/rtmp/rtmp_types.h>

typedef uint8_t byte;
typedef size_t  rtmp_time_t;

#include <stdint.h>
#include <stddef.h>

#ifndef __cplusplus
    #include <stdbool.h>
    #define nullptr ((void*)0x0)
#else
    #define restrict
#endif

#ifdef WINDOWS
    #include <windows.h>
    #include <winsock2.h>
    typedef SOCKET rtmp_sock_t;
#else
    #include <sys/socket.h>
    #include <netinet/tcp.h>
    #include <netinet/in.h>
    #include <netdb.h>
    typedef int rtmp_sock_t;
#endif
```

## Types ##
#### bool ####
> Brought in from `stdbool.h` in C, along with `true` and `false`.
> Just a standard boolean value.

#### byte ####
> A quick shorthand to refer to buffer data in terms of unsigned octets.

#### nullptr ####
> Defined because `NULL` is ugly and it makes the C look a bit better.

#### rtmp_time_t ####
> The primary means of storing a timestamp, which is implied to always be milliseconds when passed as an `rtmp_time_t`.

#### rtmp_sock_t ####
> A platform-agnostic way to refer to a socket.

#### restrict ####
> Defined to maintain compatibility with C++

## Remarks ##
As the code indicates, `stdint.h`, `stddef.h`, and `stdbool.h` are all included in this header, so most common definitions are brought in through this header.

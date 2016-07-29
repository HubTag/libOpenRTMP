/*
    parseurl.c

    Copyright (C) 2016 Hubtag LLC.

    ----------------------------------------

    This file is part of libOpenRTMP.

    libOpenRTMP is free software: you can redistribute it and/or modify
    it under the terms of version 3 of the GNU Affero General Public License
    as published by the Free Software Foundation.

    libOpenRTMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with libOpenRTMP. If not, see <http://www.gnu.org/licenses/>.

*/

#include <ctype.h>
#include "memutil.h"
#include "parseurl.h"
#include <string.h>
#include <stdlib.h>

struct parseurl{
    const char *scheme;
    const char *user;
    const char *password;
    const char *host;
    const char *port;
    const char *path;
    const char *query;
    const char *fragment;
    char *allocated;
    char *url;
};

//If the span of chars from ':' to the first '/' is all numeric, assume it's a port and a scheme wasn't provided.
static bool parseurl_verify_scheme( char *url ){
    bool ret = true;
    do{
        if( !isdigit( *url ) ){
            if( *url == '/' || *url == '\0' ){
                return ret;
            }
            return true;
        }
        ++url;
        ret = false;
    } while( true );
    return false;
}

//Schemes start with a letter followed by any alnum char or + . -
static char *parseurl_scheme( char *url ){
    if( !isalpha( *(url++)) ){
        return nullptr;
    }
    while( *url ){
        if( *url == ':'){
            if( !parseurl_verify_scheme( url + 1 ) ){
                return nullptr;
            }
            *url = 0;
            return url  + 1;
        }
        if( !(isalnum(*url) || *url == '+' || *url == '.' || *url == '-' ) ){
            return nullptr;
        }
        ++url;
    }
    return nullptr;
}
static bool parseurl_has_authority( char *url ){
    return url[0] == '/' && url[1] == '/';
}

static char *parseurl_user( char *url ){
    char *end = strpbrk( url, "@/" );
    if( !end || *end != '@'){
        return nullptr;
    }
    end = strpbrk( url, ":@" );
    if( end ){
        *end = 0;
        return end + 1;
    }
    return nullptr;
}
static char *parseurl_password( char *url ){
    char *end = strpbrk( url, "@/" );
    if( !end || *end != '@'){
        return nullptr;
    }
    *end = 0;
    return end + 1;
}
static char *parseurl_host( char *url ){
    char *end = strpbrk( url, ":/" );
    return end;
}
static char *parseurl_port( char *url ){
    char *end = strpbrk( url, "/" );
    if( !end ){
        return nullptr;
    }
    *end = 0;
    return end + 1;
}
static char *parseurl_path( char *url ){
    char *end = strpbrk( url, "?#" );
    if( !end ){
        return nullptr;
    }
    return end;
}
static char *parseurl_query( char *url ){
    char *end = strpbrk( url, "#" );
    if( !end ){
        return nullptr;
    }
    *end = 0;
    return end + 1;
}

static bool parseurl_parse( parseurl_t out, const char *url ){
    if( !url ){
        return false;
    }
    size_t len = strlen( url );
    //allocating an extra byte to make room for a null between authority and path,
    //and an extra null terminator.
    out->allocated = malloc( 3 + len );
    if( !out->allocated ){
        return false;
    }
    char *buf = out->allocated;
    char *end = out->allocated + len;
    strcpy( buf, url );
    buf[len] = buf[len+1] = buf[len+2] = 0;
    char *temp = parseurl_scheme( buf );
    if( temp ){
        out->scheme = buf;
        buf = temp;
    }
    if( !temp || parseurl_has_authority( buf ) ){
        if( parseurl_has_authority( buf ) ){
            //Skip slashes for authority
            buf += 2;
        }
        temp = parseurl_user( buf );
        if( temp ){
            out->user = buf;
            buf = temp;
            temp = parseurl_password( buf );
            if( temp ){
                out->password = buf;
                buf = temp;
            }
        }
        temp = parseurl_host( buf );
        out->host = buf;
        if( temp ){
            char sep = *temp;
            *temp = 0;
            buf = temp + 1;
            if( sep == ':' ){
                temp = parseurl_port( buf );
                if( *buf ){
                    out->port = buf;
                }
                if( temp ){
                    buf = temp;
                }
                else{
                    goto verify;
                }
            }
            //Move the rest of the string down a byte since we need to insert a slash for path.
            //This also gracefully handles the default case if no path is specified.
            memmove( buf + 1, buf, end - buf );
            *buf = '/';
            ++end;
        }
        else{
            goto verify;
        }
    }
    temp = parseurl_path( buf );
    out->path = buf;
    if( temp ){
        char sep = *temp;
        *temp = 0;
        buf = temp + 1;
        if( sep == '?'){
            temp = parseurl_query( buf );
            out->query = buf;
            if( temp ){
                out->fragment = temp;
            }
        }
        else if( sep == '#'){
            out->fragment = buf;
        }
    }
verify:
    //Invalidate cached URL
    free( out->url );
    out->url = nullptr;

    if( !out->scheme || !out->path ){
        return false;
    }
    //Verify port is numeric
    for( const char * p = out->port; p && *p; ++p ){
        if( !isdigit(*p) ){
            return false;
        }
    }
    return true;
}


static void parseurl_to_url( parseurl_t parser ){
    free( parser->url );
    size_t length =
        strlen_check( parser->scheme ) +
        strlen_check( parser->user ) +
        strlen_check( parser->password ) +
        strlen_check( parser->host ) +
        strlen_check( parser->port ) +
        strlen_check( parser->path ) +
        strlen_check( parser->query ) +
        strlen_check( parser->fragment ) +
        9;
    parser->url = malloc( length + 1 );
    char *buf = parser->url;

    if( parser->scheme ){
        buf = strwrite( buf, parser->scheme );
        buf = strwrite( buf, ":" );
    }
    if( parser->host && parser->scheme ){
        buf = strwrite( buf, "//" );
    }
    if( parser->user ){
        buf = strwrite( buf, parser->user );
    }
    if( parser->password ){
        buf = strwrite( buf, ":" );
        buf = strwrite( buf, parser->password );
    }
    if( parser->user || parser->password ){
        buf = strwrite( buf, "@" );
    }
    if( parser->host ){
        buf = strwrite( buf, parser->host );
    }
    if( parser->port ){
        buf = strwrite( buf, ":" );
        buf = strwrite( buf, parser->port );
    }
    if( parser->path ){
        buf = strwrite( buf, parser->path );
    }
    if( parser->query ){
        buf = strwrite( buf, "?" );
        buf = strwrite( buf, parser->query );
    }
    if( parser->fragment ){
        buf = strwrite( buf, "#" );
        buf = strwrite( buf, parser->fragment );
    }
}


parseurl_t parseurl_create( void ){
    parseurl_t ret = calloc( 1, sizeof( struct parseurl ) );
    return ret;
}
void parseurl_destroy( parseurl_t parser ){
    free( parser->allocated );
    free( parser->url );
    free( parser );
}
const char * parseurl_get( parseurl_t parser, parseurl_part_t part, const char *fallback ){
    const char *ret = nullptr;
    switch( part ){
        case PARSEURL_SCHEME:
            ret = parser->scheme;
            break;
        case PARSEURL_USER:
            ret = parser->user;
            break;
        case PARSEURL_PASS:
            ret = parser->password;
            break;
        case PARSEURL_HOST:
            ret = parser->host;
            break;
        case PARSEURL_PORT:
            ret = parser->port;
            break;
        case PARSEURL_PATH:
            ret = parser->path;
            break;
        case PARSEURL_QUERY:
            ret = parser->query;
            break;
        case PARSEURL_FRAGMENT:
            ret = parser->fragment;
            break;
        case PARSEURL_URL:
            if( !parser->url ){
                parseurl_to_url( parser );
            }
            ret = parser->url;
            break;
        default:
            break;
    }
    if( !ret ){
        ret = fallback;
    }
    return ret;
}

bool parseurl_set( parseurl_t parser, parseurl_part_t part, const char *value ){
    //Kind of hacky, but it works.
    switch( part ){
        case PARSEURL_SCHEME:
            parser->scheme = value;
            break;
        case PARSEURL_USER:
            parser->user = value;
            break;
        case PARSEURL_PASS:
            parser->password = value;
            break;
        case PARSEURL_HOST:
            parser->host = value;
            break;
        case PARSEURL_PORT:
            parser->port = value;
            break;
        case PARSEURL_PATH:
            parser->path = value;
            break;
        case PARSEURL_QUERY:
            parser->query = value;
            break;
        case PARSEURL_FRAGMENT:
            parser->fragment = value;
            break;
        case PARSEURL_URL:
            return parseurl_parse( parser, value );
        default:
            return false;
            break;
    }
    const char *url = parseurl_get( parser, PARSEURL_URL, nullptr );
    return parseurl_parse( parser, url );
}

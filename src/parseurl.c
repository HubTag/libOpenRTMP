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

//Null terminates the scheme if available and valid, and returns the rest of the string.
//Returns nullptr otherwise.
static char *parseurl_scheme( char *url ){
    //Schemes start with a letter followed by any alnum char or + . -
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

//Returns true if the leading slashes for an authority block are present.
static bool parseurl_has_authority( char *url ){
    return url[0] == '/' && url[1] == '/';
}

//Null terminates the username component of the URL if available,
//and returns the rest of the string. Otherwise, returns nullptr.
static char *parseurl_user( char *url ){
    char *end = strpbrk( url, "@/?#" );
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

//Null terminates the password and returns the index of the host block
//if the @ is correctly placed. Returns the rest of the string if
//successful, otherwise returns nullptr.
static char *parseurl_password( char *url ){
    char *end = strpbrk( url, "@/?#" );
    if( !end || *end != '@'){
        return nullptr;
    }
    *end = 0;
    return end + 1;
}

//Returns index of the character immediately following the host if available.
//If the hostname is erroneous, nullptr is returned.
//Does not null terminate the host.
static char *parseurl_host( char *url ){
    char *end;
    const char *host_delims = ":/?#";
    if( *url == '['){
        //We have an IPv6 (or later) literal
        //Find the closing brace.
        end = strpbrk( url, "]" );
        if( end ){
            //We have the closing brace,
            if( !end[1] || strchr( host_delims, end[1]) ){
                //If we're at the end of the string, or the host is immediately
                //followed by a valid delimiter, return the index after the host.
                return end + 1;
            }
            //Erroneous delimiter.
            return nullptr;
        }
        //Couldn't find closing bracket.
        return nullptr;
    }

    end = strpbrk( url, host_delims );
    if( !end ){
        //Keep the contract. This is needed because there isn't
        //really another solution to handle both erroneous
        //IPv6 blocks and end-of-string detection.
        return url + strlen( url );
    }
    return end;
}

//Returns index of /, #, or ? if available, otherwise nullptr.
//Does not null terminate the port.
static char *parseurl_port( char *url ){
    char *end = strpbrk( url, "/?#" );
    if( !end ){
        return nullptr;
    }
    return end;
}

//Returns index of # or ? if available, otherwise nullptr.
//Does not null terminate the path.
static char *parseurl_path( char *url ){
    char *end = strpbrk( url, "?#" );
    if( !end ){
        return nullptr;
    }
    return end;
}

//Null terminates the query block and returns index of the # if available,
//otherwise returns nullptr.
static char *parseurl_query( char *url ){
    char *end = strpbrk( url, "#" );
    if( !end ){
        return nullptr;
    }
    *end = 0;
    return end + 1;
}

//Trims/reads whitespace and returns false if processing should be halted due
//to erroneous whitespace.
static bool parseurl_handle_space( char ** buf, char ** end, int enforce ){
    if( *end == *buf ){
        return true;
    }
    if( enforce != PARSEURL_URL_STRICT ){
        //Trim leading and training whitespace
        while( isspace(**buf) ){
            ++(*buf);
        }
        --(*end);
        while( isspace(**end) ){
            **end = 0;
            --(*end);
        }
        ++(*end);
    }
    if( enforce != PARSEURL_URL_FORGIVING ){
        char *iter = (*buf);
        while( *iter ){
            //Questionable behavior... Convert inline spaces to +
            /*if( *iter == ' ' ){
                *iter = '+';
            }*/
            if( isspace( *iter ) ){
                return false;
            }
            ++iter;
        }
    }
    return true;
}

//Parses the authority block and returns nullptr if it is erroneous.
//Otherwise, return the rest of the string.
static char* parseurl_parse_authority( parseurl_t out, char *buf, int enforce, char **end ){//The scheme wasn't found or we have a // indicating that there is an authority section
    char *temp;
    static char nullchar = '\0';
    if( parseurl_has_authority( buf ) ){
        //Skip slashes for authority
        buf += 2;
    }
    else if(enforce == PARSEURL_URL_STRICT || enforce == PARSEURL_URL){
        //Don't forgive the lack of an authority indicator,
        //as it is erroneous to not have one.
        return nullptr;
    }
    temp = parseurl_user( buf );
    if( temp ){
        //We've definitely found a user name
        out->user = buf;
        buf = temp;
        temp = parseurl_password( buf );
        if( temp ){
            //We've found a password
            out->password = buf;
            buf = temp;
        }
    }
    //parseurl_host doesn't null terminate the string and returns the a pointer to the separator for processing.
    temp = parseurl_host( buf );
    out->host = buf;
    if( temp ){
        //parseurl_host will always return a pointer unless the host is malformed
        char sep = *temp;
        *temp = 0;
        buf = temp + 1;
        //If the separator is a null pointer, return an empty string.
        if( sep == 0 ){
            return &nullchar;
        }
        if( sep == ':' ){
            //We have a port
            temp = parseurl_port( buf );
            if( temp ){
                sep = *temp;
                *temp = 0;
            }
            if( *buf ){
                //If the port string has content.
                out->port = buf;
            }
            if( temp ){
                buf = temp + 1;
            }
            else{
                //If we're at the end of the string, return an empty string.
                return &nullchar;
            }
        }
        //Move the rest of the string down a byte since we need to insert a null terminator
        //without corrupting the subsequent parts of the URL.
        memmove( buf + 1, buf, (*end) - buf );
        *buf = sep;
        //Since the end moved, increment the end pointer.
        ++(*end);
        return buf;
    }
    else{
        //Return nullptr on failure.
        return nullptr;
    }
}

//Verify the integrity of the parser URL. If clear is true, just dump everything.
//Returns true only if the URL is acceptable.
static bool parseurl_verify( parseurl_t out, bool clear, int enforce ){
    if( !clear && out->user && out->user[0] == 0 ){
        //If user is blank, clear it.
        out->user = nullptr;
        if( out->password ){
            out->password = nullptr;
        }

        if( enforce == PARSEURL_URL_STRICT ){
            //An empty username is technically erroneous
            clear = true;
        }
    }

    //Invalidate cached URL. This is deferred until here
    //to allow for using the cached URL as the input for re-generating the URL.
    free( out->url );
    out->url = nullptr;

    if( clear ){
        free( out->allocated );
        memset( out, 0, sizeof( struct parseurl ) );
        return false;
    }


    if( !out->scheme || !out->path ){
        //Scheme and path are required parts of the URL.
        //We don't clear it just in case the user wants to assign
        //their own default scheme.
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

//Parse the URL text. Returns true if it's acceptable.
static bool parseurl_parse_text( parseurl_t out, char *buf, int enforce, char *end ){
    //With everything already set up, parse buf into out
    char *temp = parseurl_scheme( buf );

    if( temp ){
        //Scheme found
        out->scheme = buf;
        buf = temp;
    }
    else if( enforce == PARSEURL_URL_STRICT || enforce == PARSEURL_URL ){
        //Since we aren't forgiving, don't assume the scheme has been or will be filled in
        return parseurl_verify( out, true, enforce );
    }
    if( !temp || parseurl_has_authority( buf ) ){
        //If we haven't found the scheme or we have an authority string, parse the authority block.
        temp = parseurl_parse_authority( out, buf, enforce, &end );
        if( temp ){
            buf = temp;
            if( *buf == '\0' ){
                //An empty string means we should be done processing.
                return parseurl_verify( out, false, enforce );
            }
        }
        else{
            //nullptr always means fatal error.
            return parseurl_verify( out, true, enforce );
        }
    }
    //parseurl_path doesn't null terminate buf, and it returns the position of the separator
    temp = parseurl_path( buf );
    out->path = buf;
    if( temp ){
        //If the path doesn't end in a null, save the separator and set it to null
        char sep = *temp;
        *temp = 0;
        buf = temp + 1;
        if( sep == '?'){
            //We have a query block
            temp = parseurl_query( buf );
            out->query = buf;
            if( temp ){
                //If parseurl_query doesn't return null, then we must have a fragment.
                out->fragment = temp;
            }
        }
        else if( sep == '#'){
            out->fragment = buf;
        }
    }

    return parseurl_verify( out, false, enforce );
}

//Prepare and parse a given URL text.
static bool parseurl_parse( parseurl_t out, const char *url, int enforce ){
    //Prepare to parse
    if( !url ){
        return false;
    }
    free( out->allocated );
    size_t len = strlen( url );
    //allocating an extra byte to make room for a null between authority and path,
    //and an extra null terminator.
    out->allocated = (char*) malloc( 3 + len );
    if( !out->allocated ){
        return false;
    }
    char *buf = out->allocated;
    char *end = out->allocated + len;
    strcpy( buf, url );
    //Triple null terminated because an empty path would cause us to read one-past the last null.
    buf[len] = buf[len+1] = buf[len+2] = 0;
    if( !parseurl_handle_space(&buf, &end, enforce) ){
        return parseurl_verify( out, true, enforce );
    }

    return parseurl_parse_text(out, buf, enforce, end);
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
        9;  //Magic number; it's the length of all the constant parts of the URL.
    if( length == 9 ){
        parser->url = nullptr;
        return;
    }
    parser->url = (char*) malloc( length + 1 );
    char *buf = parser->url;

    if( parser->scheme ){
        buf = strwrite( buf, parser->scheme );
        buf = strwrite( buf, ":" );
    }
    if( parser->host && parser->scheme ){
        //If there's no scheme, it is common to omit the authority indicator.
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
    if( !parser->path || (parser->path && parser->host && parser->path[0] != '/') ){
        //If there's an authority block, the path must start with /
        buf = strwrite( buf, "/" );
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
    parseurl_t ret = ezalloc( ret );
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
                //We don't have it cached, so generate it.
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
    free( parser->url );
    parser->url = nullptr;
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
        case PARSEURL_URL_FORGIVING:
        case PARSEURL_URL_STRICT:
            return parseurl_parse( parser, value, part );
        default:
            return false;
            break;
    }
    //Due to the fact that the components are all stored within a single allocation, it is
    //much simpler, albiet slower, to just generate a new URL with the provided changes
    //and parse that.
    //Perhaps replace this with some memmovs and pointer shuffling?
    const char *url = parseurl_get( parser, PARSEURL_URL, nullptr );
    return parseurl_parse( parser, url, PARSEURL_URL );
}

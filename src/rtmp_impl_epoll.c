/*
    rtmp_impl_epoll.c

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

#include <errno.h>
#include "rtmp_config.h"
#include "rtmp.h"
#include "rtmp/rtmp_private.h"
#include "rtmp/rtmp_types.h"
#include "memutil.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef RTMP_POLLTECH_EPOLL


rtmp_t rtmp_create( void ){
    rtmp_t mgr = calloc( 1, sizeof( struct rtmp_mgr ) );
    mgr->type = RTMP_T_RTMP_T;
    mgr->epoll_args.epollfd = epoll_create(1);
    VEC_INIT(mgr->servers);
    mgr->last_refresh = rtmp_get_time();
    return mgr;
}

static void destroy_server(rtmp_mgr_svr_t stream){
    if( stream->type == RTMP_T_SERVER_T ){
        rtmp_server_destroy( stream->server );
    }
    else if( stream->type == RTMP_T_CLIENT_T ){
        rtmp_client_destroy( stream->client );
    }
    free( stream );
}

void rtmp_destroy( rtmp_t mgr ){
    VEC_DESTROY_DTOR( mgr->servers, destroy_server );
    close( mgr->epoll_args.epollfd );
    free( mgr );
}

static rtmp_cb_status_t stream_event(
    rtmp_stream_t conn,
    rtmp_event_t event,
    void * restrict user
){
    rtmp_mgr_svr_t self = (rtmp_mgr_svr_t) user;
    struct epoll_event e;
    e.data.ptr = user;
    e.events = self->flags;
    if( event == RTMP_EVENT_FILLED && (e.events & EPOLLOUT) == 0 ){
        e.events |= EPOLLOUT;
        epoll_ctl( self->mgr->epoll_args.epollfd, EPOLL_CTL_MOD, self->socket, &e );
    }
    if( event == RTMP_EVENT_EMPTIED && (e.events & EPOLLIN) == 0 ){
        e.events |= EPOLLIN;
        epoll_ctl( self->mgr->epoll_args.epollfd, EPOLL_CTL_MOD, self->socket, &e );
    }
    self->flags = e.events;
    return RTMP_CB_CONTINUE;
}

static rtmp_err_t create_stream( rtmp_t mgr, void * client_or_server, rtmp_sock_t sock, rtmp_t_t type ){
    struct epoll_event event;
    rtmp_mgr_svr_t *item_loc = VEC_PUSH(mgr->servers);

    if( !item_loc ){
        return RTMP_ERR_OOM;
    }
    rtmp_mgr_svr_t item = calloc( 1, sizeof( struct rtmp_mgr_svr ) );
    *item_loc = item;

    item->type = type;
    item->socket = sock;
    rtmp_stream_t stream = nullptr;
    if( type == RTMP_T_SERVER_T){
        ALIAS( client_or_server, rtmp_server_t *, server );
        item->server = rtmp_server_create();
        *server = item->server;
        stream = rtmp_server_stream(*server );
    }
    else if( type == RTMP_T_CLIENT_T ){
        ALIAS( client_or_server, rtmp_client_t *, client );
        item->client = *client;
        stream = rtmp_client_stream( *client );
    }
    item->flags = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLPRI | EPOLLHUP;
    item->mgr = mgr;
    item->closing = false;

    rtmp_err_t err = rtmp_stream_reg_event( stream, RTMP_EVENT_FILLED, stream_event, item );
    err = err ? err : rtmp_stream_reg_event( stream, RTMP_EVENT_EMPTIED, stream_event, item );
    if( err ){
        return err;
    }
    event.data.ptr = item;
    event.events = item->flags;

    epoll_ctl( mgr->epoll_args.epollfd, EPOLL_CTL_ADD, sock, &event );
    return err;
}

static rtmp_err_t handle_server( rtmp_t mgr, int flags ){
    if( (flags & EPOLLERR) || (flags & EPOLLHUP) ){
        shutdown( mgr->listen_socket, SHUT_RDWR );
        close( mgr->listen_socket );
        epoll_ctl( mgr->epoll_args.epollfd, EPOLL_CTL_DEL, mgr->listen_socket, nullptr );
        return RTMP_ERR_CONNECTION_FAIL;
    }
    if( (flags & EPOLLIN) ){
        rtmp_sock_t sock = accept( mgr->listen_socket, nullptr, 0 );
        if( sock <= 0 ){
            perror( "accept" );
            return RTMP_ERR_POLL_FAIL;
        }
        rtmp_server_t server;
        rtmp_err_t ret = create_stream( mgr, &server, sock, RTMP_T_SERVER_T );
        if( ret >= RTMP_ERR_ERROR ){
            return ret;
        }

        rtmp_server_set_app_list( server, mgr->applist );
        if( mgr->callback ){
            return mgr->callback( server, mgr->callback_data ) == RTMP_CB_CONTINUE ? RTMP_ERR_NONE : RTMP_ERR_ABORT ;
        }
    }
    return RTMP_ERR_NONE;
}


static rtmp_err_t handle_stream( rtmp_t mgr, rtmp_mgr_svr_t stream, int flags ){
    struct epoll_event e;
    e.data.ptr = stream;
    e.events = stream->flags;
    rtmp_stream_t s;
    if( (flags & EPOLLERR) || (flags & EPOLLHUP) ){
        goto confail;

    }
    s = stream->type == RTMP_T_SERVER_T ?
        rtmp_server_stream( stream->server ) :
        rtmp_client_stream( stream->client ) ;
    if( flags & EPOLLOUT ){
        const void * buffer;
        size_t size;
        rtmp_chunk_conn_t conn = s ? rtmp_stream_get_conn( s ) : nullptr;
        if( conn && rtmp_chunk_conn_get_out_buff( conn, &buffer, &size ) == RTMP_ERR_NONE ){
            if( size == 0 ){
                if( stream->closing ){
                    goto confail;
                }
                e.events &= ~EPOLLOUT;
                stream->flags &= ~EPOLLOUT;
                epoll_ctl( mgr->epoll_args.epollfd, EPOLL_CTL_MOD, stream->socket, &e );
            }
            else{
                size = send( stream->socket, buffer, size, MSG_NOSIGNAL );
                if( size == (size_t)-1 || size == 0 ){
                    goto confail;
                }
                rtmp_chunk_conn_commit_out_buff( conn, size );
                if( rtmp_chunk_conn_service( conn ) >= RTMP_ERR_FATAL ){
                    goto confail;
                }
            }
        }
    }
    if( flags & EPOLLIN ){
        void * buffer;
        size_t size;
        rtmp_chunk_conn_t conn = s ? rtmp_stream_get_conn( s ) : nullptr;
        if( conn && rtmp_chunk_conn_get_in_buff( conn, &buffer, &size ) == RTMP_ERR_NONE ){
            if( size == 0 ){
                e.events &= ~EPOLLIN;
                stream->flags &= ~EPOLLIN;
                epoll_ctl( mgr->epoll_args.epollfd, EPOLL_CTL_MOD, stream->socket, &e );
                if( rtmp_chunk_conn_service( conn ) >= RTMP_ERR_FATAL ){
                    stream->closing = true;
                    stream->flags = EPOLLOUT;
                    e.events = EPOLLOUT;
                    epoll_ctl( mgr->epoll_args.epollfd, EPOLL_CTL_MOD, stream->socket, &e );
                }
            }
            else{
                size = recv( stream->socket, buffer, size, MSG_NOSIGNAL );
                if( size == (size_t)-1 || size == 0 ){
                    goto confail;
                }
                rtmp_chunk_conn_commit_in_buff( conn, size );
                if( rtmp_chunk_conn_service( conn ) >= RTMP_ERR_FATAL ){
                    stream->closing = true;
                    stream->flags = EPOLLOUT;
                    e.events = EPOLLOUT;
                    epoll_ctl( mgr->epoll_args.epollfd, EPOLL_CTL_MOD, stream->socket, &e );
                }
            }
        }
    }
    return RTMP_ERR_NONE;
    confail:
        printf("DEAD\n");
    epoll_ctl( mgr->epoll_args.epollfd, EPOLL_CTL_DEL, stream->socket, &e );
    shutdown( stream->socket, SHUT_RDWR );
    close( stream->socket );
    for( size_t i = 0; i < VEC_SIZE(mgr->servers); ++i ){
        if( mgr->servers[i] == stream ){
            VEC_ERASE( mgr->servers, i );
            break;
        }
    }
    if( stream->type == RTMP_T_SERVER_T ){
        rtmp_server_destroy( stream->server );
    }
    else if( stream->type == RTMP_T_CLIENT_T ){
        rtmp_client_destroy( stream->client );
    }
    free( stream );
    return RTMP_ERR_CONNECTION_FAIL;
}


rtmp_err_t rtmp_service( rtmp_t mgr, int timeout ){
    struct epoll_event events[RTMP_EPOLL_MAX];
    rtmp_err_t err = RTMP_ERR_NONE;
    int fd_count = epoll_wait( mgr->epoll_args.epollfd, events, RTMP_EPOLL_MAX, timeout );
    if( rtmp_get_time() > mgr->last_refresh + RTMP_REFRESH_TIME ){
        size_t s = VEC_SIZE(mgr->servers);
        for( size_t i = 0; i < s; ++i ){
            rtmp_chunk_conn_service( rtmp_stream_get_conn( rtmp_server_stream( mgr->servers[i]->server ) ) );
        }
        mgr->last_refresh = rtmp_get_time();
    }
    if( fd_count < 0 ){
        return RTMP_ERR_POLL_FAIL;
    }
    for( size_t i = 0; i < (size_t)fd_count; ++i ){
        rtmp_t_t * type = events[i].data.ptr;
        switch( *type ){
        case RTMP_T_RTMP_T:
            err = handle_server( events[i].data.ptr, events[i].events );
            break;
        case RTMP_T_CLIENT_T:
        case RTMP_T_SERVER_T:
            err = handle_stream( mgr, events[i].data.ptr, events[i].events );
            break;
        default:
            err = RTMP_ERR_POLL_FAIL;
            break;
        }
        if( err != RTMP_ERR_NONE ){
            return err;
        }
    }
    return err;
}

rtmp_err_t rtmp_connect( rtmp_t mgr, rtmp_client_t client ){
    rtmp_sock_t sock;
    union{
        struct sockaddr_in sin;
        struct sockaddr s;
    } svr;
    struct hostent *host;
    const char * hostname;
    uint16_t port;
    if( rtmp_client_get_conninfo( client, &hostname, &port ) != RTMP_ERR_NONE){
        rtmp_client_destroy( client );
        return RTMP_ERR_CONNECTION_FAIL;
    }

    host = gethostbyname( hostname );
    if( !host || !host->h_addr_list || !host->h_addr_list[0] ){
        rtmp_client_destroy( client );
        return RTMP_ERR_DNS_FAIL;
    }

    sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( sock <= 0 ){
        rtmp_client_destroy( client );
        return RTMP_ERR_CONNECTION_FAIL;
    }
    //Allow socket reuse
    static int one = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof( one ) );

    memset( &svr, 0, sizeof( svr ) );
    svr.sin.sin_addr.s_addr = *((in_addr_t**)host->h_addr_list)[0];
    svr.sin.sin_family = AF_INET;
    svr.sin.sin_port = htons(port);
    if( connect( sock, &svr.s, sizeof( svr ) ) < 0 ){
        rtmp_client_destroy( client );
        close( sock );
        return RTMP_ERR_CONNECTION_FAIL;
    }

    return create_stream( mgr, &client, sock, RTMP_T_CLIENT_T );

    //mgr->callback = cb;
    //mgr->callback_data = user;
    //mgr->listen_socket = sock;

    return RTMP_ERR_NONE;
}

rtmp_err_t rtmp_listen( rtmp_t mgr, const char * iface, short port, rtmp_connect_proc cb, void *user ){
    rtmp_sock_t sock;
    union{
        struct sockaddr_in sin;
        struct sockaddr s;
    } svr;
    struct hostent *host;

    host = gethostbyname( iface );
    if( !host || !host->h_addr_list || !host->h_addr_list[0] ){
        return RTMP_ERR_DNS_FAIL;
    }

    sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( sock <= 0 ){
        return RTMP_ERR_CONNECTION_FAIL;
    }
    //Allow socket reuse
    static int one = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof( one ) );

    memset( &svr, 0, sizeof( svr ) );
    svr.sin.sin_addr.s_addr = *((in_addr_t**)host->h_addr_list)[0];
    svr.sin.sin_family = AF_INET;
    svr.sin.sin_port = htons(port);
    if( bind( sock, &svr.s, sizeof( svr ) ) < 0 ){
        close( sock );
        return RTMP_ERR_CONNECTION_FAIL;
    }

    if( listen( sock, RTMP_LISTEN_SIZE ) < 0 ){
        close( sock );
        return RTMP_ERR_CONNECTION_FAIL;
    }

    struct epoll_event evt;
    evt.events = EPOLLERR | EPOLLIN;
    evt.data.ptr = mgr;

    if( epoll_ctl( mgr->epoll_args.epollfd, EPOLL_CTL_ADD, sock, &evt ) < 0 ){
        close( sock );
        return RTMP_ERR_POLL_FAIL;
    }

    mgr->callback = cb;
    mgr->callback_data = user;
    mgr->listen_socket = sock;

    return RTMP_ERR_NONE;
}

#endif

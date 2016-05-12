#pragma once

#include "includes/libopenrtmp.hpp"
#include "includes/OpenRTMPClient.hpp"
#include "includes/OpenRTMPServer.hpp"

// server functions
void *rtmp_make_server(){
    return (void *) new OpenRTMPServer();
}

void rtmp_reference_server(void* server){

}

void rtmp_destroy_server(void* server){

}

// client functions
void *rtmp_make_client(){
    return (void *) new OpenRTMPClient();
}

void rtmp_reference_client(void* client){

}

void rtmp_destroy_client(void* client){

}

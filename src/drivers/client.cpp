#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <sstream>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <iostream>
#include "client.h"

 Client::Client(string address, int port):
    _address(address),_port(port){    
 }

  Client::~Client(){

 }

bool Client::open() {
    int iResult;
    WSADATA wsaData;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;

    // Initialize the variables
    _socket = INVALID_SOCKET;
    _connected = false;

    // Initialize Winsock driver
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
        return false;

    // Configure current Socket Connection and Protocol
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Convert int>>string>>char*
    std::stringstream port_str;
    port_str << _port;

    // Resolve the server address and port
    iResult = getaddrinfo(_address.c_str(), port_str.str().c_str(), &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        _socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (_socket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( _socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(_socket);
            _socket = INVALID_SOCKET;
            continue;
        }
        break;
    }
    // Free memory
    freeaddrinfo(result);

    // Get if the connection was succesfull
    if (_socket == INVALID_SOCKET) 
        return false;

    // Check if the thread already exist
    if (_thread == NULL) {
        // Create the new thread
        _thread = make_shared<thread>(&Client::_connection_loop,this);
        // Set running as true
        _connected = true;
        // return true
    }

    // Set connected to true and retur the result
    _connected = true;   
    return true;
}

 bool Client::is_connected() {
     return _connected;
 }

 bool Client::send_message(const char* buffer, const int count) {
    // Send an initial buffer
    if (send(_socket, buffer, count, 0) == SOCKET_ERROR)
        return false;
    else
        //shutdown(_socket, SD_SEND);
        return true;
  }
   
 void Client::_connection_loop() {
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    int iResult;
    // Receive until the peer closes the connection
    do {
        iResult = recv(_socket, recvbuf, recvbuflen, 0);
        if ( iResult > 0 )
            printf("Bytes received: %d\n", iResult);
        else if ( iResult == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());
    } while( iResult > 0 && _connected);
}

bool Client::close(){
     if (_connected) {
        // Set running as False to Stop the thread
        _connected = false;
        // Pause until the thread ends
        _thread->join();
    }
    // shutdown the connection since no more data will be sent
    shutdown(_socket, SD_SEND);
    // cleanup
    closesocket(_socket);
    WSACleanup();
    return true;
}

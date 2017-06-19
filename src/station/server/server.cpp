#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <sstream>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <iostream>
#include <chrono> 
#include "server.h"

Server::Server(int port):
    _port(port){    
}

Server::~Server(){

 }

 bool Server::is_running() {
     return _running;
 }

bool Server::start(){
    // Check if the thread already exist
    if (_thread == NULL) {
        // Set running as true
        _running = true;
        // Create the new thread
        _thread = make_shared<thread>(&Server::_server_loop,this);
        // return true
        return true;
    }
    // Proccess still running
    return false;
}

bool Server::stop(){
    if (_running) {
        // Set running as False to Stop the thread
        _running = false;
        // Pause until the thread ends
        _thread->join();
        // Close socket and cleacn the server
        closesocket(_socket);
        WSACleanup();
    }
    return true;
}

void Server::_server_loop() 
{
    WSADATA wsaData;
    int iResult;
    _socket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;
    
    // Initialize Winsock
    if ( WSAStartup(MAKEWORD(2,2), &wsaData)!= 0)
        return;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Convert int>>string>>char*
    std::stringstream port_str;
    port_str << _port;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, port_str.str().c_str(), &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return;
    }

    // Create a SOCKET for connecting to server
    _socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (_socket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    // Setup the TCP listening socket
    iResult = bind(_socket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(_socket);
        WSACleanup();
        return;
    }

    freeaddrinfo(result);

    while (_running){

        iResult = listen(_socket, SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
            printf("listen failed with error: %d\n", WSAGetLastError());
            closesocket(_socket);
            WSACleanup();
            return;
        }

        // Accept a client socket
        SOCKET client_socket = accept(_socket, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(_socket);
            WSACleanup();
            return;
        }

        // Create new thread with the new socket connection
        //thread client_thread (&Server::_client_loop,this,client);
        _client_thread = make_shared<thread>(&Server::_client_loop,this,client_socket);
    }
   
}

void Server::_client_loop(SOCKET socket){
    int iResult;
    int iSendResult;
    char recvbuf[Server::DEFAULT_BUFLEN];
    int recvbuflen = Server::DEFAULT_BUFLEN;
    // Receive until the peer shuts down the connection
    cout << "Client connected. Syncronizing.." << endl;
    do {
        cout << "Wating for Messages...." << endl;
        iResult = recv(socket, recvbuf, recvbuflen, 0);
        cout << "Received from Client.." << endl;
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);

            // Echo the buffer back to the sender
            iSendResult = send( socket, recvbuf, iResult, 0 );
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(socket);
                WSACleanup();
                return;
            }
            printf("Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else  {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(socket);
            WSACleanup();
            return;
        }
        // Sleep 1 second
        std::this_thread::sleep_for (std::chrono::seconds(1));
    } while (iResult > 0);

    // shutdown the connection since we're done
    iResult = shutdown(socket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(socket);
        WSACleanup();
        return;
    }

    // cleanup
    closesocket(socket);
    WSACleanup();

    return;
}

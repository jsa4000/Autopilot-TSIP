
#ifndef _SERVER_SOCKET_H_
#define _SERVER_SOCKET_H_

#include <string>
#include <thread>
#include <winsock2.h>
using namespace std;

class Server {

    public:

    static const int DEFAULT_BUFLEN = 512;
    static const int DEFAULT_PORT = 27015;

    Server(int port=Server::DEFAULT_PORT);
    ~Server();

    bool start();
    bool stop();
    bool is_running();

    private:

    int _port;

    void _client_loop(SOCKET socket);
    void _server_loop();
    bool _running = false;
    SOCKET _listen_socket;
    shared_ptr<thread> _thread = NULL;

};

#endif // _SERVER_SOCKET_H_
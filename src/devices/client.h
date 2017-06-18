#ifndef _CLIENT_SOCKET_H_
#define _CLIENT_SOCKET_H_

#include <string>
#include <thread>
#include <winsock2.h>
using namespace std;

class Client {

    public:

    static const int DEFAULT_BUFLEN = 512;
    static const int DEFAULT_PORT = 27015;

    Client(string address, int port=Client::DEFAULT_PORT);
    ~Client();

    bool open();
    bool close();
    bool is_connected();
    bool send_message(const char* buffer, const int count);

    private:

    string _address;
    int _port;

    void _connection_loop();
    bool _connected = false;
    SOCKET _socket;
    shared_ptr<thread> _thread = NULL;

};

#endif // _CLIENT_SOCKET_H_
#include <iostream>
#include "tsip.h"
#include "socket.h"
using namespace std;

int main(int argc, const char * argv[]) {

    cout << "Hello World" << endl;

    // Use the function loaded from the tsip header.
    // tsip::test_thread();
    // tsip::test_boost();
    // Test the Socket packages Winsock2

    const char* server_address = "localhost";
    tsip_socket::test_socket(server_address);

    return 0;
}



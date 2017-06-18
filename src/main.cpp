#include <iostream>
#include "rtos/tsip.h"
#include "dma/socket.h"
using namespace std;

int main(int argc, const char * argv[]) {

    cout << "Hello World" << endl;

   

    // Optional 1

    // The idea is to enable the interruption from RTOS
    // What Would it change?
    //  Premption == Enable ?
    // If disabled the the DMA not make so much sense. However it could be
    // handy if there are other priority task or ground station message
    // to process. Even the manual action so, this will totally disable the
    // autopilot.

    // Optional 2
    // Test-driven development (TDD), also called test-driven design, 
    // is a method of implementing software programming that interlaces
    // unit testing, programming and refactoring on source code.
    // Implements this pattern to ensure the first points of the test.


    // Hz to Seconds. 1/Seconds = Herzios; Seconds = 1/Hz

    // Use the function loaded from the tsip header.
    tsip::test_thread();
    tsip::test_boost();
    // Test the Socket packages Winsock2

    const char* server_address = "localhost";
    tsip_socket::test_socket(server_address);

    return 0;
}



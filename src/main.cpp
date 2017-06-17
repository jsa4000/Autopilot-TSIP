#include <iostream>
#include "tsip.h"
#include "socket.h"
using namespace std;

int main(int argc, const char * argv[]) {

    cout << "Hello World" << endl;

    /* DMA

    Direct memory access (DMA) is a means of having a peripheral  
    device control a processor's memory bus directly. DMA permits 
    the peripheral, such as a UART, to transfer data directly to
    or from memory without having each byte (or word) handled by 
    the processor. Thus DMA enables more efficient use of interrupts,
    increases data throughput, and potentially reduces hardware 
    costs by eliminating the need for peripheral-specific FIFO buffers. 

    */

    /* Preemption

    In computing, preemption is the act of temporarily interrupting a 
    task being carried out by a computer system, without requiring its 
    cooperation, and with the intention of resuming the task at a later
    time. Such changes of the executed task are known as context switches.
    It is normally carried out by a privileged task or part of the system 
    known as a preemptive scheduler, which has the power to preempt, or 
    interrupt, and later resume, other tasks in the system.
    
    */

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
    // tsip::test_thread();
    // tsip::test_boost();
    // Test the Socket packages Winsock2

    const char* server_address = "localhost";
    tsip_socket::test_socket(server_address);

    return 0;
}



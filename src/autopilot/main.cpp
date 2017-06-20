#include "rtos/rtos.h"

int main(int argc, const char * argv[]) {
    using namespace std;

    // START THE PROGRAM
    cout << "PROGRAM STARTED" << endl;

    //START THE REAL-TIME OPERATIVE SYSTEM (RTOS)
    RTOS os = RTOS(1000);
    os.start();

    char c;
    // WAIT UNTIL THE END OR QUIT
     while (os.is_running() && (c != 'q')){
        // Read the user key
        c = getchar();
        if (c == 'q') {
            cout << "RTOS Shoutdown" << endl;
        }
     }

    // Stop RTOS
    os.stop();

    // END THE PROGRAM
    cout << "PROGRAM ENDED" << endl;
}



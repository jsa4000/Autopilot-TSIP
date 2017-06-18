#include <iostream>
#include "server.h"

int main(int argc, const char * argv[]) {
    using namespace std;

    // START THE PROGRAM
    cout << "STATION SIMULATOR STARTED" << endl;

    //START THE STATIION SIMULATIONSYSTEM (RTOS)
    Server station = Server(1000);
    station.start();

    // // WAIT UNTIL THE END OR QUIT
     while (station.is_running()){
        
         //os.stop()
     }
  
    // END THE PROGRAM
    cout << "PROGRAM ENDED" << endl;
}


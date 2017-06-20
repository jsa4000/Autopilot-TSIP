#include "rtos.h"

RTOS::RTOS(int timer)
    :Task("RTOS", HIGH_PRIORITY,timer) {

}

RTOS::~RTOS(){
}

bool RTOS::init(){
    return true;
}

void RTOS::shutdown(){

    // finally stop the main thread
    stop();
}

void RTOS::_default_callback(){

    std::cout << "countdown:\n";
    for (int i=10; i>0; --i) {
        std::cout << i << std::endl;
        std::this_thread::sleep_for (std::chrono::seconds(1));
    }
    std::cout << "Lift off!\n";

    // //START THE STATIION SIMULATIONSYSTEM (RTOS)
    // Client client = Client("localhost");
    // client.open();

    // // // WAIT UNTIL THE END OR QUIT
    //  while (client.is_connected()){
        
    //      //os.stop()
    //  }

    //  client.close();

}





#include "rtos.h"
#include "devices/tsip.h"
#include "devices/com.h"

RTOS::RTOS(uint64_t timer)
    :Task("RTOS", HIGH_PRIORITY, timer) {
    // Create a new Scheduler by default (delfault pointer)
    _stateless = true;
    _scheduler = make_shared<Scheduler>(10);
}

RTOS::~RTOS(){
    // Dispose the memory allocated
    if (_scheduler && _scheduler->is_running()){
        _scheduler->stop();
        _scheduler = nullptr;
    }
}

/* WatchdogTask10000HZ (HIGH-PRIORITY)

This task is used o detect and recover from computer malfunctions. During 
normal operation, the computer regularly resets the watchdog timer to 
prevent it from elapsing, or "timing out".The timeout signal is used to 
initiate corrective action or actions. The corrective actions typically 
include placing the computer system in a safe state and restoring normal 
system operation.

*/
void WatchdogTask10000HZ(Task* task){
    cout << "Verifying the System... ";
    // Perform global verification of the System or Malfunctions
    // Deadlocks, devices, disconnectiosns, updates, overheating, etc..
    task->sleep(3000);
    // System checked correctly. Shutdown? Corrective actions?
    cout << "OK " << endl;
}

/* DMARead200Hz (MEDIUM-PRIORITY)

Following task is going to read de TSIP packets from the serial Port.
The serial port is read using DMA with a Buffer of 512

*/
void DMARead200Hz(Task* task){

    cout << "Reading Data from Serial Port... " << endl;

    // Get the current Queue to share the params
    Queue<TsipPacket*> *packet_queue = (Queue<TsipPacket*>*) task->get_parameters();
    
    // Create default variables
    uint8_t read_byte, DMA_buffer[512];

    // Check if last  TSIP message was interrupted
    TsipPacket *packet = nullptr;
    if (task->get_data()){
        // Get the last incomplete TSIP messages from the task
        packet = (TsipPacket*)task->get_data();
    } else {
        // Create a new one.
        packet = new TsipPacket();
    }
    
    // Get next batch of bytes from DEM Serial Port.
    int32_t byte_count = uavnComRead(DMA_buffer, sizeof DMA_buffer);

    // Loop through buffer read from Serial Port
    for (int32_t i = 0; i < byte_count; i += 1) {
        // Get the current byte
        read_byte = DMA_buffer[i];
        // Check if a TSIP packet has already started
        if (packet->active) {
            // Check if final of the current TSIP packets
            if (read_byte == ETX) {
               // Add current packet to the quaue
                packet_queue->push(packet);
                // Start new TSIP packet
                packet = new TsipPacket();
            } else {
                // Set the current byte into the TSIP buffer
                packet->data[packet->size++] = read_byte;
            }
        } else if (read_byte == DLE) {
                // New start TSIP packet detected
                packet->active = true;
        }
        // End TSIP packet not detected (DLE ETX)
        if (packet->size >= TSIP_SIZE) {
            // Reset current TSIP Packet from start (noise in data)
            packet->size = 0;
            packet->active = false;
        }
    }
    // Append current (uncompleted) TSIP data to the task
    task->set_data(packet);
}

/* TaskUplink200Hz (MEDIUM-PRIORITY)

Once the TSIP data is extracted from TSIP packet. The following task
will process such TSIP data, performing the required autopilot actions. 

*/
void TaskUplink200Hz(Task* task){

    cout << "Transforming and Parsing TSIP data... " << endl;

    Queue<TsipPacket*> *packet_queue = (Queue<TsipPacket*>*) task->get_parameters();
    // Check the queued TSIP packets, parse them and perform the action.
    while (!packet_queue->empty()) {
        // Get the next packet to parse
        TsipPacket *packet = packet_queue->pop();
        // Parse the current packet and perform the action
        ParseTsipData(packet->data, packet->size);
        // Delete current item since not a smart object
        delete packet;
        // Do somthing after parsing TSIP packet and perform action
        task->sleep(10);
    }
}

/* UpdateDisplay1000Hz (LOW-PRIORITY)

Update the display showing the information needed.
The display will be updated every 1 second.

*/
void UpdateDisplay1000Hz(Task* task){
    system("cls");
    cout << "*****************************************" << endl;
    cout << "*************    RTOS       *************" << endl;
    cout << "*****************************************" << endl;

    time_t _tm =time(NULL );
    struct tm * curtime = localtime ( &_tm );
    cout<< asctime(curtime);

    cout << "*****************************************" << endl;
    cout << "*****************************************" << endl;

}

bool RTOS::init(){
    // Create shared queue to be passed between tasks socket and tsip
    //shared_ptr<Queue<string>> packet_queue = make_shared<Queue<string>>();
    Queue<TsipPacket*> *packet_queue = new Queue<TsipPacket*>();
    // Initilaize the different subsystems, drivers, etc..
    _scheduler->add(make_shared<Task>("Watchdog", HIGH_PRIORITY, 10000, WatchdogTask10000HZ));
    // Sokets for TCPIP/COM connection
    _scheduler->add(make_shared<Task>("DMARead", MID_PRIORITY, 400, DMARead200Hz, packet_queue));
    // TSIP task, 200Hz, Non highest nor lowest.
    _scheduler->add(make_shared<Task>("Uplink", MID_PRIORITY, 200, TaskUplink200Hz, packet_queue));
    // Display Task to show the time
    _scheduler->add(make_shared<Task>("Display",LOW_PRIORITY, 1000, UpdateDisplay1000Hz));
     // Return the result of the initilization
    return true;
}

bool RTOS::start() {
    // Start the scheduler task at the same time as the current RTOS loop
    _scheduler->start();
     // Call to parent function
    return Task::start();
}


void RTOS::shutdown(){
    // Start the scheduler task at the same time as the current RTOS loop
    _scheduler->stop();
    // finally stop the main thread
    stop();
}

void RTOS::_default_callback(){
    // Main Process for the OS

    // Do something if needed
}





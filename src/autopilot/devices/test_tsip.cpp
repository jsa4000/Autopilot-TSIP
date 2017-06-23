#include "tsip.h"

/*
Following test will parse the TSIP data. If exception test will fail
*/
bool test_ParseTsipData(){
    try {
        //TsipPacket(2, 0x8E, 0xA9);   // self-survey parameters
        //TsipPacket(2, 0x8E, 0x41);   // manufacturing operating parameters
        //TsipPacket(2, 0x8E, 0x42);   // production parameters

        TsipPacket packet;
        packet.data[0] = 0x8E;
        packet.data[1] = 0xA9;
        packet.size = 2;

        ParseTsipData(packet.data, packet.size);
    } catch (std::ex const std::exception& e){
        // Test failed to return the TSIP data
        return false;
    }
}
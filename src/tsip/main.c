#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

#include "crc32.h"

/* PACKING TSIP MESASGES

1. Create the TSIP Package. ID1 (1byte) + ID2 (1byte) + DATA (n bytes)
	-> Packages are created with two IDs where  ID1 != (DLE,ETX)

2. Add 2 bytes S/N (Device insertion)
	-> ID1 (1byte) + ID2 (1byte) + DATA (n bytes) + S/N (2 bytes)
	
3. Add CRC32 (4 bytes)
	NOTES:  1. CRC are calculated with previous data -> ID1 + ID2 + DATA + S/N
			2. CRC doesn't take into consideration DLE + escape bytes
			3. DLE or DLE+ETX are not included in the CRC
			

4. Finally the insrtion. 

	NOTES: 1. This wil insert the DLE and DLE+ETX delimiting the message
		   2. Also it will add the DLE scape bytes to ensure integrity.

*/
bool tsip_pack(uint8_t id1, uint8_t id2, uint8_t* data, const uint32_t data_size, uint16_t sn) {
return true;
}


bool compute_fn(double z){  // [1]

    return true;
}


int main(void){

    int four4 = four();
    crc32(NULL,0,0);

    printf("%.7g\n", compute_fn(3.0));

    return 0;
}
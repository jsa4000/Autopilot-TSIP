#ifndef _TSIP_H_
#define _TSIP_H_

// TSIP Byte codes.
#define ETX 0x03
#define DLE 0x10

#include <cstdint>

/**
* \brief Parse a TSIP data.
*
* \param[in] buffer Const pointer where data is located.
* \param[in] numberOfBytes It is filled with the number bytes in the TSIP packet.
*/
void ParseTsipData(const uint8_t * const buffer, const int32_t numberOfBytes);


#endif //_TSIP_H_
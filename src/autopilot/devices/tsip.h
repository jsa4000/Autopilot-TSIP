#ifndef _TSIP_H_
#define _TSIP_H_

// TSIP byte definitions.

#define ETX 0x03
#define DLE 0x10

// Define global data for handling _current_ packet.

struct {
    byte data[1000];    // TSIP data buffer
    long index;         // current index into buffer
} Tsip;


// Define function prototypes, global data, macros.

void tsip_read (void);
void tsip_parse (void);
void tsip_write (void);

/**
* \brief Read received raw data from COM port.
*
* This is a non-blocking read function. This means that only available received
* data will be served. User may decide to call this function within a loop
* until the desired amount of data is received.
*
* \param buf Pointer to destination buffer where to copy received data.
* \param count Maximum number of bytes to be read.
* \return Number of read bytes. This value will always be <= count.
*/
int32_t uavnComRead(uint8_t * const buffer, const uint32_t count);

/**
* \brief Parse a TSIP data.
*
* \param[in] buffer Const pointer where data is located.
* \param[in] numberOfBytes It is filled with the number bytes in the TSIP packet.
*/
void ParseTsipData(const uint8_t * const buffer, const int32_t numberOfBytes);


#endif //_TSIP_H_


#include "../romloader.h"
#include "../uuencoder.h"


#ifndef __ROMLOADER_UART_READ_FUNCTINOID_H__
#define __ROMLOADER_UART_READ_FUNCTINOID_H__


class romloader_uart_read_functinoid : public romloader_read_functinoid
{
public:
	romloader_uart_read_functinoid(void) {}

	virtual bool read_data32(uint32_t ulAddress, uint32_t *pulData) = 0;
	virtual int update_device(ROMLOADER_CHIPTYP tChiptyp) = 0;

protected:
	unsigned int crc16(uint16_t usCrc, uint8_t ucData)
	{
		usCrc  = (usCrc >> 8) | ((usCrc & 0xff) << 8);
		usCrc ^= ucData;
		usCrc ^= (usCrc & 0xff) >> 4;
		usCrc ^= (usCrc & 0x0f) << 12;
		usCrc ^= ((usCrc & 0xff) << 4) << 1;

		return usCrc;
	};
};

#endif  /* __ROMLOADER_UART_READ_FUNCTINOID_H__ */


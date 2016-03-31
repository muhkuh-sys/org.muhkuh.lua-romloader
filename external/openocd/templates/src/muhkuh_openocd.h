#include <stddef.h>
#include <stdint.h>

#ifndef __MUHKUH_OPENOCD_H__
#define __MUHKUH_OPENOCD_H__

void *muhkuh_openocd_init(void);
int muhkuh_openocd_get_result(void *pvContext, char *pcBuffer, size_t sizBufferMax);
int muhkuh_openocd_command_run_line(void *pvContext, char *pcLine);
void muhkuh_openocd_uninit(void *pvContext);

int muhkuh_openocd_read_data08(void *pvContext, uint32_t ulNetxAddress, uint8_t *pucData);
int muhkuh_openocd_read_data16(void *pvContext, uint32_t ulNetxAddress, uint16_t *pusData);
int muhkuh_openocd_read_data32(void *pvContext, uint32_t ulNetxAddress, uint32_t *pulData);
int muhkuh_openocd_read_image(void *pvContext, uint32_t ulNetxAddress, uint8_t *pucData, uint32_t ulSize);
int muhkuh_openocd_write_data08(void *pvContext, uint32_t ulNetxAddress, uint8_t ucData);
int muhkuh_openocd_write_data16(void *pvContext, uint32_t ulNetxAddress, uint16_t usData);
int muhkuh_openocd_write_data32(void *pvContext, uint32_t ulNetxAddress, uint32_t ulData);
int muhkuh_openocd_write_image(void *pvContext, uint32_t ulNetxAddress, const uint8_t *pucData, uint32_t ulSize);


#endif  /* __MUHKUH_OPENOCD_H__ */


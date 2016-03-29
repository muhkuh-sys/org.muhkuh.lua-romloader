#include <stddef.h>

#ifndef __MUHKUH_OPENOCD_H__
#define __MUHKUH_OPENOCD_H__

void *muhkuh_openocd_init(void);
int muhkuh_openocd_get_result(void *pvContext, char *pcBuffer, size_t sizBufferMax);
int muhkuh_openocd_command_run_line(void *pvContext, char *pcLine);
void muhkuh_openocd_uninit(void *pvContext);

#endif  /* __MUHKUH_OPENOCD_H__ */


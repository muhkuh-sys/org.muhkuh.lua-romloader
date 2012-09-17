/*---------------------------------------------------------------------------
  Author : Christoph Thelen

           Hilscher GmbH, Copyright (c) 2006, All Rights Reserved

           Redistribution or unauthorized use without expressed written 
           agreement from the Hilscher GmbH is forbidden
---------------------------------------------------------------------------*/


#include "usb_globals.h"

#ifndef __usb__
#define __usb__

//-------------------------------------

void usb_init(void);
void usb_deinit(void);
USB_CDC_ConnectionState_t usb_pollConnection(void);

//-------------------------------------

#endif  // __usb__


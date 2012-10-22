/*---------------------------------------------------------------------------
  Author : Christoph Thelen

           Hilscher GmbH, Copyright (c) 2006, All Rights Reserved

           Redistribution or unauthorized use without expressed written 
           agreement from the Hilscher GmbH is forbidden
---------------------------------------------------------------------------*/

#include "usb_requests_common.h"
#include "usb_globals.h"

//---------------------------------------------------------------------------

USB_State_t globalState;

unsigned int currentConfig;

// buffer for setup and data packets
unsigned char setupBuffer[Usb_Ep0_PacketSize];

// decoded packet
setupPacket_t tSetupPkt;
// out transaction needed
USB_SetupTransaction_t tOutTransactionNeeded;

USB_ReceiveEndpoint_t tReceiveEpState;

// new address for pending address change
unsigned int uiNewAddress;

// CDC Connection State
USB_CDC_ConnectionState_t tCdcConnectionState;

//---------------------------------------------------------------------------


/*---------------------------------------------------------------------------
  Author : Christoph Thelen

           Hilscher GmbH, Copyright (c) 2006, All Rights Reserved

           Redistribution or unauthorized use without expressed written 
           agreement from the Hilscher GmbH is forbidden
---------------------------------------------------------------------------*/


#ifndef __descriptors__
#define __descriptors__

//---------------------------------------------------------------------------

void sendDescriptor(unsigned int reqiredLength, unsigned int descriptorLength, const unsigned char *descriptor);
packet_handler_stall_req_t getDescriptor(setupPacket_t *pPacket);
void sendStringDescriptor(unsigned int languageId, unsigned int stringIdx, unsigned int wLength);

//---------------------------------------------------------------------------


#endif  // __descriptors__


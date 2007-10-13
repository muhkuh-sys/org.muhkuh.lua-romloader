


%include "../romloader_openocd_main.h"
%include "../../romloader.h"
// The romloader class defines the connection to a single netx device in
// "boot" mode. It can not be created directly, but is returned by the
// function "SelectNetx".
%class %delete romloader
 // open the connection to the device
 void connect()
 // close the connection to the device
 void disconnect()
 // returns the connection state of the device
 bool is_connected()
 // returns the device name
 wxString get_name()
 // returns the device typ
 wxString get_typ()
 // read a byte (8bit) from the netx to the pc
 unsigned char read_data08(unsigned long ulNetxAddress)
 // read a word (16bit) from the netx to the pc
 unsigned short read_data16(unsigned long ulNetxAddress)
 // read a long (32bit) from the netx to the pc
 unsigned long read_data32(unsigned long ulNetxAddress)
 // read a byte array from the netx to the pc
 wxString read_image(unsigned long ulNetxAddress, unsigned long ulSize)

 // write a byte (8bit) from the pc to the netx
 void write_data08(unsigned long ulNetxAddress, unsigned char bData)
 // write a word (16bit) from the pc to the netx
 void write_data16(unsigned long ulNetxAddress, unsigned short usData)
 // write a long (32bit) from the pc to the netx
 void write_data32(unsigned long ulNetxAddress, unsigned long ulData)
 // write a byte array from the pc to the netx
 void write_image(unsigned long ulNetxAddress, wxString strData)

 // call routine
 void call(unsigned long ulNetxAddress, unsigned long ulParameterR0)
%endclass
%function %gc romloader *romloader_openocd_create(voidptr_long pvHandle)

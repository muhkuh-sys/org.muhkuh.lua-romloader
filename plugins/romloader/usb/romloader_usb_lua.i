


%include "../romloader_usb_main.h"
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
 double read_data08(double dNetxAddress)
 // read a word (16bit) from the netx to the pc
 double read_data16(double dNetxAddress)
 // read a long (32bit) from the netx to the pc
 double read_data32(double dNetxAddress)
 // read a byte array from the netx to the pc
 wxString read_image(double dNetxAddress, double dSize, LuaFunction fnCallback, voidptr_long vplCallbackUserData)

 // write a byte (8bit) from the pc to the netx
 void write_data08(double dNetxAddress, double dData)
 // write a word (16bit) from the pc to the netx
 void write_data16(double dNetxAddress, double dData)
 // write a long (32bit) from the pc to the netx
 void write_data32(double dNetxAddress, double dData)
 // write a byte array from the pc to the netx
 void write_image(double dNetxAddress, wxString strData, LuaFunction fnCallback, voidptr_long vplCallbackUserData)

 // call routine
 void call(double dNetxAddress, double dParameterR0, LuaFunction fnCallback, voidptr_long vplCallbackUserData)
%endclass
%function %gc romloader *romloader_usb_create(voidptr_long pvHandle)

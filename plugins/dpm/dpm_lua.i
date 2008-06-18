
// The dpm class defines the connection to a single netx dpm.
// It can not be created directly, but is returned by the
// function "SelectPlugin".
%class %delete dpm
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
	wxString read_image(unsigned long ulNetxAddress, unsigned long ulSize, LuaFunction fnCallback, voidptr_long vplCallbackUserData)

	// write a byte (8bit) from the pc to the netx
	void write_data08(unsigned long ulNetxAddress, unsigned char bData)
	// write a word (16bit) from the pc to the netx
	void write_data16(unsigned long ulNetxAddress, unsigned short usData)
	// write a long (32bit) from the pc to the netx
	void write_data32(unsigned long ulNetxAddress, unsigned long ulData)
	// write a byte array from the pc to the netx
	void write_image(unsigned long ulNetxAddress, wxString strData, LuaFunction fnCallback, voidptr_long vplCallbackUserData)
%endclass


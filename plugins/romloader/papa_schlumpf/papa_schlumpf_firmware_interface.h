#ifndef __PAPA_SCHLUMPF_FIRMWARE_INTERFACE_H__
#define __PAPA_SCHLUMPF_FIRMWARE_INTERFACE_H__

#include <stdint.h>

#define PAPA_SCHLUMPF_USB_VENDOR_ID               0x1939
#define PAPA_SCHLUMPF_USB_PRODUCT_ID              0x0028
#define PAPA_SCHLUMPF_USB_PRODUCT_RELEASE         0x0001

#define PAPA_SCHLUMPF_COMMAND_OVERHEAD            sizeof(unsigned long) - sizeof(uint32_t) - (2 * sizeof(uint32_t))
#define PAPA_SCHLUMPF_COMMAND_PACKET_SIZE         64
#define PAPA_SCHLUMPF_COMMAND_DATA_SIZE           PAPA_SCHLUMPF_COMMAND_PACKET_SIZE - PAPA_SCHLUMPF_COMMAND_OVERHEAD


/*Commands for USB communication between PC and Test board*/
typedef enum PAPA_SCHLUMPF_USB_COMMANDS_ENUM
{
	USB_COMMAND_Read_8 = 0,
	USB_COMMAND_Read_16 = 1,
	USB_COMMAND_Read_32 = 2,
	USB_COMMAND_Read_Image = 3,
	USB_COMMAND_Write_8 = 4,
	USB_COMMAND_Write_16  = 5,
	USB_COMMAND_Write_32 = 6,
	USB_COMMAND_Write_Image = 7,
	USB_COMMAND_Call = 8,
	USB_COMMAND_Start = 9,
} PAPA_SCHLUMPF_USB_COMMANDS_T;

/*Status for the command execution between PC and Test board*/
typedef enum PAPA_SCHLUMPF_USB_COMMAND_STATUS_ENUM
{
	USB_COMMAND_STATUS_Ok                  = 0,
	USB_COMMAND_STATUS_Error               = 1,
	USB_COMMAND_STATUS_Print               = 2,
	USB_COMMAND_STATUS_MAILBOX_READ_ERROR  = 3,
	USB_COMMAND_STATUS_MAILBOX_WRITE_ERROR = 4,
	USB_COMMAND_STATUS_EXECUTION_ERROR     = 5,
	USB_COMMAND_STATUS_UNKNOWN_COMMAND     = 6,
	USB_COMMAND_STATUS_CALL_DONE           = 7,
} PAPA_SCHLUMPF_USB_COMMAND_STATUS_T;

/*Indicates status for the raw usb communication*/
typedef enum USB_STATUS_ENUM
{
	USB_STATUS_OK = 0,
	USB_STATUS_NOT_ALL_BYTES_TRANSFERED = 1,
	USB_STATUS_ERROR = 2,
	USB_STATUS_RECEIVED_EMPTY_PACKET = 3,
} USB_STATUS_T;

typedef struct PAPA_SCHLUMPF_USB_PARAMETER_READ_STRUCT
{
	uint32_t ulAddress;
	uint32_t ulElemCount;
} PAPA_SCHLUMPF_USB_PARAMETER_READ_T;

typedef struct PAPA_SCHLUMPF_USB_PARAMETER_WRITE_STRUCT
{
	uint32_t ulAddress;
	uint32_t ulElemCount;
	union
	{
		uint8_t  ucData[PAPA_SCHLUMPF_COMMAND_DATA_SIZE];
		uint16_t usData[PAPA_SCHLUMPF_COMMAND_DATA_SIZE / sizeof(uint16_t)];
		uint32_t ulData[PAPA_SCHLUMPF_COMMAND_DATA_SIZE / sizeof(uint32_t)];
	} uData;
} PAPA_SCHLUMPF_USB_PARAMETER_WRITE_T;

typedef struct PAPA_SCHLUMPF_USB_PARAMETER_CALL_STRUCT
{
	uint32_t ulFunctionAddress;
	uint32_t ulFunctionParameter;
} PAPA_SCHLUMPF_USB_PARAMETER_CALL_T;

typedef struct PAPA_SCHLUMPF_PARAMETER_STRUCT
{
	uint32_t ulDummy;
	uint32_t ulPapaSchlumpfUsbCommand;
	union
	{
		PAPA_SCHLUMPF_USB_PARAMETER_READ_T tPapaSchlumpfUsbParameterRead;
		PAPA_SCHLUMPF_USB_PARAMETER_WRITE_T tPapaSchlumpfUsbParameterWrite;
		PAPA_SCHLUMPF_USB_PARAMETER_CALL_T tPapaSchlumpfUsbParameterCall;
	} uPapaSchlumpfUsbParameter;
} PAPA_SCHLUMPF_PARAMETER_T;


#endif  /* __PAPA_SCHLUMPF_FIRMWARE_INTERFACE_H__ */

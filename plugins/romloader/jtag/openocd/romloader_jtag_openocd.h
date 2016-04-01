#include <stddef.h>
#include <stdint.h>

#ifndef __ROMLOADER_JTAG_OPENOCD_H__
#define __ROMLOADER_JTAG_OPENOCD_H__


/*-------------------------------------------------------------------------*/




/*-------------------------------------------------------------------------*/


class romloader_jtag_openocd
{
public:
	romloader_jtag_openocd(void);
	~romloader_jtag_openocd(void);


	typedef struct OPENOCD_NAME_RESOLVE_STRUCT
	{
		const char *pstrSymbolName;
		size_t sizPointerOffset;
	} OPENOCD_NAME_RESOLVE_T;

	typedef struct INTERFACE_SETUP_STRUCT
	{
		const char *pcID;
		const char *pcCode_Setup;
		const char *pcCode_Probe;
	} INTERFACE_SETUP_STRUCT_T;

	typedef struct TARGET_SETUP_STRUCT
	{
		const char *pcID;
		const char *pcCode;
	} TARGET_SETUP_STRUCT_T;

	typedef void * (*PFN_MUHKUH_OPENOCD_INIT_T) (void);
	typedef int (*PFN_MUHKUH_OPENOCD_GET_RESULT_T) (void *pvContext, char *pcBuffer, size_t sizBufferMax);
	typedef int (*PFN_MUHKUH_OPENOCD_COMMAND_RUN_LINE_T) (void *pvContext, const char *pcLine);
	typedef void (*PFN_MUHKUH_OPENOCD_UNINIT_T) (void *pvContext);

	typedef int (*PFN_MUHKUH_OPENOCD_READ_DATA08) (void *pvContext, uint32_t ulNetxAddress, uint8_t *pucData);
	typedef int (*PFN_MUHKUH_OPENOCD_READ_DATA16) (void *pvContext, uint32_t ulNetxAddress, uint16_t *pusData);
	typedef int (*PFN_MUHKUH_OPENOCD_READ_DATA32) (void *pvContext, uint32_t ulNetxAddress, uint32_t *pulData);
	typedef int (*PFN_MUHKUH_OPENOCD_READ_IMAGE) (void *pvContext, uint32_t ulNetxAddress, uint8_t *pucData, uint32_t ulSize);
	typedef int (*PFN_MUHKUH_OPENOCD_WRITE_DATA08) (void *pvContext, uint32_t ulNetxAddress, uint8_t ucData);
	typedef int (*PFN_MUHKUH_OPENOCD_WRITE_DATA16) (void *pvContext, uint32_t ulNetxAddress, uint16_t usData);
	typedef int (*PFN_MUHKUH_OPENOCD_WRITE_DATA32) (void *pvContext, uint32_t ulNetxAddress, uint32_t ulData);
	typedef int (*PFN_MUHKUH_OPENOCD_WRITE_IMAGE) (void *pvContext, uint32_t ulNetxAddress, uint8_t *pucData, uint32_t ulSize);


	typedef struct MUHKUH_OPENOCD_FUNCTION_POINTERS_UNION
	{
		PFN_MUHKUH_OPENOCD_INIT_T pfnInit;
		PFN_MUHKUH_OPENOCD_GET_RESULT_T pfnGetResult;
		PFN_MUHKUH_OPENOCD_COMMAND_RUN_LINE_T pfnCommandRunLine;
		PFN_MUHKUH_OPENOCD_UNINIT_T pfnUninit;
		PFN_MUHKUH_OPENOCD_READ_DATA08 pfnReadData08;
		PFN_MUHKUH_OPENOCD_READ_DATA16 pfnReadData16;
		PFN_MUHKUH_OPENOCD_READ_DATA32 pfnReadData32;
		PFN_MUHKUH_OPENOCD_READ_IMAGE pfnReadImage;
		PFN_MUHKUH_OPENOCD_WRITE_DATA08 pfnWriteData08;
		PFN_MUHKUH_OPENOCD_WRITE_DATA16 pfnWriteData16;
		PFN_MUHKUH_OPENOCD_WRITE_DATA32 pfnWriteData32;
		PFN_MUHKUH_OPENOCD_WRITE_IMAGE pfnWriteImage;
	} MUHKUH_OPENOCD_FUNCTION_POINTERS_T;


	typedef union MUHKUH_OPENOCD_FUNCTIONS_UNION
	{
		MUHKUH_OPENOCD_FUNCTION_POINTERS_T tFn;
		void *pv[sizeof(MUHKUH_OPENOCD_FUNCTION_POINTERS_T)/sizeof(void*)];
	} MUHKUH_OPENOCD_FUNCTIONS_T;


	typedef struct ROMLOADER_JTAG_DEVICE_STRUCT
	{
		void *pvSharedLibraryHandle;
		MUHKUH_OPENOCD_FUNCTIONS_T tFunctions;
		void *pvOpenocdContext;
	} ROMLOADER_JTAG_DEVICE_T;


	typedef struct ROMLOADER_JTAG_DETECT_ENTRY_STRUCT
	{
		char *pcInterface;
		char *pcTarget;
	} ROMLOADER_JTAG_DETECT_ENTRY_T;


	int initialize(void);
	int detect(ROMLOADER_JTAG_DETECT_ENTRY_T **pptEntries, size_t *psizEntries);
	void uninit(void);

private:
	static const OPENOCD_NAME_RESOLVE_T atOpenOcdResolve[12];

	static const INTERFACE_SETUP_STRUCT_T atInterfaceCfg[4];

	static const TARGET_SETUP_STRUCT_T atTargetCfg[2];

	static const char *pcResetCode;


	ROMLOADER_JTAG_DETECT_ENTRY_T *m_ptDetected;
	size_t m_sizDetectedCnt;
	size_t m_sizDetectedMax;


	void free_detect_entries(void);
	int add_detected_entry(const char *pcInterface, const char *pcTarget);

	int openocd_open(ROMLOADER_JTAG_DEVICE_T *ptDevice);
	void openocd_close(ROMLOADER_JTAG_DEVICE_T *ptDevice);
	int romloader_jtag_openocd_init(void);

	int setup_interface(ROMLOADER_JTAG_DEVICE_T *ptDevice, const INTERFACE_SETUP_STRUCT_T *ptIfCfg);
	int probe_interface(ROMLOADER_JTAG_DEVICE_T *ptDevice, const INTERFACE_SETUP_STRUCT_T *ptIfCfg);
	int probe_target(ROMLOADER_JTAG_DEVICE_T *ptDevice, const INTERFACE_SETUP_STRUCT_T *ptIfCfg, const TARGET_SETUP_STRUCT_T *ptTargetCfg);
	int detect_target(const INTERFACE_SETUP_STRUCT_T *ptIfCfg);
};


#endif  /* __ROMLOADER_JTAG_OPENOCD_H__ */

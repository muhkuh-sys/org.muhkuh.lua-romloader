
#ifndef __main__
#define __main__


typedef enum
{
	NETX_CONSOLEAPP_RESULT_OK = 0,
	NETX_CONSOLEAPP_RESULT_ERROR = 1
} NETX_CONSOLEAPP_RESULT_T;

typedef struct
{
	unsigned long ulReturnValue;
	void *pvInitParams;
	void *pvReturnMessage;
} NETX_CONSOLEAPP_PARAMETER_T;


#endif


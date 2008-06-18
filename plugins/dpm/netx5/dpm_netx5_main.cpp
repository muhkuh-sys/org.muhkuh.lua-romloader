/***************************************************************************
 *   Copyright (C) 2007 by Christoph Thelen                                *
 *   doc_bacardi@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include <windows.h>
#include <winioctl.h>

#include "dpm_netx5_main.h"
#include "_luaif/dpm_netx5_wxlua_bindings.h"


/*-------------------------------------*/

#define CIFX_IO_CTRL_DPMINFO  0x00224003
#define CIFX_IO_CTRL_DPMMAP   0x0022400f
#define CIFX_IO_CTRL_DPMUNMAP 0x00224013

/*-------------------------------------*/


typedef struct
{
  union
  {
    volatile unsigned char aucRaw[256];
    volatile unsigned short ausRaw[128];
    volatile unsigned long aulRaw[64];
    struct
    {
      volatile unsigned long ulCfg0x0;
      volatile unsigned long aulReserved04[3];
      volatile unsigned long ulAddrCfg;
      volatile unsigned long ulTimingCfg;
      volatile unsigned long ulRdyCfg;
      volatile unsigned long ulStatus;
      volatile unsigned long ulStatusErrAutoReset;
      volatile unsigned long ulStatusErrAddr;
      volatile unsigned long aulReserved28[6];
      struct
      {
        volatile unsigned long ulWinEnd;
        volatile unsigned long ulWinMap;
      } atDpmWin[4];
      volatile unsigned long aulReserved60[8];
      volatile unsigned long ulIrqRaw;
      volatile unsigned long ulIrqFiqMask;
      volatile unsigned long ulIrqIrqMask;
      volatile unsigned long ulIrqFiqMasked;
      volatile unsigned long ulIrqIrqMasked;
      volatile unsigned long ulFiqIrqPadCtrl;
      volatile unsigned long aulReserved98[2];
      volatile unsigned long ulIoCfgMisc;
      volatile unsigned long aulReservedA4[3];
      volatile unsigned long ulPioOut0;
      volatile unsigned long ulPioOut1;
      volatile unsigned long ulPioOe0;
      volatile unsigned long ulPioOe1;
      volatile unsigned long ulPioIn0;
      volatile unsigned long ulPioIn1;
      volatile unsigned long aulReservedC8[13];
      volatile unsigned long ulNetxVersion;
    } sReg;
  };
} dpmMemoryLayout_t;


typedef enum
{
  DPM_SIZE_08,
  DPM_SIZE_16,
  DPM_SIZE_32
} tDpmSize;


/*-------------------------------------*/
/* configuration */

/* number of instances which can be used simultneous */
static const unsigned int uiInstances = 1;

/*-------------------------------------*/
/* some local prototypes */

bool fn_connect(void *pvHandle);
void fn_disconnect(void *pvHandle);
bool fn_is_connected(void *pvHandle);
int fn_read_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char *pucData);
int fn_read_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short *pusData);
int fn_read_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long *pulData);
int fn_read_image(void *pvHandle, unsigned long ulNetxAddress, char *pbData, unsigned long ulSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);
int fn_write_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char ucData);
int fn_write_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short usData);
int fn_write_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulData);
int fn_write_image(void *pvHandle, unsigned long ulNetxAddress, const char *pucData, unsigned long ulSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);

/*-------------------------------------*/

static muhkuh_plugin_desc plugin_desc =
{
	wxT("netX5 DPM plugin"),
	wxT(""),
	{ 0, 0, 2 }
};

const dpm_functioninterface tFunctionInterface =
{
	fn_connect,
	fn_disconnect,
	fn_is_connected,
	fn_read_data08,
	fn_read_data16,
	fn_read_data32,
	fn_read_image,
	fn_write_data08,
	fn_write_data16,
	fn_write_data32,
	fn_write_image
};

static wxLuaState *m_ptLuaState;


typedef struct
{
  long *plError;              // Driver error
  void *pvHandle;             // Handle of mapping
  void **ppvDPMPointer;       // Memory pointer
  unsigned long ulDataLength; // Complete size of the mapped memory
} tDpmIoCtrl;


/*-------------------------------------*/
/* config struct for all instances, this is just the connect flag at the moment */


typedef struct
{
	bool fIsUsed;
	bool fIsConnected;
	HANDLE hDriver;
	tDpmIoCtrl tDPMInfo;
	dpmMemoryLayout_t *ptDpm;
} dpm_netx5_instance_cfg_t;

dpm_netx5_instance_cfg_t atInstanceCfg[uiInstances];

/*-------------------------------------*/

int fn_init(wxLog *ptLogTarget, wxXmlNode *ptCfgNode, wxString &strPluginId)
{
	wxLog *pOldLogTarget;
	dpm_netx5_instance_cfg_t *ptC, *ptE;
	wxString strMsg;


	/* set main app's log target */
	pOldLogTarget = wxLog::GetActiveTarget();
	if( pOldLogTarget!=ptLogTarget )
	{
		wxLog::SetActiveTarget(ptLogTarget);
		if( pOldLogTarget!=NULL )
		{
			delete pOldLogTarget;
		}
	}

	/* say hi */
	strMsg.Printf(wxT("dpm netx5 plugin init, preparing %d instance(s)"), uiInstances);
	wxLogMessage(strMsg);

	/* remember id */
	plugin_desc.strPluginId = strPluginId;

	/* init the lua state */
	m_ptLuaState = NULL;

	/* loop over all instance configs and init them */
	ptC = atInstanceCfg;
	ptE = ptC + uiInstances;
	while( ptC<ptE )
	{
		/* init instance */
		ptC->fIsUsed = false;
		ptC->fIsConnected = false;
    ptC->hDriver = INVALID_HANDLE_VALUE;
    memset(&(ptC->tDPMInfo), 0, sizeof(tDpmIoCtrl));
    ptC->ptDpm = NULL;
		/* next instance */
		++ptC;
	}

	return 0;
}


/*-------------------------------------*/

int fn_init_lua(wxLuaState *ptLuaState)
{
	bool fSuccess;


	/* remember the lua state for instance creation */
	m_ptLuaState = ptLuaState;

	/* init the lua bindings */
	fSuccess = wxLuaBinding_dpm_netx5_lua_init();
//	fSuccess = wxLuaBinding_dpm_netx5_lua_bind(m_ptLuaState);
	if( fSuccess!=true )
	{
		wxLogMessage(wxT("failed to init dpm_netx5 lua bindings"));
		return -1;
	}

	return 0;
}


/*-------------------------------------*/

int fn_leave(void)
{
	unsigned int uiCnt;
	wxString strMsg;


	wxLogMessage(wxT("dpm_netx5 plugin leave"));

	/* check for connected instances */
	for(uiCnt=0; uiCnt<uiInstances; ++uiCnt)
	{
		if( atInstanceCfg[uiCnt].fIsUsed==true )
		{
			strMsg.Printf(wxT("dpm_netx5 plugin instance %d is still used"), uiInstances);
			wxLogError(strMsg);
		}
		if( atInstanceCfg[uiCnt].fIsConnected==true )
		{
			strMsg.Printf(wxT("dpm_netx5 plugin instance %d is still connected"), uiInstances);
			wxLogError(strMsg);
		}
	}

	return 0;
}

/*-------------------------------------*/

const muhkuh_plugin_desc *fn_get_desc(void)
{
	return &plugin_desc;
}

/*-------------------------------------*/


static int setUnstableReg32(dpmMemoryLayout_t *ptDpm, size_t sizOffset, unsigned long ulData)
{
  int iRetryCnt;
  unsigned long ulReadBack;


  wxLogMessage(_("dpm_netx5: set unstable reg 0x%04x to 0x%08x"), sizOffset, ulData);
  iRetryCnt = 16384;
  do
  {
    // write register
    ptDpm->aulRaw[sizOffset/sizeof(unsigned long)] = ulData;
    // readback data
    ulReadBack = ptDpm->aulRaw[sizOffset/sizeof(unsigned long)];
    if( ulReadBack==ulData )
    {
      break;
    }
  } while( --iRetryCnt>0 );

  if( iRetryCnt>0 )
  {
      wxLogMessage(_("dpm_netx5: set unstable reg ok!"));
  }
  else
  {
      wxLogError(_("dpm_netx5: set unstable reg falied!"));  
  }

  return iRetryCnt;
}


static bool setup_netx5_dpm(unsigned int uiInstance)
{
  dpmMemoryLayout_t *ptDpm;
  tDpmSize tSize;
	int iResult;
	unsigned long ulValue;
	unsigned char ucCfg0x0;
	int iWidth;
	wxString strMsg;
	bool fResult;


  // TODO: get settings from driver
  tSize = DPM_SIZE_32;

  // assume failure
  fResult = false;

  // check instance
  if( uiInstance>=uiInstances )
  {
    wxLogMessage(_("setup_netx5_dpm: invalid instance %d, max is %d"), uiInstance, uiInstances);
  }
  else
  {
    // is the driver handle valid?
    ptDpm = atInstanceCfg[uiInstance].ptDpm;
    if( ptDpm==NULL )
    {
      wxLogMessage(_("setup_netx5_dpm: instance %d has no dpm mapped yet!"), uiInstance);
    }
    else
    {
      switch( tSize )
      {
      case DPM_SIZE_08:
        ucCfg0x0 = 0x00;
        iWidth = 8;
        break;
      case DPM_SIZE_16:
        ucCfg0x0 = 0x05;
        iWidth = 16;
        break;
      case DPM_SIZE_32:
        ucCfg0x0 = 0x08;
        iWidth = 32;
        break;
      default:
        // invalid DPM size
        wxLogError(_("dpm_netx5: invalid dpm size id: %d"), tSize);
        return false;
        break;
      }

      wxLogMessage(_("dpm_netx5: init dpm to %d bits"), iWidth);

      // init the DPM to selected size
      wxLogMessage(_("setup_netx5_dpm: trying to set unstable reg 0x00 to 0x%08x"), ucCfg0x0);
      iResult = setUnstableReg32(ptDpm, 0x00, ucCfg0x0);
      if( iResult>0 )
      {
        wxLogMessage(_("setup_netx5_dpm: reg 0x00 is ok!"));

        wxLogMessage(_("setup_netx5_dpm: trying to set unstable reg 0x18 to 0x03"));
        iResult = setUnstableReg32(ptDpm, 0x18, 0x03);
        if( iResult>0 )
        {
          wxLogMessage(_("setup_netx5_dpm: reg 0x18 is ok!"));

          wxLogMessage(_("setup_netx5_dpm: trying to set unstable reg 0xa0 to 0x00"));
          iResult = setUnstableReg32(ptDpm, 0xa0, 0x00);
          if( iResult>0 )
          {
            wxLogMessage(_("setup_netx5_dpm: reg 0xa0 is ok!"));

            wxLogMessage(_("setup_netx5_dpm: trying to set unstable reg 0x10 to 0x07"));
            iResult = setUnstableReg32(ptDpm, 0x10, 0x07);
            if( iResult>0 )
            {
              wxLogMessage(_("setup_netx5_dpm: reg 0x10 is ok!"));

              // expect no netx5 DPM
              iResult = -1;

              wxLogMessage(_("dpm_netx5: dpm configured!"));

              // check for netx version
              ulValue = ptDpm->sReg.ulNetxVersion;
              wxLogMessage(_("dpm_netx5: read version register: 0x%08x"), ulValue);
              // mask out the license bits
              ulValue &= 0x000000ff;
              if( ulValue!=0x00000041 )
              {
                wxLogError(_("dpm_netx5: version register has unexpected value, must be 0x00000041!"));
              }
              else
              {
                // check the dpm_win4_end register, it is fixed to 0x000fff00
                ulValue = ptDpm->sReg.atDpmWin[3].ulWinEnd;
                wxLogMessage(_("dpm_netx5: read dpm_win_end3: 0x%08x"), ulValue);
                if( ulValue!=0x000fff00 )
                {
                  wxLogError(_("dpm_netx5: dpm_win_end3 register has unexpected value, must be 0x000fff00!"));
                }
                else
                {
                  // ok, netx5 DPM detected!
                  wxLogMessage(_("dpm_netx5: ok, netx5 dpm detected!"));
                  fResult = true;
                }
              }
            }
          }
        }
      }
    }
  }

  return fResult;
}


static int open_netx5_dpm(unsigned int uiInstance)
{
  HANDLE hDriver;
  long lError;
  void *pvDpm;
  tDpmIoCtrl tDPMInfo;
  BOOL fRes;
  DWORD dwBytesReturned;
	bool fResult;


  // assume failure
  fResult = false;

  // check instance
  if( uiInstance<uiInstances )
  {
    // is the driver handle valid?
    hDriver = atInstanceCfg[uiInstance].hDriver;
    if( hDriver!=INVALID_HANDLE_VALUE )
    {
      tDPMInfo.plError       = &lError;
      tDPMInfo.ppvDPMPointer = &pvDpm;

      wxLogMessage(_("Get DPM info..."));
	    fRes = ::DeviceIoControl(hDriver,                      // Handle of the device
                               CIFX_IO_CTRL_DPMINFO,         // Control code of operation to perform
                               &tDPMInfo,                    // Address of buffer for input data
                               sizeof(tDpmIoCtrl),           // Size of input buffer
                               NULL,                         // Address of output buffer
                               0,                            // Size of output buffer
                               &dwBytesReturned,             // Address of actual bytes of output
                               NULL                          // Address of overlapped structure
                              );
      if( fRes==TRUE && lError==0 )
      {
        wxLogMessage("Physical Address of DPM memory : %p", pvDpm);
        wxLogMessage("Size of mapped memory          : %p", tDPMInfo.ulDataLength);

        wxLogMessage(_("Map DPM to virtual memory..."));
        fRes = ::DeviceIoControl(hDriver,                      // Handle of the device
                                 CIFX_IO_CTRL_DPMMAP,          // Control code of operation to perform
                                 &tDPMInfo,                    // Address of buffer for input data
                                 sizeof(tDpmIoCtrl),           // Size of input buffer
                                 NULL,                         // Address of output buffer
                                 0,                            // Size of output buffer
                                 &dwBytesReturned,             // Address of actual bytes of output
                                 NULL                          // Address of overlapped structure
                                );
        if( fRes==TRUE && lError==0 )
        {
          atInstanceCfg[uiInstance].tDPMInfo.plError       = NULL;
          atInstanceCfg[uiInstance].tDPMInfo.pvHandle      = tDPMInfo.pvHandle;
          atInstanceCfg[uiInstance].tDPMInfo.ppvDPMPointer = (void**)&(atInstanceCfg[uiInstance].ptDpm);
          atInstanceCfg[uiInstance].tDPMInfo.ulDataLength  = tDPMInfo.ulDataLength;
          atInstanceCfg[uiInstance].ptDpm = (dpmMemoryLayout_t*)pvDpm;
          fResult = setup_netx5_dpm(uiInstance);
        }
      }
    }
  }

  return fResult;
}


static void close_netx5_dpm(unsigned int uiInstance)
{
  HANDLE hDriver;
	tDpmIoCtrl tDPMInfo;
  BOOL fRes;
  DWORD dwBytesReturned;
  long lError;


  // check instance
  if( uiInstance<uiInstances )
  {
    // is the driver handle valid?
    hDriver = atInstanceCfg[uiInstance].hDriver;
    if( hDriver!=INVALID_HANDLE_VALUE )
    {
      // is the dpm pointer valid?
      if( atInstanceCfg[uiInstance].ptDpm!=NULL )
      {
        tDPMInfo.plError = &lError;
        tDPMInfo.ppvDPMPointer = atInstanceCfg[uiInstance].tDPMInfo.ppvDPMPointer;
        tDPMInfo.pvHandle = atInstanceCfg[uiInstance].tDPMInfo.pvHandle;
        tDPMInfo.ulDataLength = atInstanceCfg[uiInstance].tDPMInfo.ulDataLength;

        // release the dpm pointer
        fRes = ::DeviceIoControl(hDriver,                               // Handle of the device
                                 CIFX_IO_CTRL_DPMUNMAP,                 // Control code of operation to perform
                                 &tDPMInfo,                             // Address of buffer for input data
                                 sizeof(tDpmIoCtrl),                    // Size of input buffer
                                 NULL,                                  // Address of output buffer
                                 0,                                     // Size of output buffer
                                 &dwBytesReturned,                      // Address of actual bytes of output
                                 NULL                                   // Address of overlapped structure
                                );
        if( fRes!=TRUE )
        {
          wxLogError(_("Failed to unmap netX5 DPM pointer!"));
        }

        atInstanceCfg[uiInstance].ptDpm = NULL;
        memset(&(atInstanceCfg[uiInstance].tDPMInfo), 0, sizeof(tDpmIoCtrl));
      }

      // Close the driver
      atInstanceCfg[uiInstance].hDriver = INVALID_HANDLE_VALUE;
		  CloseHandle(hDriver);
    }
  }
}


/*-------------------------------------*/


int fn_detect_interfaces(std::vector<muhkuh_plugin_instance*> *pvInterfaceList)
{
	unsigned int uiInterfaceCnt;
	unsigned int uiInterfacesDetected;
	muhkuh_plugin_instance *ptInst;
	wxString strDrvName;
	wxString strName;
	wxString strTyp;
	wxString strLuaCreateFn;
	void *pvHandle;
	bool fIsUsed;
	void *pvDpm;
	HANDLE hDriver;
  dpmMemoryLayout_t *ptDpm;
  bool fResult;


	strTyp = plugin_desc.strPluginId;
	strLuaCreateFn = wxT("muhkuh.dpm_netx5_create");

  uiInterfacesDetected = 0;

	// detect all interfaces
	for(uiInterfaceCnt=0; uiInterfaceCnt<uiInstances; ++uiInterfaceCnt)
	{
	  // interface already open?
	  if( atInstanceCfg[uiInterfaceCnt].ptDpm==NULL )
	  {
	    // no, interface not open yet
	    strDrvName.Printf(wxT("\\\\.\\cifX%d"), uiInterfaceCnt);
	    hDriver = ::CreateFile( strDrvName.fn_str(),                               // Address of name of the file           
	                            GENERIC_READ | GENERIC_WRITE,                      // Access (read-write) mode              
	                            FILE_SHARE_READ | FILE_SHARE_WRITE,                // Share mode                            
	                            NULL,                                              // Address of security descriptor        
	                            OPEN_EXISTING,                                     // How to create                         
                              FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING,    // File attributes            
	                            NULL                                               // Handle of file with attributes to copy
                            );
	    if( hDriver!=INVALID_HANDLE_VALUE )
	    {
	    
	      atInstanceCfg[uiInterfaceCnt].hDriver = hDriver;

  		  // Read the DPM pointer from the driver
		    fResult = open_netx5_dpm(uiInterfaceCnt);
		    if( fResult==true )
		    {
		      strName.Printf("dpm_netx5_%d", uiInterfaceCnt);
		      pvHandle = (void*)uiInterfaceCnt;
		      fIsUsed = atInstanceCfg[uiInterfaceCnt].fIsUsed;
		      ptInst = new muhkuh_plugin_instance(strName, strTyp, fIsUsed, strLuaCreateFn, pvHandle);

		      pvInterfaceList->push_back(ptInst);

          ++uiInterfacesDetected;
		    }
		    else
		    {
		      close_netx5_dpm(uiInterfaceCnt);
		    }
		  }
		}
	}

	return (int)uiInterfaceCnt;
}


/*-------------------------------------*/

static void dpm_netx5_close_instance(void *pvHandle)
{
	unsigned int uiIdx;
	wxString strMsg;
	bool fIsUsed;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("dpm_netx5_close_instance: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
	}
	else
	{
		/* is the instance really used? */
		fIsUsed = atInstanceCfg[uiIdx].fIsUsed;
		if( fIsUsed==false )
		{
			strMsg.Printf(wxT("dpm_netx5_close_instance: plugin instance %p is not in use"), pvHandle);
			wxLogError(strMsg);
		}
		else
		{
			strMsg.Printf(wxT("dpm_netx5 %d: close"), uiIdx);
			wxLogMessage(strMsg);
			/* free the instance */
			atInstanceCfg[uiIdx].fIsUsed = false;
			// close the driver
			close_netx5_dpm(uiIdx);
		}
	}
}


/*-------------------------------------*/

dpm *dpm_netx5_create(void *pvHandle)
{
	unsigned int uiIdx;
	wxString strMsg;
	wxString strName;
	wxString strTyp;
	dpm *ptInstance;
	bool fIsUsed;


	ptInstance = NULL;

	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("dpm_netx5_create: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
	}
	else
	{
		/* is the instance really unused? */
		fIsUsed = atInstanceCfg[uiIdx].fIsUsed;
		if( fIsUsed==true )
		{
			strMsg.Printf(wxT("dpm_netx5_create: plugin instance %p is already in use"), pvHandle);
			wxLogError(strMsg);
		}
		else
		{
			/* create the new instance */
			strTyp = plugin_desc.strPluginId;
			strName.Printf("dpm_netx5_%d", uiIdx);
			ptInstance = new dpm(strName, strTyp, &tFunctionInterface, pvHandle, dpm_netx5_close_instance, m_ptLuaState);
			atInstanceCfg[uiIdx].fIsUsed = true;
		}
	}

	return ptInstance;
}


/*-------------------------------------*/

/* open the connection to the device */
bool fn_connect(void *pvHandle)
{
	unsigned int uiIdx;
	wxString strMsg;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("dpm_netx5_fn_connect: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return false;
	}
	else
	{
		strMsg.Printf(wxT("dpm_netx5 %d: connected"), uiIdx);
		wxLogMessage(strMsg);
		atInstanceCfg[uiIdx].fIsConnected = true;
		return true;
	}
}

/* close the connection to the device */
void fn_disconnect(void *pvHandle)
{
	unsigned int uiIdx;
	wxString strMsg;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("dpm_netx5_fn_disconnect: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
	}
	else
	{
		strMsg.Printf(wxT("dpm_netx5 %d: disconnected"), uiIdx);
		wxLogMessage(strMsg);
		atInstanceCfg[uiIdx].fIsConnected = false;
	}
}

/* returns the connection state of the device */
bool fn_is_connected(void *pvHandle)
{
	unsigned int uiIdx;
	wxString strMsg;
	bool fIsConnected;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("dpm_netx5_fn_is_connected: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return false;
	}
	else
	{
		fIsConnected = atInstanceCfg[uiIdx].fIsConnected;
		strMsg.Printf(wxT("dpm_netx5 %d: is_connected = %d"), uiIdx, (fIsConnected?1:0));
		wxLogMessage(strMsg);
		return fIsConnected;
	}
}

/* read a byte (8bit) from the netx to the pc */
int fn_read_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char *pucData)
{
	unsigned int uiIdx;
	wxString strMsg;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("dpm_netx5_fn_read_data08: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return -1;
	}
	else
	{
		strMsg.Printf(wxT("dpm_netx5 %d: read08 from 0x%08lx"), uiIdx, ulNetxAddress);
		wxLogMessage(strMsg);
		if( pucData!=NULL )
		{
			*pucData = atInstanceCfg[uiIdx].ptDpm->aucRaw[ulNetxAddress];
		}
		return 0;
	}
}

/* read a word (16bit) from the netx to the pc */
int fn_read_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short *pusData)
{
	unsigned int uiIdx;
	wxString strMsg;
	unsigned long ulAlignedAdr;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("dpm_netx5_fn_read_data16: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return -1;
	}
	else
	{
	  if( (ulNetxAddress&1)!=0 )
	  {
  		strMsg.Printf(wxT("dpm_netx5 %d: read16 from unaligned address 0x%08lx, aligning to 0x%08lx"), uiIdx, ulNetxAddress, ulNetxAddress&0xfffffffe);
	  	wxLogWarning(strMsg);
	  }
	  // align address
	  ulAlignedAdr = ulNetxAddress>>1;
		strMsg.Printf(wxT("dpm_netx5 %d: read16 from 0x%08lx"), uiIdx, ulNetxAddress);
		wxLogMessage(strMsg);
		if( pusData!=NULL )
		{
			*pusData = atInstanceCfg[uiIdx].ptDpm->ausRaw[ulAlignedAdr];;
		}
		return 0;
	}
}

/* read a long (32bit) from the netx to the pc */
int fn_read_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long *pulData)
{
	unsigned int uiIdx;
	wxString strMsg;
	unsigned long ulAlignedAdr;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("dpm_netx5_fn_read_data32: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return -1;
	}
	else
	{
	  if( (ulNetxAddress&3)!=0 )
	  {
  		strMsg.Printf(wxT("dpm_netx5 %d: read32 from unaligned address 0x%08lx, aligning to 0x%08lx"), uiIdx, ulNetxAddress, ulNetxAddress&0xfffffffc);
	  	wxLogWarning(strMsg);
	  }
	  // align address
	  ulAlignedAdr = ulNetxAddress>>2;
		strMsg.Printf(wxT("dpm_netx5 %d: read32 from 0x%08lx"), uiIdx, ulNetxAddress);
		wxLogMessage(strMsg);
		if( pulData!=NULL )
		{
			*pulData = atInstanceCfg[uiIdx].ptDpm->aulRaw[ulAlignedAdr];;
		}
		return 0;
	}
}


/* read a byte array from the netx to the pc */
int fn_read_image(void *pvHandle, unsigned long ulNetxAddress, char *pcData, unsigned long ulSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	unsigned int uiIdx;
	wxString strMsg;
	void *pvSrc;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("dpm_netx5_fn_read_image: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return -1;
	}
	else
	{
		strMsg.Printf(wxT("dpm_netx5 %d: read_image from 0x%08lx with %ld bytes"), uiIdx, ulNetxAddress, ulSize);
		wxLogMessage(strMsg);
		if( pcData!=NULL )
		{
		  pvSrc = (void*)(atInstanceCfg[uiIdx].ptDpm->aucRaw + ulNetxAddress);
		  memcpy(pcData, pvSrc, ulSize);
		}
		return 0;
	}
}


/* write a byte (8bit) from the pc to the netx */
int fn_write_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char ucData)
{
	unsigned int uiIdx;
	wxString strMsg;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("dpm_netx5_fn_write_data08: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return -1;
	}
	else
	{
		strMsg.Printf(wxT("dpm_netx5 %d: write08 from 0x%08lx with 0x%02x"), uiIdx, ulNetxAddress, ucData);
		wxLogMessage(strMsg);
		atInstanceCfg[uiIdx].ptDpm->aucRaw[ulNetxAddress] = ucData;
		return 0;
	}
}


/* write a word (16bit) from the pc to the netx */
int fn_write_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short usData)
{
	unsigned int uiIdx;
	wxString strMsg;
	unsigned long ulAlignedAdr;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("dpm_netx5_fn_write_data16: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return -1;
	}
	else
	{
	  if( (ulNetxAddress&1)!=0 )
	  {
  		strMsg.Printf(wxT("dpm_netx5 %d: write16 to unaligned address 0x%08lx, aligning to 0x%08lx"), uiIdx, ulNetxAddress, ulNetxAddress&0xfffffffe);
	  	wxLogWarning(strMsg);
	  }
	  // align address
	  ulAlignedAdr = ulNetxAddress>>1;
		strMsg.Printf(wxT("dpm_netx5 %d: write16 from 0x%08lx with 0x%04x"), uiIdx, ulNetxAddress, usData);
		wxLogMessage(strMsg);
		atInstanceCfg[uiIdx].ptDpm->ausRaw[ulAlignedAdr] = usData;
		return 0;
	}
}


/* write a long (32bit) from the pc to the netx */
int fn_write_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulData)
{
	unsigned int uiIdx;
	wxString strMsg;
	unsigned long ulAlignedAdr;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("dpm_netx5_fn_write_data32: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return -1;
	}
	else
	{
	  if( (ulNetxAddress&3)!=0 )
	  {
  		strMsg.Printf(wxT("dpm_netx5 %d: write32 to unaligned address 0x%08lx, aligning to 0x%08lx"), uiIdx, ulNetxAddress, ulNetxAddress&0xfffffffc);
	  	wxLogWarning(strMsg);
	  }
	  // align address
	  ulAlignedAdr = ulNetxAddress>>2;
		strMsg.Printf(wxT("dpm_netx5 %d: write32 from 0x%08lx with 0x%08lx"), uiIdx, ulNetxAddress, ulData);
		wxLogMessage(strMsg);
		atInstanceCfg[uiIdx].ptDpm->aulRaw[ulAlignedAdr] = ulData;
		return 0;
	}
}


/* write a byte array from the pc to the netx */
int fn_write_image(void *pvHandle, unsigned long ulNetxAddress, const char *pcData, unsigned long ulSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	unsigned int uiIdx;
	wxString strMsg;
	void *pvDst;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("dpm_netx5_fn_write_image: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return -1;
	}
	else
	{
		strMsg.Printf(wxT("dpm_netx5 %d: write_image from 0x%08lx with %ld bytes:"), uiIdx, ulNetxAddress, ulSize);
		wxLogMessage(strMsg);
		pvDst = (void*)(atInstanceCfg[uiIdx].ptDpm->aucRaw + ulNetxAddress);
		memcpy(pvDst, pcData, ulSize);
		return 0;
	}
}


/*-------------------------------------*/

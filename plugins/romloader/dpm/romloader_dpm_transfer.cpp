#include "romloader_dpm_transfer.h"

#include "romloader_dpm_memorylayout.h"


romloader_dpm_transfer::romloader_dpm_transfer(romloader_dpm_device *ptDpmDevice)
 : m_ptDpmDevice(ptDpmDevice)
{
}


romloader_dpm_transfer::~romloader_dpm_transfer(void)
{
	if( m_ptDpmDevice!=NULL )
	{
		delete m_ptDpmDevice;
	}
}



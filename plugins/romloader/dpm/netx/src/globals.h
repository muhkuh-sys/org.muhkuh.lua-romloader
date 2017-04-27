/*
 * globals.h
 *
 *  Created on: 22.03.2017
 *      Author: jpriesnitz
 */

#include "asic_types.h" // Definitely needed
#include "netx_io_areas.h"



#ifndef PLUGINS_ROMLOADER_DPM_NETX_SRC_GLOBALS_H_
#define PLUGINS_ROMLOADER_DPM_NETX_SRC_GLOBALS_H_



#if ASIC_TYP==ASIC_TYP_NETX500
#define HOSTDEF_PT_DPM_AREA HOSTDEF(ptDpmArea)
#define HOSTDEF_PT_HANDSHAKE_ARM_MIRROR_AREA HOSTDEF(ptHandshakeDtcmArmMirrorArea)
#define PT_HANDSCHAKE_ARM_MIRROR_AREA_HANDSHAKE_REG ptHandshakeDtcmArmMirrorArea->aulHandshakeReg[0]

#elif ASIC_TYP==ASIC_TYP_NETX50
#define HOSTDEF_PT_DPM_AREA HOSTDEF(ptDpmArea)
#define HOSTDEF_PT_HANDSHAKE_ARM_MIRROR_AREA HOSTDEF(ptHandshakeDtcmArmMirrorArea)
#define PT_HANDSCHAKE_ARM_MIRROR_AREA_HANDSHAKE_REG ptHandshakeDtcmArmMirrorArea->aulHandshakeReg[0]

#elif ASIC_TYP==ASIC_TYP_NETX10
#define HOSTDEF_PT_DPM_AREA HOSTDEF(ptDpmArea)
#define HOSTDEF_PT_HANDSHAKE_ARM_MIRROR_AREA HOSTDEF(ptHandshakeDtcmArmMirrorArea)
#define PT_HANDSCHAKE_ARM_MIRROR_AREA_HANDSHAKE_REG ptHandshakeDtcmArmMirrorArea->aulHandshakeReg[0]

#elif ASIC_TYP==ASIC_TYP_NETX56
#define HOSTDEF_PT_DPM_AREA HOSTDEF(ptDpmArea)
#define HOSTDEF_PT_HANDSHAKE_ARM_MIRROR_AREA HOSTDEF(ptHandshakeDtcmArmMirrorArea)
#define PT_HANDSCHAKE_ARM_MIRROR_AREA_HANDSHAKE_REG ptHandshakeDtcmArmMirrorArea->aulHandshakeReg[0]

#elif ASIC_TYP==ASIC_TYP_NETX4000_RELAXED
#define HOSTDEF_PT_DPM_AREA HOSTDEF(ptDpmArea)
#define HOSTDEF_PT_HANDSHAKE_ARM_MIRROR_AREA HOSTDEF(ptHandshake0ArmMirrorArea)
#define PT_HANDSCHAKE_ARM_MIRROR_AREA_HANDSHAKE_REG ptHandshake0ArmMirrorArea->aulHandshakeReg[0]
#else
#       error "Unknown ASIC type!"
#define HOSTDEF_PT_DPM_AREA HOSTDEF(Addr_NX4000_dpm)
#define HOSTDEF_PT_HANDSHAKE_ARM_MIRROR_AREA NULL
#define PT_HANDSCHAKE_ARM_MIRROR_AREA_HANDSHAKE_REG NULL


#endif

#endif /* PLUGINS_ROMLOADER_DPM_NETX_SRC_GLOBALS_H_ */

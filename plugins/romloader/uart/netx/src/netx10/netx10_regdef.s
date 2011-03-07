@ ******************************************************************
@ from regdef.v by regdef2s auto-generated Assembler file           
@ please see the regdef.html file for detailed register description 
@ ******************************************************************

@ =====================================================================
@
@ Area ARMiTCM_memarea
 .equ Addr_NX10_ARMiTCM_memarea, 0x00000000
@
@ =====================================================================
@ =====================================================================
@
@ Area intram0_start4
 .equ Addr_NX10_intram0_mirror_itcm, 0x00000000
@
@ =====================================================================
 .equ Adr_NX10_intram0_mirror_itcm_intram0_base_start4, 0x00000004
 .equ Adr_NX10_intram0_base_start4                    , 0x00000004
 .equ Adr_NX10_intram0_mirror_itcm_intram0_end_start4 , 0x0000FFFC
 .equ Adr_NX10_intram0_end_start4                     , 0x0000FFFC

@ =====================================================================
@
@ Area arm_boot_vector
 .equ Addr_NX10_arm_boot_vector, 0x00000000
@
@ =====================================================================
 .equ Adr_NX10_arm_boot_vector_arm_boot_vector, 0x00000000
 .equ Adr_NX10_arm_boot_vector                , 0x00000000

@ =====================================================================
@
@ Area intram1
 .equ Addr_NX10_intram1_mirror_itcm, 0x00010000
 .equ Addr_NX10_intram1_mirror_dtcm, 0x04010000
 .equ Addr_NX10_intram1, 0x08010000
 .equ Addr_NX10_intram1_mirror_dpm, 0x10010000
 .equ Addr_NX10_intram1_mirror_hi, 0xFFF10000
@
@ =====================================================================
 .equ Adr_NX10_intram1_mirror_itcm_intram1_base, 0x00010000
 .equ Adr_NX10_intram1_mirror_dtcm_intram1_base, 0x04010000
 .equ Adr_NX10_intram1_intram1_base            , 0x08010000
 .equ Adr_NX10_intram1_mirror_dpm_intram1_base , 0x10010000
 .equ Adr_NX10_intram1_mirror_hi_intram1_base  , 0xFFF10000
 .equ Adr_NX10_intram1_mirror_itcm_intram1_end , 0x0001FFFC
 .equ Adr_NX10_intram1_mirror_dtcm_intram1_end , 0x0401FFFC
 .equ Adr_NX10_intram1_intram1_end             , 0x0801FFFC
 .equ Adr_NX10_intram1_mirror_dpm_intram1_end  , 0x1001FFFC
 .equ Adr_NX10_intram1_mirror_hi_intram1_end   , 0xFFF1FFFC

@ =====================================================================
@
@ Area intram2
 .equ Addr_NX10_intram2_mirror_itcm, 0x00020000
 .equ Addr_NX10_intram2_mirror_dtcm, 0x04020000
 .equ Addr_NX10_intram2, 0x08020000
 .equ Addr_NX10_intram2_mirror_dpm, 0x10020000
 .equ Addr_NX10_intram2_mirror_hi, 0xFFF20000
@
@ =====================================================================
 .equ Adr_NX10_intram2_mirror_itcm_intram2_base, 0x00020000
 .equ Adr_NX10_intram2_mirror_dtcm_intram2_base, 0x04020000
 .equ Adr_NX10_intram2_intram2_base            , 0x08020000
 .equ Adr_NX10_intram2_mirror_dpm_intram2_base , 0x10020000
 .equ Adr_NX10_intram2_mirror_hi_intram2_base  , 0xFFF20000
 .equ Adr_NX10_intram2_mirror_itcm_intram2_end , 0x0002FFFC
 .equ Adr_NX10_intram2_mirror_dtcm_intram2_end , 0x0402FFFC
 .equ Adr_NX10_intram2_intram2_end             , 0x0802FFFC
 .equ Adr_NX10_intram2_mirror_dpm_intram2_end  , 0x1002FFFC
 .equ Adr_NX10_intram2_mirror_hi_intram2_end   , 0xFFF2FFFC

@ =====================================================================
@
@ Area intram3
 .equ Addr_NX10_intram3_mirror_itcm, 0x00030000
 .equ Addr_NX10_intram3_mirror_dtcm, 0x04030000
 .equ Addr_NX10_intram3, 0x08030000
 .equ Addr_NX10_intram3_mirror_dpm, 0x10030000
 .equ Addr_NX10_intram3_mirror_hi, 0xFFF30000
@
@ =====================================================================
 .equ Adr_NX10_intram3_mirror_itcm_intram3_base, 0x00030000
 .equ Adr_NX10_intram3_mirror_dtcm_intram3_base, 0x04030000
 .equ Adr_NX10_intram3_intram3_base            , 0x08030000
 .equ Adr_NX10_intram3_mirror_dpm_intram3_base , 0x10030000
 .equ Adr_NX10_intram3_mirror_hi_intram3_base  , 0xFFF30000
 .equ Adr_NX10_intram3_mirror_itcm_intram3_end , 0x0003FFFC
 .equ Adr_NX10_intram3_mirror_dtcm_intram3_end , 0x0403FFFC
 .equ Adr_NX10_intram3_intram3_end             , 0x0803FFFC
 .equ Adr_NX10_intram3_mirror_dpm_intram3_end  , 0x1003FFFC
 .equ Adr_NX10_intram3_mirror_hi_intram3_end   , 0xFFF3FFFC

@ =====================================================================
@
@ Area intram4
 .equ Addr_NX10_intram4_mirror_itcm, 0x00040000
 .equ Addr_NX10_intram4_mirror_dtcm, 0x04040000
 .equ Addr_NX10_intram4, 0x08040000
 .equ Addr_NX10_intram4_mirror_dpm, 0x10040000
 .equ Addr_NX10_intram4_mirror_hi, 0xFFF40000
@
@ =====================================================================
 .equ Adr_NX10_intram4_mirror_itcm_intram4_base, 0x00040000
 .equ Adr_NX10_intram4_mirror_dtcm_intram4_base, 0x04040000
 .equ Adr_NX10_intram4_intram4_base            , 0x08040000
 .equ Adr_NX10_intram4_mirror_dpm_intram4_base , 0x10040000
 .equ Adr_NX10_intram4_mirror_hi_intram4_base  , 0xFFF40000
 .equ Adr_NX10_intram4_mirror_itcm_intram4_end , 0x00047FFC
 .equ Adr_NX10_intram4_mirror_dtcm_intram4_end , 0x04047FFC
 .equ Adr_NX10_intram4_intram4_end             , 0x08047FFC
 .equ Adr_NX10_intram4_mirror_dpm_intram4_end  , 0x10047FFC
 .equ Adr_NX10_intram4_mirror_hi_intram4_end   , 0xFFF47FFC

@ =====================================================================
@
@ Area handshake
 .equ Addr_NX10_handshake_mirror_itcm, 0x00048000
 .equ Addr_NX10_handshake_mirror_dtcm, 0x04048000
 .equ Addr_NX10_handshake, 0x08048000
 .equ Addr_NX10_handshake_mirror_dpm, 0x10048000
 .equ Addr_NX10_handshake_mirror_hi, 0xFFF48000
@
@ =====================================================================
 .equ Adr_NX10_handshake_mirror_itcm_handshake_base, 0x00048000
 .equ Adr_NX10_handshake_mirror_dtcm_handshake_base, 0x04048000
 .equ Adr_NX10_handshake_handshake_base            , 0x08048000
 .equ Adr_NX10_handshake_mirror_dpm_handshake_base , 0x10048000
 .equ Adr_NX10_handshake_mirror_hi_handshake_base  , 0xFFF48000
 .equ Adr_NX10_handshake_mirror_itcm_handshakeend  , 0x0004FFFC
 .equ Adr_NX10_handshake_mirror_dtcm_handshakeend  , 0x0404FFFC
 .equ Adr_NX10_handshake_handshakeend              , 0x0804FFFC
 .equ Adr_NX10_handshake_mirror_dpm_handshakeend   , 0x1004FFFC
 .equ Adr_NX10_handshake_mirror_hi_handshakeend    , 0xFFF4FFFC

@ =====================================================================
@
@ Area intram5
 .equ Addr_NX10_intram5_mirror_itcm, 0x00048000
 .equ Addr_NX10_intram5_mirror_itcm_dpm_mirror, 0x0004A000
 .equ Addr_NX10_intram5_mirror_itcm_arm_mirror, 0x0004C000
 .equ Addr_NX10_intram5_mirror_itcm_xpic_mirror, 0x0004E000
 .equ Addr_NX10_intram5_mirror_dtcm, 0x04048000
 .equ Addr_NX10_intram5_mirror_dtcm_dpm_mirror, 0x0404A000
 .equ Addr_NX10_intram5_mirror_dtcm_arm_mirror, 0x0404C000
 .equ Addr_NX10_intram5_mirror_dtcm_xpic_mirror, 0x0404E000
 .equ Addr_NX10_intram5, 0x08048000
 .equ Addr_NX10_intram5_dpm_mirror, 0x0804A000
 .equ Addr_NX10_intram5_arm_mirror, 0x0804C000
 .equ Addr_NX10_intram5_xpic_mirror, 0x0804E000
 .equ Addr_NX10_intram5_mirror_dpm, 0x10048000
 .equ Addr_NX10_intram5_mirror_dpm_dpm_mirror, 0x1004A000
 .equ Addr_NX10_intram5_mirror_dpm_arm_mirror, 0x1004C000
 .equ Addr_NX10_intram5_mirror_dpm_xpic_mirror, 0x1004E000
 .equ Addr_NX10_intram5_mirror_hi, 0xFFF48000
 .equ Addr_NX10_intram5_mirror_hi_dpm_mirror, 0xFFF4A000
 .equ Addr_NX10_intram5_mirror_hi_arm_mirror, 0xFFF4C000
 .equ Addr_NX10_intram5_mirror_hi_xpic_mirror, 0xFFF4E000
@
@ =====================================================================
 .equ Adr_NX10_intram5_mirror_itcm_intram5_base            , 0x00048000
 .equ Adr_NX10_intram5_mirror_itcm_dpm_mirror_intram5_base , 0x0004A000
 .equ Adr_NX10_intram5_mirror_itcm_arm_mirror_intram5_base , 0x0004C000
 .equ Adr_NX10_intram5_mirror_itcm_xpic_mirror_intram5_base, 0x0004E000
 .equ Adr_NX10_intram5_mirror_dtcm_intram5_base            , 0x04048000
 .equ Adr_NX10_intram5_mirror_dtcm_dpm_mirror_intram5_base , 0x0404A000
 .equ Adr_NX10_intram5_mirror_dtcm_arm_mirror_intram5_base , 0x0404C000
 .equ Adr_NX10_intram5_mirror_dtcm_xpic_mirror_intram5_base, 0x0404E000
 .equ Adr_NX10_intram5_intram5_base                        , 0x08048000
 .equ Adr_NX10_intram5_dpm_mirror_intram5_base             , 0x0804A000
 .equ Adr_NX10_intram5_arm_mirror_intram5_base             , 0x0804C000
 .equ Adr_NX10_intram5_xpic_mirror_intram5_base            , 0x0804E000
 .equ Adr_NX10_intram5_mirror_dpm_intram5_base             , 0x10048000
 .equ Adr_NX10_intram5_mirror_dpm_dpm_mirror_intram5_base  , 0x1004A000
 .equ Adr_NX10_intram5_mirror_dpm_arm_mirror_intram5_base  , 0x1004C000
 .equ Adr_NX10_intram5_mirror_dpm_xpic_mirror_intram5_base , 0x1004E000
 .equ Adr_NX10_intram5_mirror_hi_intram5_base              , 0xFFF48000
 .equ Adr_NX10_intram5_mirror_hi_dpm_mirror_intram5_base   , 0xFFF4A000
 .equ Adr_NX10_intram5_mirror_hi_arm_mirror_intram5_base   , 0xFFF4C000
 .equ Adr_NX10_intram5_mirror_hi_xpic_mirror_intram5_base  , 0xFFF4E000
 .equ Adr_NX10_intram5_mirror_itcm_intram5_end             , 0x00049FFC
 .equ Adr_NX10_intram5_mirror_itcm_dpm_mirror_intram5_end  , 0x0004BFFC
 .equ Adr_NX10_intram5_mirror_itcm_arm_mirror_intram5_end  , 0x0004DFFC
 .equ Adr_NX10_intram5_mirror_itcm_xpic_mirror_intram5_end , 0x0004FFFC
 .equ Adr_NX10_intram5_mirror_dtcm_intram5_end             , 0x04049FFC
 .equ Adr_NX10_intram5_mirror_dtcm_dpm_mirror_intram5_end  , 0x0404BFFC
 .equ Adr_NX10_intram5_mirror_dtcm_arm_mirror_intram5_end  , 0x0404DFFC
 .equ Adr_NX10_intram5_mirror_dtcm_xpic_mirror_intram5_end , 0x0404FFFC
 .equ Adr_NX10_intram5_intram5_end                         , 0x08049FFC
 .equ Adr_NX10_intram5_dpm_mirror_intram5_end              , 0x0804BFFC
 .equ Adr_NX10_intram5_arm_mirror_intram5_end              , 0x0804DFFC
 .equ Adr_NX10_intram5_xpic_mirror_intram5_end             , 0x0804FFFC
 .equ Adr_NX10_intram5_mirror_dpm_intram5_end              , 0x10049FFC
 .equ Adr_NX10_intram5_mirror_dpm_dpm_mirror_intram5_end   , 0x1004BFFC
 .equ Adr_NX10_intram5_mirror_dpm_arm_mirror_intram5_end   , 0x1004DFFC
 .equ Adr_NX10_intram5_mirror_dpm_xpic_mirror_intram5_end  , 0x1004FFFC
 .equ Adr_NX10_intram5_mirror_hi_intram5_end               , 0xFFF49FFC
 .equ Adr_NX10_intram5_mirror_hi_dpm_mirror_intram5_end    , 0xFFF4BFFC
 .equ Adr_NX10_intram5_mirror_hi_arm_mirror_intram5_end    , 0xFFF4DFFC
 .equ Adr_NX10_intram5_mirror_hi_xpic_mirror_intram5_end   , 0xFFF4FFFC

@ =====================================================================
@
@ Area rom
 .equ Addr_NX10_rom_mirror_itcm, 0x00070000
 .equ Addr_NX10_rom_mirror_dtcm, 0x04070000
 .equ Addr_NX10_rom, 0x08070000
 .equ Addr_NX10_rom_mirror_dpm, 0x10070000
 .equ Addr_NX10_rom_mirror_hi, 0xFFF70000
@
@ =====================================================================
 .equ Adr_NX10_rom_mirror_itcm_rom_base, 0x00070000
 .equ Adr_NX10_rom_mirror_dtcm_rom_base, 0x04070000
 .equ Adr_NX10_rom_rom_base            , 0x08070000
 .equ Adr_NX10_rom_mirror_dpm_rom_base , 0x10070000
 .equ Adr_NX10_rom_mirror_hi_rom_base  , 0xFFF70000
 .equ Adr_NX10_rom_mirror_itcm_rom_end , 0x0007FFFC
 .equ Adr_NX10_rom_mirror_dtcm_rom_end , 0x0407FFFC
 .equ Adr_NX10_rom_rom_end             , 0x0807FFFC
 .equ Adr_NX10_rom_mirror_dpm_rom_end  , 0x1007FFFC
 .equ Adr_NX10_rom_mirror_hi_rom_end   , 0xFFF7FFFC

@ =====================================================================
@
@ Area ARMdTCM_memarea
 .equ Addr_NX10_ARMdTCM_memarea, 0x04000000
@
@ =====================================================================
@ =====================================================================
@
@ Area intram0
 .equ Addr_NX10_intram0_mirror_dtcm, 0x04000000
 .equ Addr_NX10_intram0, 0x08000000
 .equ Addr_NX10_intram0_mirror_dpm, 0x10000000
 .equ Addr_NX10_intram0_mirror_hi, 0xFFF00000
@
@ =====================================================================
 .equ Adr_NX10_intram0_mirror_dtcm_intram0_base, 0x04000000
 .equ Adr_NX10_intram0_intram0_base            , 0x08000000
 .equ Adr_NX10_intram0_mirror_dpm_intram0_base , 0x10000000
 .equ Adr_NX10_intram0_mirror_hi_intram0_base  , 0xFFF00000
 .equ Adr_NX10_intram0_mirror_dtcm_intram0_end , 0x0400FFFC
 .equ Adr_NX10_intram0_intram0_end             , 0x0800FFFC
 .equ Adr_NX10_intram0_mirror_dpm_intram0_end  , 0x1000FFFC
 .equ Adr_NX10_intram0_mirror_hi_intram0_end   , 0xFFF0FFFC

@ =====================================================================
@
@ Area ARM_buf_area
 .equ Addr_NX10_ARM_buf_area, 0x08000000
@
@ =====================================================================
@ =====================================================================
@
@ Area intlogic_motion_mirror_buf
 .equ Addr_NX10_intlogic_motion_mirror_buf, 0x08100000
@
@ =====================================================================
@ =====================================================================
@
@ Area intlogic_sys_mirror_buf
 .equ Addr_NX10_intlogic_sys_mirror_buf, 0x08180000
@
@ =====================================================================
@ =====================================================================
@
@ Area sqirom
 .equ Addr_NX10_sqirom, 0x09000000
@
@ =====================================================================
@ =====================================================================
@
@ Area ARM_nbuf_area
 .equ Addr_NX10_ARM_nbuf_area, 0x10000000
@
@ =====================================================================
@ =====================================================================
@
@ Area dpm_full_window
 .equ Addr_NX10_dpm_full_window, 0x10000000
@
@ =====================================================================
@ =====================================================================
@
@ Area intlogic_motion
 .equ Addr_NX10_intlogic_motion, 0x10100000
@
@ =====================================================================
@ =====================================================================
@
@ Area sr
 .equ Addr_NX10_sr_motion, 0x10125640
@
@ =====================================================================
 .equ Adr_NX10_sr_motion_sr_sr0           , 0x10125640
 .equ Adr_NX10_sr_sr0                     , 0x10125640
 .equ Adr_NX10_sr_motion_sr_sr1           , 0x10125644
 .equ Adr_NX10_sr_sr1                     , 0x10125644
 .equ Adr_NX10_sr_motion_sr_sr2           , 0x10125648
 .equ Adr_NX10_sr_sr2                     , 0x10125648
 .equ Adr_NX10_sr_motion_sr_sr3           , 0x1012564C
 .equ Adr_NX10_sr_sr3                     , 0x1012564C
 .equ Adr_NX10_sr_motion_sr_sr4           , 0x10125650
 .equ Adr_NX10_sr_sr4                     , 0x10125650
 .equ Adr_NX10_sr_motion_sr_sr5           , 0x10125654
 .equ Adr_NX10_sr_sr5                     , 0x10125654
 .equ Adr_NX10_sr_motion_sr_config_shared0, 0x10125658
 .equ Adr_NX10_sr_config_shared0          , 0x10125658

@ =====================================================================
@
@ Area cordic
 .equ Addr_NX10_cordic, 0x10140000
@
@ =====================================================================
 .equ Adr_NX10_cordic_cordic_ctrl                        , 0x10140000
 .equ Adr_NX10_cordic_ctrl                               , 0x10140000
 .equ Adr_NX10_cordic_cordic_x_reg                       , 0x10140004
 .equ Adr_NX10_cordic_x_reg                              , 0x10140004
 .equ Adr_NX10_cordic_cordic_y_reg                       , 0x10140008
 .equ Adr_NX10_cordic_y_reg                              , 0x10140008
 .equ Adr_NX10_cordic_cordic_z_reg                       , 0x1014000C
 .equ Adr_NX10_cordic_z_reg                              , 0x1014000C
 .equ Adr_NX10_cordic_cordic_c_reg                       , 0x10140010
 .equ Adr_NX10_cordic_c_reg                              , 0x10140010
 .equ Adr_NX10_cordic_cordic_fsm_state                   , 0x10140014
 .equ Adr_NX10_cordic_fsm_state                          , 0x10140014
 .equ Adr_NX10_cordic_cordic_lin_39_to_8                 , 0x10140018
 .equ Adr_NX10_cordic_lin_39_to_8                        , 0x10140018
 .equ Adr_NX10_cordic_cordic_lin_7_to_0                  , 0x1014001C
 .equ Adr_NX10_cordic_lin_7_to_0                         , 0x1014001C
 .equ Adr_NX10_cordic_cordic_coeff_ram_start_circ_39_to_8, 0x10140100
 .equ Adr_NX10_cordic_coeff_ram_start_circ_39_to_8       , 0x10140100
 .equ Adr_NX10_cordic_cordic_coeff_ram_end_circ_39_to_8  , 0x1014019C
 .equ Adr_NX10_cordic_coeff_ram_end_circ_39_to_8         , 0x1014019C
 .equ Adr_NX10_cordic_cordic_coeff_ram_start_hyp_39_to_8 , 0x10140200
 .equ Adr_NX10_cordic_coeff_ram_start_hyp_39_to_8        , 0x10140200
 .equ Adr_NX10_cordic_cordic_coeff_ram_end_hyp_39_to_8   , 0x1014029C
 .equ Adr_NX10_cordic_coeff_ram_end_hyp_39_to_8          , 0x1014029C
 .equ Adr_NX10_cordic_cordic_coeff_ram_start_circ_7_to_0 , 0x10140300
 .equ Adr_NX10_cordic_coeff_ram_start_circ_7_to_0        , 0x10140300
 .equ Adr_NX10_cordic_cordic_coeff_ram_end_circ_7_to_0   , 0x1014034C
 .equ Adr_NX10_cordic_coeff_ram_end_circ_7_to_0          , 0x1014034C
 .equ Adr_NX10_cordic_cordic_coeff_ram_start_hyp_7_to_0  , 0x10140350
 .equ Adr_NX10_cordic_coeff_ram_start_hyp_7_to_0         , 0x10140350
 .equ Adr_NX10_cordic_cordic_coeff_ram_end_hyp_7_to_0    , 0x1014039C
 .equ Adr_NX10_cordic_coeff_ram_end_hyp_7_to_0           , 0x1014039C

@ =====================================================================
@
@ Area mpwm
 .equ Addr_NX10_mpwm, 0x10140500
@
@ =====================================================================
 .equ Adr_NX10_mpwm_mpwm_config_counter, 0x10140500
 .equ Adr_NX10_mpwm_config_counter     , 0x10140500
 .equ Adr_NX10_mpwm_mpwm_config_pins   , 0x10140504
 .equ Adr_NX10_mpwm_config_pins        , 0x10140504
 .equ Adr_NX10_mpwm_mpwm_config_failure, 0x10140508
 .equ Adr_NX10_mpwm_config_failure     , 0x10140508
 .equ Adr_NX10_mpwm_mpwm_irq_config    , 0x1014050C
 .equ Adr_NX10_mpwm_irq_config         , 0x1014050C
 .equ Adr_NX10_mpwm_mpwm_irq_raw       , 0x10140510
 .equ Adr_NX10_mpwm_irq_raw            , 0x10140510
 .equ Adr_NX10_mpwm_mpwm_irq_masked    , 0x10140514
 .equ Adr_NX10_mpwm_irq_masked         , 0x10140514
 .equ Adr_NX10_mpwm_mpwm_irq_msk_set   , 0x10140518
 .equ Adr_NX10_mpwm_irq_msk_set        , 0x10140518
 .equ Adr_NX10_mpwm_mpwm_irq_msk_reset , 0x1014051C
 .equ Adr_NX10_mpwm_irq_msk_reset      , 0x1014051C
 .equ Adr_NX10_mpwm_mpwm_cnt0_period   , 0x10140520
 .equ Adr_NX10_mpwm_cnt0_period        , 0x10140520
 .equ Adr_NX10_mpwm_mpwm_cnt0          , 0x10140524
 .equ Adr_NX10_mpwm_cnt0               , 0x10140524
 .equ Adr_NX10_mpwm_mpwm_cnt0_systime  , 0x10140528
 .equ Adr_NX10_mpwm_cnt0_systime       , 0x10140528
 .equ Adr_NX10_mpwm_mpwm_cnt0_watchdog , 0x1014052C
 .equ Adr_NX10_mpwm_cnt0_watchdog      , 0x1014052C
 .equ Adr_NX10_mpwm_mpwm_cnt1_period   , 0x10140530
 .equ Adr_NX10_mpwm_cnt1_period        , 0x10140530
 .equ Adr_NX10_mpwm_mpwm_cnt1          , 0x10140534
 .equ Adr_NX10_mpwm_cnt1               , 0x10140534
 .equ Adr_NX10_mpwm_mpwm_cnt1_systime  , 0x10140538
 .equ Adr_NX10_mpwm_cnt1_systime       , 0x10140538
 .equ Adr_NX10_mpwm_mpwm_cnt1_watchdog , 0x1014053C
 .equ Adr_NX10_mpwm_cnt1_watchdog      , 0x1014053C
 .equ Adr_NX10_mpwm_mpwm_t0            , 0x10140540
 .equ Adr_NX10_mpwm_t0                 , 0x10140540
 .equ Adr_NX10_mpwm_mpwm_t1            , 0x10140544
 .equ Adr_NX10_mpwm_t1                 , 0x10140544
 .equ Adr_NX10_mpwm_mpwm_t2            , 0x10140548
 .equ Adr_NX10_mpwm_t2                 , 0x10140548
 .equ Adr_NX10_mpwm_mpwm_t3            , 0x1014054C
 .equ Adr_NX10_mpwm_t3                 , 0x1014054C
 .equ Adr_NX10_mpwm_mpwm_t4            , 0x10140550
 .equ Adr_NX10_mpwm_t4                 , 0x10140550
 .equ Adr_NX10_mpwm_mpwm_t5            , 0x10140554
 .equ Adr_NX10_mpwm_t5                 , 0x10140554
 .equ Adr_NX10_mpwm_mpwm_t6            , 0x10140558
 .equ Adr_NX10_mpwm_t6                 , 0x10140558
 .equ Adr_NX10_mpwm_mpwm_t7            , 0x1014055C
 .equ Adr_NX10_mpwm_t7                 , 0x1014055C
 .equ Adr_NX10_mpwm_mpwm_t0_shadow     , 0x10140560
 .equ Adr_NX10_mpwm_t0_shadow          , 0x10140560
 .equ Adr_NX10_mpwm_mpwm_t1_shadow     , 0x10140564
 .equ Adr_NX10_mpwm_t1_shadow          , 0x10140564
 .equ Adr_NX10_mpwm_mpwm_t2_shadow     , 0x10140568
 .equ Adr_NX10_mpwm_t2_shadow          , 0x10140568
 .equ Adr_NX10_mpwm_mpwm_t3_shadow     , 0x1014056C
 .equ Adr_NX10_mpwm_t3_shadow          , 0x1014056C
 .equ Adr_NX10_mpwm_mpwm_t4_shadow     , 0x10140570
 .equ Adr_NX10_mpwm_t4_shadow          , 0x10140570
 .equ Adr_NX10_mpwm_mpwm_t5_shadow     , 0x10140574
 .equ Adr_NX10_mpwm_t5_shadow          , 0x10140574
 .equ Adr_NX10_mpwm_mpwm_t6_shadow     , 0x10140578
 .equ Adr_NX10_mpwm_t6_shadow          , 0x10140578
 .equ Adr_NX10_mpwm_mpwm_t7_shadow     , 0x1014057C
 .equ Adr_NX10_mpwm_t7_shadow          , 0x1014057C

@ =====================================================================
@
@ Area menc
 .equ Addr_NX10_menc, 0x10140580
@
@ =====================================================================
 .equ Adr_NX10_menc_menc_config         , 0x10140580
 .equ Adr_NX10_menc_config              , 0x10140580
 .equ Adr_NX10_menc_menc_enc0_position  , 0x10140584
 .equ Adr_NX10_menc_enc0_position       , 0x10140584
 .equ Adr_NX10_menc_menc_enc1_position  , 0x10140588
 .equ Adr_NX10_menc_enc1_position       , 0x10140588
 .equ Adr_NX10_menc_menc_capture_now    , 0x1014058C
 .equ Adr_NX10_menc_capture_now         , 0x1014058C
 .equ Adr_NX10_menc_menc_capture0_config, 0x10140590
 .equ Adr_NX10_menc_capture0_config     , 0x10140590
 .equ Adr_NX10_menc_menc_capture0_val   , 0x10140594
 .equ Adr_NX10_menc_capture0_val        , 0x10140594
 .equ Adr_NX10_menc_menc_capture0_ta    , 0x10140598
 .equ Adr_NX10_menc_capture0_ta         , 0x10140598
 .equ Adr_NX10_menc_menc_capture0_te    , 0x1014059C
 .equ Adr_NX10_menc_capture0_te         , 0x1014059C
 .equ Adr_NX10_menc_menc_capture1_config, 0x101405A0
 .equ Adr_NX10_menc_capture1_config     , 0x101405A0
 .equ Adr_NX10_menc_menc_capture1_val   , 0x101405A4
 .equ Adr_NX10_menc_capture1_val        , 0x101405A4
 .equ Adr_NX10_menc_menc_capture1_ta    , 0x101405A8
 .equ Adr_NX10_menc_capture1_ta         , 0x101405A8
 .equ Adr_NX10_menc_menc_capture1_te    , 0x101405AC
 .equ Adr_NX10_menc_capture1_te         , 0x101405AC
 .equ Adr_NX10_menc_menc_capture2_config, 0x101405B0
 .equ Adr_NX10_menc_capture2_config     , 0x101405B0
 .equ Adr_NX10_menc_menc_capture2_val   , 0x101405B4
 .equ Adr_NX10_menc_capture2_val        , 0x101405B4
 .equ Adr_NX10_menc_menc_capture2_ta    , 0x101405B8
 .equ Adr_NX10_menc_capture2_ta         , 0x101405B8
 .equ Adr_NX10_menc_menc_capture2_te    , 0x101405BC
 .equ Adr_NX10_menc_capture2_te         , 0x101405BC
 .equ Adr_NX10_menc_menc_capture3_config, 0x101405C0
 .equ Adr_NX10_menc_capture3_config     , 0x101405C0
 .equ Adr_NX10_menc_menc_capture3_val   , 0x101405C4
 .equ Adr_NX10_menc_capture3_val        , 0x101405C4
 .equ Adr_NX10_menc_menc_capture3_ta    , 0x101405C8
 .equ Adr_NX10_menc_capture3_ta         , 0x101405C8
 .equ Adr_NX10_menc_menc_capture3_te    , 0x101405CC
 .equ Adr_NX10_menc_capture3_te         , 0x101405CC
 .equ Adr_NX10_menc_menc_status         , 0x101405D0
 .equ Adr_NX10_menc_status              , 0x101405D0
 .equ Adr_NX10_menc_menc_irq_masked     , 0x101405D4
 .equ Adr_NX10_menc_irq_masked          , 0x101405D4
 .equ Adr_NX10_menc_menc_irq_msk_set    , 0x101405D8
 .equ Adr_NX10_menc_irq_msk_set         , 0x101405D8
 .equ Adr_NX10_menc_menc_irq_msk_reset  , 0x101405DC
 .equ Adr_NX10_menc_irq_msk_reset       , 0x101405DC

@ =====================================================================
@
@ Area io_link
 .equ Addr_NX10_io_link, 0x10140600
@
@ =====================================================================
@ =====================================================================
@
@ Area xlink
 .equ Addr_NX10_xlink0, 0x10140600
 .equ Addr_NX10_xlink1, 0x10140610
 .equ Addr_NX10_xlink2, 0x10140620
 .equ Addr_NX10_xlink3, 0x10140630
@
@ =====================================================================
 .equ Adr_NX10_xlink0_xlink_cfg , 0x10140600
 .equ Adr_NX10_xlink1_xlink_cfg , 0x10140610
 .equ Adr_NX10_xlink2_xlink_cfg , 0x10140620
 .equ Adr_NX10_xlink3_xlink_cfg , 0x10140630
 .equ Adr_NX10_xlink0_xlink_tx  , 0x10140604
 .equ Adr_NX10_xlink1_xlink_tx  , 0x10140614
 .equ Adr_NX10_xlink2_xlink_tx  , 0x10140624
 .equ Adr_NX10_xlink3_xlink_tx  , 0x10140634
 .equ Adr_NX10_xlink0_xlink_rx  , 0x10140608
 .equ Adr_NX10_xlink1_xlink_rx  , 0x10140618
 .equ Adr_NX10_xlink2_xlink_rx  , 0x10140628
 .equ Adr_NX10_xlink3_xlink_rx  , 0x10140638
 .equ Adr_NX10_xlink0_xlink_stat, 0x1014060C
 .equ Adr_NX10_xlink1_xlink_stat, 0x1014061C
 .equ Adr_NX10_xlink2_xlink_stat, 0x1014062C
 .equ Adr_NX10_xlink3_xlink_stat, 0x1014063C

@ =====================================================================
@
@ Area io_link_irq
 .equ Addr_NX10_io_link_irq, 0x10140640
@
@ =====================================================================
 .equ Adr_NX10_io_link_irq_io_link_irq_raw      , 0x10140640
 .equ Adr_NX10_io_link_irq_raw                  , 0x10140640
 .equ Adr_NX10_io_link_irq_io_link_irq_masked   , 0x10140644
 .equ Adr_NX10_io_link_irq_masked               , 0x10140644
 .equ Adr_NX10_io_link_irq_io_link_irq_msk_set  , 0x10140648
 .equ Adr_NX10_io_link_irq_msk_set              , 0x10140648
 .equ Adr_NX10_io_link_irq_io_link_irq_msk_reset, 0x1014064C
 .equ Adr_NX10_io_link_irq_msk_reset            , 0x1014064C
 .equ Adr_NX10_io_link_irq_io_link_irq_enable   , 0x10140650
 .equ Adr_NX10_io_link_irq_enable               , 0x10140650

@ =====================================================================
@
@ Area spi
 .equ Addr_NX10_spi_motion, 0x10140680
@
@ =====================================================================
 .equ Adr_NX10_spi_motion_spi_cr0                       , 0x10140680
 .equ Adr_NX10_spi_cr0                                  , 0x10140680
 .equ Adr_NX10_spi_motion_spi_cr1                       , 0x10140684
 .equ Adr_NX10_spi_cr1                                  , 0x10140684
 .equ Adr_NX10_spi_motion_spi_dr                        , 0x10140688
 .equ Adr_NX10_spi_dr                                   , 0x10140688
 .equ Adr_NX10_spi_motion_spi_sr                        , 0x1014068C
 .equ Adr_NX10_spi_sr                                   , 0x1014068C
 .equ Adr_NX10_spi_motion_spi_cpsr                      , 0x10140690
 .equ Adr_NX10_spi_cpsr                                 , 0x10140690
 .equ Adr_NX10_spi_motion_spi_imsc                      , 0x10140694
 .equ Adr_NX10_spi_imsc                                 , 0x10140694
 .equ Adr_NX10_spi_motion_spi_ris                       , 0x10140698
 .equ Adr_NX10_spi_ris                                  , 0x10140698
 .equ Adr_NX10_spi_motion_spi_mis                       , 0x1014069C
 .equ Adr_NX10_spi_mis                                  , 0x1014069C
 .equ Adr_NX10_spi_motion_spi_icr                       , 0x101406A0
 .equ Adr_NX10_spi_icr                                  , 0x101406A0
 .equ Adr_NX10_spi_motion_spi_irq_cpu_sel               , 0x101406A4
 .equ Adr_NX10_spi_irq_cpu_sel                          , 0x101406A4
 .equ Adr_NX10_spi_motion_spi_dmacr                     , 0x101406A8
 .equ Adr_NX10_spi_dmacr                                , 0x101406A8
 .equ Adr_NX10_spi_motion_spi_data_register             , 0x101406B0
 .equ Adr_NX10_spi_data_register                        , 0x101406B0
 .equ Adr_NX10_spi_motion_spi_status_register           , 0x101406B4
 .equ Adr_NX10_spi_status_register                      , 0x101406B4
 .equ Adr_NX10_spi_motion_spi_control_register          , 0x101406B8
 .equ Adr_NX10_spi_control_register                     , 0x101406B8
 .equ Adr_NX10_spi_motion_spi_interrupt_control_register, 0x101406BC
 .equ Adr_NX10_spi_interrupt_control_register           , 0x101406BC

@ =====================================================================
@
@ Area adc_ctrl
 .equ Addr_NX10_adc_ctrl, 0x101406C0
@
@ =====================================================================
 .equ Adr_NX10_adc_ctrl_adc_ctrl_start             , 0x101406C0
 .equ Adr_NX10_adc_ctrl_start                      , 0x101406C0
 .equ Adr_NX10_adc_ctrl_adc_ctrl_autosample_config0, 0x101406C4
 .equ Adr_NX10_adc_ctrl_autosample_config0         , 0x101406C4
 .equ Adr_NX10_adc_ctrl_adc_ctrl_autosample_config1, 0x101406C8
 .equ Adr_NX10_adc_ctrl_autosample_config1         , 0x101406C8
 .equ Adr_NX10_adc_ctrl_adc_ctrl_mansample_config0 , 0x101406CC
 .equ Adr_NX10_adc_ctrl_mansample_config0          , 0x101406CC
 .equ Adr_NX10_adc_ctrl_adc_ctrl_mansample_config1 , 0x101406D0
 .equ Adr_NX10_adc_ctrl_mansample_config1          , 0x101406D0
 .equ Adr_NX10_adc_ctrl_adc_ctrl_status            , 0x101406D4
 .equ Adr_NX10_adc_ctrl_status                     , 0x101406D4
 .equ Adr_NX10_adc_ctrl_adc_ctrl_adc0_val          , 0x101406D8
 .equ Adr_NX10_adc_ctrl_adc0_val                   , 0x101406D8
 .equ Adr_NX10_adc_ctrl_adc_ctrl_adc1_val          , 0x101406DC
 .equ Adr_NX10_adc_ctrl_adc1_val                   , 0x101406DC
 .equ Adr_NX10_adc_ctrl_adc_ctrl_irq_raw           , 0x101406E0
 .equ Adr_NX10_adc_ctrl_irq_raw                    , 0x101406E0
 .equ Adr_NX10_adc_ctrl_adc_ctrl_irq_masked        , 0x101406E4
 .equ Adr_NX10_adc_ctrl_irq_masked                 , 0x101406E4
 .equ Adr_NX10_adc_ctrl_adc_ctrl_irq_msk_set       , 0x101406E8
 .equ Adr_NX10_adc_ctrl_irq_msk_set                , 0x101406E8
 .equ Adr_NX10_adc_ctrl_adc_ctrl_irq_msk_reset     , 0x101406EC
 .equ Adr_NX10_adc_ctrl_irq_msk_reset              , 0x101406EC

@ =====================================================================
@
@ Area xpic_timer
 .equ Addr_NX10_xpic_timer, 0x10140700
@
@ =====================================================================
 .equ Adr_NX10_xpic_timer_xpic_timer_config_timer0          , 0x10140700
 .equ Adr_NX10_xpic_timer_config_timer0                     , 0x10140700
 .equ Adr_NX10_xpic_timer_xpic_timer_config_timer1          , 0x10140704
 .equ Adr_NX10_xpic_timer_config_timer1                     , 0x10140704
 .equ Adr_NX10_xpic_timer_xpic_timer_config_timer2          , 0x10140708
 .equ Adr_NX10_xpic_timer_config_timer2                     , 0x10140708
 .equ Adr_NX10_xpic_timer_xpic_timer_preload_timer0         , 0x1014070C
 .equ Adr_NX10_xpic_timer_preload_timer0                    , 0x1014070C
 .equ Adr_NX10_xpic_timer_xpic_timer_preload_timer1         , 0x10140710
 .equ Adr_NX10_xpic_timer_preload_timer1                    , 0x10140710
 .equ Adr_NX10_xpic_timer_xpic_timer_preload_timer2         , 0x10140714
 .equ Adr_NX10_xpic_timer_preload_timer2                    , 0x10140714
 .equ Adr_NX10_xpic_timer_xpic_timer_timer0                 , 0x10140718
 .equ Adr_NX10_xpic_timer_timer0                            , 0x10140718
 .equ Adr_NX10_xpic_timer_xpic_timer_timer1                 , 0x1014071C
 .equ Adr_NX10_xpic_timer_timer1                            , 0x1014071C
 .equ Adr_NX10_xpic_timer_xpic_timer_timer2                 , 0x10140720
 .equ Adr_NX10_xpic_timer_timer2                            , 0x10140720
 .equ Adr_NX10_xpic_timer_xpic_timer_irq_raw                , 0x10140724
 .equ Adr_NX10_xpic_timer_irq_raw                           , 0x10140724
 .equ Adr_NX10_xpic_timer_xpic_timer_irq_masked             , 0x10140728
 .equ Adr_NX10_xpic_timer_irq_masked                        , 0x10140728
 .equ Adr_NX10_xpic_timer_xpic_timer_irq_msk_set            , 0x1014072C
 .equ Adr_NX10_xpic_timer_irq_msk_set                       , 0x1014072C
 .equ Adr_NX10_xpic_timer_xpic_timer_irq_msk_reset          , 0x10140730
 .equ Adr_NX10_xpic_timer_irq_msk_reset                     , 0x10140730
 .equ Adr_NX10_xpic_timer_xpic_timer_systime_s              , 0x10140734
 .equ Adr_NX10_xpic_timer_systime_s                         , 0x10140734
 .equ Adr_NX10_xpic_timer_xpic_timer_systime_ns             , 0x10140738
 .equ Adr_NX10_xpic_timer_systime_ns                        , 0x10140738
 .equ Adr_NX10_xpic_timer_xpic_timer_compare_systime_s_value, 0x1014073C
 .equ Adr_NX10_xpic_timer_compare_systime_s_value           , 0x1014073C

@ =====================================================================
@
@ Area xpic_vic
 .equ Addr_NX10_xpic_vic, 0x10140800
@
@ =====================================================================
 .equ Adr_NX10_xpic_vic_xpic_vic_config         , 0x10140800
 .equ Adr_NX10_xpic_vic_config                  , 0x10140800
 .equ Adr_NX10_xpic_vic_xpic_vic_raw_intr0      , 0x10140804
 .equ Adr_NX10_xpic_vic_raw_intr0               , 0x10140804
 .equ Adr_NX10_xpic_vic_xpic_vic_raw_intr1      , 0x10140808
 .equ Adr_NX10_xpic_vic_raw_intr1               , 0x10140808
 .equ Adr_NX10_xpic_vic_xpic_vic_softint0_set   , 0x1014080C
 .equ Adr_NX10_xpic_vic_softint0_set            , 0x1014080C
 .equ Adr_NX10_xpic_vic_xpic_vic_softint1_set   , 0x10140810
 .equ Adr_NX10_xpic_vic_softint1_set            , 0x10140810
 .equ Adr_NX10_xpic_vic_xpic_vic_softint0_reset , 0x10140814
 .equ Adr_NX10_xpic_vic_softint0_reset          , 0x10140814
 .equ Adr_NX10_xpic_vic_xpic_vic_softint1_reset , 0x10140818
 .equ Adr_NX10_xpic_vic_softint1_reset          , 0x10140818
 .equ Adr_NX10_xpic_vic_xpic_vic_fiq_addr       , 0x1014081C
 .equ Adr_NX10_xpic_vic_fiq_addr                , 0x1014081C
 .equ Adr_NX10_xpic_vic_xpic_vic_irq_addr       , 0x10140820
 .equ Adr_NX10_xpic_vic_irq_addr                , 0x10140820
 .equ Adr_NX10_xpic_vic_xpic_vic_vector_addr    , 0x10140824
 .equ Adr_NX10_xpic_vic_vector_addr             , 0x10140824
 .equ Adr_NX10_xpic_vic_xpic_vic_table_base_addr, 0x10140828
 .equ Adr_NX10_xpic_vic_table_base_addr         , 0x10140828
 .equ Adr_NX10_xpic_vic_xpic_vic_fiq_vect_config, 0x1014082C
 .equ Adr_NX10_xpic_vic_fiq_vect_config         , 0x1014082C
 .equ Adr_NX10_xpic_vic_xpic_vic_vect_config0   , 0x10140830
 .equ Adr_NX10_xpic_vic_vect_config0            , 0x10140830
 .equ Adr_NX10_xpic_vic_xpic_vic_vect_config1   , 0x10140834
 .equ Adr_NX10_xpic_vic_vect_config1            , 0x10140834
 .equ Adr_NX10_xpic_vic_xpic_vic_vect_config2   , 0x10140838
 .equ Adr_NX10_xpic_vic_vect_config2            , 0x10140838
 .equ Adr_NX10_xpic_vic_xpic_vic_vect_config3   , 0x1014083C
 .equ Adr_NX10_xpic_vic_vect_config3            , 0x1014083C
 .equ Adr_NX10_xpic_vic_xpic_vic_vect_config4   , 0x10140840
 .equ Adr_NX10_xpic_vic_vect_config4            , 0x10140840
 .equ Adr_NX10_xpic_vic_xpic_vic_vect_config5   , 0x10140844
 .equ Adr_NX10_xpic_vic_vect_config5            , 0x10140844
 .equ Adr_NX10_xpic_vic_xpic_vic_vect_config6   , 0x10140848
 .equ Adr_NX10_xpic_vic_vect_config6            , 0x10140848
 .equ Adr_NX10_xpic_vic_xpic_vic_vect_config7   , 0x1014084C
 .equ Adr_NX10_xpic_vic_vect_config7            , 0x1014084C
 .equ Adr_NX10_xpic_vic_xpic_vic_vect_config8   , 0x10140850
 .equ Adr_NX10_xpic_vic_vect_config8            , 0x10140850
 .equ Adr_NX10_xpic_vic_xpic_vic_vect_config9   , 0x10140854
 .equ Adr_NX10_xpic_vic_vect_config9            , 0x10140854
 .equ Adr_NX10_xpic_vic_xpic_vic_vect_config10  , 0x10140858
 .equ Adr_NX10_xpic_vic_vect_config10           , 0x10140858
 .equ Adr_NX10_xpic_vic_xpic_vic_vect_config11  , 0x1014085C
 .equ Adr_NX10_xpic_vic_vect_config11           , 0x1014085C
 .equ Adr_NX10_xpic_vic_xpic_vic_vect_config12  , 0x10140860
 .equ Adr_NX10_xpic_vic_vect_config12           , 0x10140860
 .equ Adr_NX10_xpic_vic_xpic_vic_vect_config13  , 0x10140864
 .equ Adr_NX10_xpic_vic_vect_config13           , 0x10140864
 .equ Adr_NX10_xpic_vic_xpic_vic_vect_config14  , 0x10140868
 .equ Adr_NX10_xpic_vic_vect_config14           , 0x10140868
 .equ Adr_NX10_xpic_vic_xpic_vic_vect_config15  , 0x1014086C
 .equ Adr_NX10_xpic_vic_vect_config15           , 0x1014086C
 .equ Adr_NX10_xpic_vic_xpic_vic_default0       , 0x10140870
 .equ Adr_NX10_xpic_vic_default0                , 0x10140870
 .equ Adr_NX10_xpic_vic_xpic_vic_default1       , 0x10140874
 .equ Adr_NX10_xpic_vic_default1                , 0x10140874

@ =====================================================================
@
@ Area xpic_wdg
 .equ Addr_NX10_xpic_wdg, 0x10140900
@
@ =====================================================================
 .equ Adr_NX10_xpic_wdg_xpic_wdg_trig            , 0x10140900
 .equ Adr_NX10_xpic_wdg_trig                     , 0x10140900
 .equ Adr_NX10_xpic_wdg_xpic_wdg_counter         , 0x10140904
 .equ Adr_NX10_xpic_wdg_counter                  , 0x10140904
 .equ Adr_NX10_xpic_wdg_xpic_wdg_xpic_irq_timeout, 0x10140908
 .equ Adr_NX10_xpic_wdg_xpic_irq_timeout         , 0x10140908
 .equ Adr_NX10_xpic_wdg_xpic_wdg_arm_irq_timeout , 0x1014090C
 .equ Adr_NX10_xpic_wdg_arm_irq_timeout          , 0x1014090C
 .equ Adr_NX10_xpic_wdg_xpic_wdg_irq_raw         , 0x10140910
 .equ Adr_NX10_xpic_wdg_irq_raw                  , 0x10140910
 .equ Adr_NX10_xpic_wdg_xpic_wdg_irq_masked      , 0x10140914
 .equ Adr_NX10_xpic_wdg_irq_masked               , 0x10140914
 .equ Adr_NX10_xpic_wdg_xpic_wdg_irq_msk_set     , 0x10140918
 .equ Adr_NX10_xpic_wdg_irq_msk_set              , 0x10140918
 .equ Adr_NX10_xpic_wdg_xpic_wdg_irq_msk_reset   , 0x1014091C
 .equ Adr_NX10_xpic_wdg_irq_msk_reset            , 0x1014091C

@ =====================================================================
@
@ Area intlogic_sys
 .equ Addr_NX10_intlogic_sys, 0x10180000
@
@ =====================================================================
@ =====================================================================
@
@ Area xc
 .equ Addr_NX10_xc, 0x10180000
 .equ Addr_NX10_xc_motion, 0x10100000
@
@ =====================================================================
 .equ Adr_NX10_xc_xc_base       , 0x10180000
 .equ Adr_NX10_xc_motion_xc_base, 0x10100000

@ =====================================================================
@
@ Area xpec
 .equ Addr_NX10_xpec0, 0x10180000
@
@ =====================================================================
 .equ Adr_NX10_xpec0_xpec_r0        , 0x10180000
 .equ Adr_NX10_xpec_r0              , 0x10180000
 .equ Adr_NX10_xpec0_xpec_r1        , 0x10180004
 .equ Adr_NX10_xpec_r1              , 0x10180004
 .equ Adr_NX10_xpec0_xpec_r2        , 0x10180008
 .equ Adr_NX10_xpec_r2              , 0x10180008
 .equ Adr_NX10_xpec0_xpec_r3        , 0x1018000C
 .equ Adr_NX10_xpec_r3              , 0x1018000C
 .equ Adr_NX10_xpec0_xpec_r4        , 0x10180010
 .equ Adr_NX10_xpec_r4              , 0x10180010
 .equ Adr_NX10_xpec0_xpec_r5        , 0x10180014
 .equ Adr_NX10_xpec_r5              , 0x10180014
 .equ Adr_NX10_xpec0_xpec_r6        , 0x10180018
 .equ Adr_NX10_xpec_r6              , 0x10180018
 .equ Adr_NX10_xpec0_xpec_r7        , 0x1018001C
 .equ Adr_NX10_xpec_r7              , 0x1018001C
 .equ Adr_NX10_xpec0_usr0           , 0x10180020
 .equ Adr_NX10_usr0                 , 0x10180020
 .equ Adr_NX10_xpec0_usr1           , 0x10180024
 .equ Adr_NX10_usr1                 , 0x10180024
 .equ Adr_NX10_xpec0_range45        , 0x10180028
 .equ Adr_NX10_range45              , 0x10180028
 .equ Adr_NX10_xpec0_range67        , 0x1018002C
 .equ Adr_NX10_range67              , 0x1018002C
 .equ Adr_NX10_xpec0_timer0         , 0x10180030
 .equ Adr_NX10_timer0               , 0x10180030
 .equ Adr_NX10_xpec0_timer1         , 0x10180034
 .equ Adr_NX10_timer1               , 0x10180034
 .equ Adr_NX10_xpec0_timer2         , 0x10180038
 .equ Adr_NX10_timer2               , 0x10180038
 .equ Adr_NX10_xpec0_timer3         , 0x1018003C
 .equ Adr_NX10_timer3               , 0x1018003C
 .equ Adr_NX10_xpec0_urx_count      , 0x10180040
 .equ Adr_NX10_urx_count            , 0x10180040
 .equ Adr_NX10_xpec0_utx_count      , 0x10180044
 .equ Adr_NX10_utx_count            , 0x10180044
 .equ Adr_NX10_xpec0_xpec_pc        , 0x10180048
 .equ Adr_NX10_xpec_pc              , 0x10180048
 .equ Adr_NX10_xpec0_zero           , 0x1018004C
 .equ Adr_NX10_zero                 , 0x1018004C
 .equ Adr_NX10_xpec0_xpec_statcfg   , 0x10180050
 .equ Adr_NX10_xpec_statcfg         , 0x10180050
 .equ Adr_NX10_xpec0_ec_maska       , 0x10180054
 .equ Adr_NX10_ec_maska             , 0x10180054
 .equ Adr_NX10_xpec0_ec_maskb       , 0x10180058
 .equ Adr_NX10_ec_maskb             , 0x10180058
 .equ Adr_NX10_xpec0_ec_mask0       , 0x1018005C
 .equ Adr_NX10_ec_mask0             , 0x1018005C
 .equ Adr_NX10_xpec0_ec_mask1       , 0x10180060
 .equ Adr_NX10_ec_mask1             , 0x10180060
 .equ Adr_NX10_xpec0_ec_mask2       , 0x10180064
 .equ Adr_NX10_ec_mask2             , 0x10180064
 .equ Adr_NX10_xpec0_ec_mask3       , 0x10180068
 .equ Adr_NX10_ec_mask3             , 0x10180068
 .equ Adr_NX10_xpec0_ec_mask4       , 0x1018006C
 .equ Adr_NX10_ec_mask4             , 0x1018006C
 .equ Adr_NX10_xpec0_ec_mask5       , 0x10180070
 .equ Adr_NX10_ec_mask5             , 0x10180070
 .equ Adr_NX10_xpec0_ec_mask6       , 0x10180074
 .equ Adr_NX10_ec_mask6             , 0x10180074
 .equ Adr_NX10_xpec0_ec_mask7       , 0x10180078
 .equ Adr_NX10_ec_mask7             , 0x10180078
 .equ Adr_NX10_xpec0_ec_mask8       , 0x1018007C
 .equ Adr_NX10_ec_mask8             , 0x1018007C
 .equ Adr_NX10_xpec0_ec_mask9       , 0x10180080
 .equ Adr_NX10_ec_mask9             , 0x10180080
 .equ Adr_NX10_xpec0_timer4         , 0x10180084
 .equ Adr_NX10_timer4               , 0x10180084
 .equ Adr_NX10_xpec0_timer5         , 0x10180088
 .equ Adr_NX10_timer5               , 0x10180088
 .equ Adr_NX10_xpec0_irq            , 0x1018008C
 .equ Adr_NX10_irq                  , 0x1018008C
 .equ Adr_NX10_xpec0_xpec_systime_ns, 0x10180090
 .equ Adr_NX10_xpec_systime_ns      , 0x10180090
 .equ Adr_NX10_xpec0_fifo_data      , 0x10180094
 .equ Adr_NX10_fifo_data            , 0x10180094
 .equ Adr_NX10_xpec0_xpec_systime_s , 0x10180098
 .equ Adr_NX10_xpec_systime_s       , 0x10180098
 .equ Adr_NX10_xpec0_xpec_adc       , 0x1018009C
 .equ Adr_NX10_xpec_adc             , 0x1018009C
 .equ Adr_NX10_xpec0_xpec_sr0       , 0x101800A0
 .equ Adr_NX10_xpec_sr0             , 0x101800A0
 .equ Adr_NX10_xpec0_xpec_sr1       , 0x101800A4
 .equ Adr_NX10_xpec_sr1             , 0x101800A4
 .equ Adr_NX10_xpec0_xpec_sr2       , 0x101800A8
 .equ Adr_NX10_xpec_sr2             , 0x101800A8
 .equ Adr_NX10_xpec0_xpec_sr3       , 0x101800AC
 .equ Adr_NX10_xpec_sr3             , 0x101800AC
 .equ Adr_NX10_xpec0_xpec_sr4       , 0x101800B0
 .equ Adr_NX10_xpec_sr4             , 0x101800B0
 .equ Adr_NX10_xpec0_xpec_sr5       , 0x101800B4
 .equ Adr_NX10_xpec_sr5             , 0x101800B4
 .equ Adr_NX10_xpec0_statcfg0       , 0x101800E0
 .equ Adr_NX10_statcfg0             , 0x101800E0
 .equ Adr_NX10_xpec0_urtx0          , 0x101800F0
 .equ Adr_NX10_urtx0                , 0x101800F0
 .equ Adr_NX10_xpec0_xpu_hold_pc    , 0x10180100
 .equ Adr_NX10_xpu_hold_pc          , 0x10180100
 .equ Adr_NX10_xpec0_pram_start     , 0x10184000
 .equ Adr_NX10_pram_start           , 0x10184000
 .equ Adr_NX10_xpec0_dram_start     , 0x10186000
 .equ Adr_NX10_dram_start           , 0x10186000

@ =====================================================================
@
@ Area xpic
 .equ Addr_NX10_xpic, 0x10188000
@
@ =====================================================================
 .equ Adr_NX10_xpic_xpic_r0        , 0x10188000
 .equ Adr_NX10_xpic_r0             , 0x10188000
 .equ Adr_NX10_xpic_xpic_r1        , 0x10188004
 .equ Adr_NX10_xpic_r1             , 0x10188004
 .equ Adr_NX10_xpic_xpic_r2        , 0x10188008
 .equ Adr_NX10_xpic_r2             , 0x10188008
 .equ Adr_NX10_xpic_xpic_r3        , 0x1018800C
 .equ Adr_NX10_xpic_r3             , 0x1018800C
 .equ Adr_NX10_xpic_xpic_r4        , 0x10188010
 .equ Adr_NX10_xpic_r4             , 0x10188010
 .equ Adr_NX10_xpic_xpic_r5        , 0x10188014
 .equ Adr_NX10_xpic_r5             , 0x10188014
 .equ Adr_NX10_xpic_xpic_r6        , 0x10188018
 .equ Adr_NX10_xpic_r6             , 0x10188018
 .equ Adr_NX10_xpic_xpic_r7        , 0x1018801C
 .equ Adr_NX10_xpic_r7             , 0x1018801C
 .equ Adr_NX10_xpic_xpic_usr0      , 0x10188020
 .equ Adr_NX10_xpic_usr0           , 0x10188020
 .equ Adr_NX10_xpic_xpic_usr1      , 0x10188024
 .equ Adr_NX10_xpic_usr1           , 0x10188024
 .equ Adr_NX10_xpic_xpic_usr2      , 0x10188028
 .equ Adr_NX10_xpic_usr2           , 0x10188028
 .equ Adr_NX10_xpic_xpic_usr3      , 0x1018802C
 .equ Adr_NX10_xpic_usr3           , 0x1018802C
 .equ Adr_NX10_xpic_xpic_usr4      , 0x10188030
 .equ Adr_NX10_xpic_usr4           , 0x10188030
 .equ Adr_NX10_xpic_xpic_pc        , 0x10188034
 .equ Adr_NX10_xpic_pc             , 0x10188034
 .equ Adr_NX10_xpic_xpic_stat      , 0x10188038
 .equ Adr_NX10_xpic_stat           , 0x10188038
 .equ Adr_NX10_xpic_xpic_zero      , 0x1018803C
 .equ Adr_NX10_xpic_zero           , 0x1018803C
 .equ Adr_NX10_xpic_xpic_pram_start, 0x1018C000
 .equ Adr_NX10_xpic_pram_start     , 0x1018C000
 .equ Adr_NX10_xpic_xpic_dram_start, 0x1018E000
 .equ Adr_NX10_xpic_dram_start     , 0x1018E000

@ =====================================================================
@
@ Area xpic_debug
 .equ Addr_NX10_xpic_debug, 0x10188100
@
@ =====================================================================
 .equ Adr_NX10_xpic_debug_xpic_hold_pc                , 0x10188100
 .equ Adr_NX10_xpic_hold_pc                           , 0x10188100
 .equ Adr_NX10_xpic_debug_xpic_break0_addr            , 0x10188104
 .equ Adr_NX10_xpic_break0_addr                       , 0x10188104
 .equ Adr_NX10_xpic_debug_xpic_break0_addr_mask       , 0x10188108
 .equ Adr_NX10_xpic_break0_addr_mask                  , 0x10188108
 .equ Adr_NX10_xpic_debug_xpic_break0_data            , 0x1018810C
 .equ Adr_NX10_xpic_break0_data                       , 0x1018810C
 .equ Adr_NX10_xpic_debug_xpic_break0_data_mask       , 0x10188110
 .equ Adr_NX10_xpic_break0_data_mask                  , 0x10188110
 .equ Adr_NX10_xpic_debug_xpic_break0_contr           , 0x10188114
 .equ Adr_NX10_xpic_break0_contr                      , 0x10188114
 .equ Adr_NX10_xpic_debug_xpic_break0_contr_mask      , 0x10188118
 .equ Adr_NX10_xpic_break0_contr_mask                 , 0x10188118
 .equ Adr_NX10_xpic_debug_xpic_break1_addr            , 0x1018811C
 .equ Adr_NX10_xpic_break1_addr                       , 0x1018811C
 .equ Adr_NX10_xpic_debug_xpic_break1_addr_mask       , 0x10188120
 .equ Adr_NX10_xpic_break1_addr_mask                  , 0x10188120
 .equ Adr_NX10_xpic_debug_xpic_break1_data            , 0x10188124
 .equ Adr_NX10_xpic_break1_data                       , 0x10188124
 .equ Adr_NX10_xpic_debug_xpic_break1_data_mask       , 0x10188128
 .equ Adr_NX10_xpic_break1_data_mask                  , 0x10188128
 .equ Adr_NX10_xpic_debug_xpic_break1_contr           , 0x1018812C
 .equ Adr_NX10_xpic_break1_contr                      , 0x1018812C
 .equ Adr_NX10_xpic_debug_xpic_break1_contr_mask      , 0x10188130
 .equ Adr_NX10_xpic_break1_contr_mask                 , 0x10188130
 .equ Adr_NX10_xpic_debug_xpic_break_last_pc          , 0x10188134
 .equ Adr_NX10_xpic_break_last_pc                     , 0x10188134
 .equ Adr_NX10_xpic_debug_xpic_break_status           , 0x10188138
 .equ Adr_NX10_xpic_break_status                      , 0x10188138
 .equ Adr_NX10_xpic_debug_xpic_break_irq_raw          , 0x1018813C
 .equ Adr_NX10_xpic_break_irq_raw                     , 0x1018813C
 .equ Adr_NX10_xpic_debug_xpic_break_irq_masked       , 0x10188140
 .equ Adr_NX10_xpic_break_irq_masked                  , 0x10188140
 .equ Adr_NX10_xpic_debug_xpic_break_irq_msk_set      , 0x10188144
 .equ Adr_NX10_xpic_break_irq_msk_set                 , 0x10188144
 .equ Adr_NX10_xpic_debug_xpic_break_irq_msk_reset    , 0x10188148
 .equ Adr_NX10_xpic_break_irq_msk_reset               , 0x10188148
 .equ Adr_NX10_xpic_debug_xpic_break_own_irq_masked   , 0x1018814C
 .equ Adr_NX10_xpic_break_own_irq_masked              , 0x1018814C
 .equ Adr_NX10_xpic_debug_xpic_break_own_irq_msk_set  , 0x10188150
 .equ Adr_NX10_xpic_break_own_irq_msk_set             , 0x10188150
 .equ Adr_NX10_xpic_debug_xpic_break_own_irq_msk_reset, 0x10188154
 .equ Adr_NX10_xpic_break_own_irq_msk_reset           , 0x10188154
 .equ Adr_NX10_xpic_debug_xpic_break_return_fiq_pc    , 0x10188158
 .equ Adr_NX10_xpic_break_return_fiq_pc               , 0x10188158
 .equ Adr_NX10_xpic_debug_xpic_break_return_irq_pc    , 0x1018815C
 .equ Adr_NX10_xpic_break_return_irq_pc               , 0x1018815C

@ =====================================================================
@
@ Area xmac
 .equ Addr_NX10_xmac0, 0x101A0000
@
@ =====================================================================
 .equ Adr_NX10_xmac0_xmac_rpu_program_start    , 0x101A0000
 .equ Adr_NX10_xmac_rpu_program_start          , 0x101A0000
 .equ Adr_NX10_xmac0_xmac_rpu_program_end      , 0x101A03FF
 .equ Adr_NX10_xmac_rpu_program_end            , 0x101A03FF
 .equ Adr_NX10_xmac0_xmac_tpu_program_start    , 0x101A0400
 .equ Adr_NX10_xmac_tpu_program_start          , 0x101A0400
 .equ Adr_NX10_xmac0_xmac_tpu_program_end      , 0x101A07FF
 .equ Adr_NX10_xmac_tpu_program_end            , 0x101A07FF
 .equ Adr_NX10_xmac0_xmac_sr0                  , 0x101A0800
 .equ Adr_NX10_xmac_sr0                        , 0x101A0800
 .equ Adr_NX10_xmac0_xmac_sr1                  , 0x101A0804
 .equ Adr_NX10_xmac_sr1                        , 0x101A0804
 .equ Adr_NX10_xmac0_xmac_sr2                  , 0x101A0808
 .equ Adr_NX10_xmac_sr2                        , 0x101A0808
 .equ Adr_NX10_xmac0_xmac_sr3                  , 0x101A080C
 .equ Adr_NX10_xmac_sr3                        , 0x101A080C
 .equ Adr_NX10_xmac0_xmac_sr4                  , 0x101A0810
 .equ Adr_NX10_xmac_sr4                        , 0x101A0810
 .equ Adr_NX10_xmac0_xmac_sr5                  , 0x101A0814
 .equ Adr_NX10_xmac_sr5                        , 0x101A0814
 .equ Adr_NX10_xmac0_xmac_status_shared0       , 0x101A0840
 .equ Adr_NX10_xmac_status_shared0             , 0x101A0840
 .equ Adr_NX10_xmac0_xmac_config_shared0       , 0x101A0844
 .equ Adr_NX10_xmac_config_shared0             , 0x101A0844
 .equ Adr_NX10_xmac0_xmac_urx_utx0             , 0x101A0860
 .equ Adr_NX10_xmac_urx_utx0                   , 0x101A0860
 .equ Adr_NX10_xmac0_xmac_urx                  , 0x101A0870
 .equ Adr_NX10_xmac_urx                        , 0x101A0870
 .equ Adr_NX10_xmac0_xmac_utx                  , 0x101A0874
 .equ Adr_NX10_xmac_utx                        , 0x101A0874
 .equ Adr_NX10_xmac0_xmac_rx                   , 0x101A0878
 .equ Adr_NX10_xmac_rx                         , 0x101A0878
 .equ Adr_NX10_xmac0_xmac_rx_hw                , 0x101A0880
 .equ Adr_NX10_xmac_rx_hw                      , 0x101A0880
 .equ Adr_NX10_xmac0_xmac_rx_hw_count          , 0x101A0884
 .equ Adr_NX10_xmac_rx_hw_count                , 0x101A0884
 .equ Adr_NX10_xmac0_xmac_tx                   , 0x101A0888
 .equ Adr_NX10_xmac_tx                         , 0x101A0888
 .equ Adr_NX10_xmac0_xmac_tx_hw                , 0x101A088C
 .equ Adr_NX10_xmac_tx_hw                      , 0x101A088C
 .equ Adr_NX10_xmac0_xmac_tx_hw_count          , 0x101A0890
 .equ Adr_NX10_xmac_tx_hw_count                , 0x101A0890
 .equ Adr_NX10_xmac0_xmac_tx_sent              , 0x101A0894
 .equ Adr_NX10_xmac_tx_sent                    , 0x101A0894
 .equ Adr_NX10_xmac0_xmac_rpu_pc               , 0x101A0898
 .equ Adr_NX10_xmac_rpu_pc                     , 0x101A0898
 .equ Adr_NX10_xmac0_xmac_tpu_pc               , 0x101A089C
 .equ Adr_NX10_xmac_tpu_pc                     , 0x101A089C
 .equ Adr_NX10_xmac0_xmac_wr0                  , 0x101A08A0
 .equ Adr_NX10_xmac_wr0                        , 0x101A08A0
 .equ Adr_NX10_xmac0_xmac_wr1                  , 0x101A08A4
 .equ Adr_NX10_xmac_wr1                        , 0x101A08A4
 .equ Adr_NX10_xmac0_xmac_wr2                  , 0x101A08A8
 .equ Adr_NX10_xmac_wr2                        , 0x101A08A8
 .equ Adr_NX10_xmac0_xmac_wr3                  , 0x101A08AC
 .equ Adr_NX10_xmac_wr3                        , 0x101A08AC
 .equ Adr_NX10_xmac0_xmac_wr4                  , 0x101A08B0
 .equ Adr_NX10_xmac_wr4                        , 0x101A08B0
 .equ Adr_NX10_xmac0_xmac_wr5                  , 0x101A08B4
 .equ Adr_NX10_xmac_wr5                        , 0x101A08B4
 .equ Adr_NX10_xmac0_xmac_wr6                  , 0x101A08B8
 .equ Adr_NX10_xmac_wr6                        , 0x101A08B8
 .equ Adr_NX10_xmac0_xmac_wr7                  , 0x101A08BC
 .equ Adr_NX10_xmac_wr7                        , 0x101A08BC
 .equ Adr_NX10_xmac0_xmac_wr8                  , 0x101A08C0
 .equ Adr_NX10_xmac_wr8                        , 0x101A08C0
 .equ Adr_NX10_xmac0_xmac_wr9                  , 0x101A08C4
 .equ Adr_NX10_xmac_wr9                        , 0x101A08C4
 .equ Adr_NX10_xmac0_xmac_sys_time             , 0x101A08C8
 .equ Adr_NX10_xmac_sys_time                   , 0x101A08C8
 .equ Adr_NX10_xmac0_xmac_sys_time_upper       , 0x101A08CC
 .equ Adr_NX10_xmac_sys_time_upper             , 0x101A08CC
 .equ Adr_NX10_xmac0_xmac_cmp0_status          , 0x101A08D0
 .equ Adr_NX10_xmac_cmp0_status                , 0x101A08D0
 .equ Adr_NX10_xmac0_xmac_cmp1_status          , 0x101A08D4
 .equ Adr_NX10_xmac_cmp1_status                , 0x101A08D4
 .equ Adr_NX10_xmac0_xmac_cmp2_status          , 0x101A08D8
 .equ Adr_NX10_xmac_cmp2_status                , 0x101A08D8
 .equ Adr_NX10_xmac0_xmac_cmp3_status          , 0x101A08DC
 .equ Adr_NX10_xmac_cmp3_status                , 0x101A08DC
 .equ Adr_NX10_xmac0_xmac_alu_flags            , 0x101A08E0
 .equ Adr_NX10_xmac_alu_flags                  , 0x101A08E0
 .equ Adr_NX10_xmac0_xmac_status_int           , 0x101A08E4
 .equ Adr_NX10_xmac_status_int                 , 0x101A08E4
 .equ Adr_NX10_xmac0_xmac_stat_bits            , 0x101A08E8
 .equ Adr_NX10_xmac_stat_bits                  , 0x101A08E8
 .equ Adr_NX10_xmac0_xmac_status_mii           , 0x101A08EC
 .equ Adr_NX10_xmac_status_mii                 , 0x101A08EC
 .equ Adr_NX10_xmac0_xmac_status_mii2          , 0x101A08F0
 .equ Adr_NX10_xmac_status_mii2                , 0x101A08F0
 .equ Adr_NX10_xmac0_xmac_config_mii           , 0x101A08F8
 .equ Adr_NX10_xmac_config_mii                 , 0x101A08F8
 .equ Adr_NX10_xmac0_xmac_config_rx_nibble_fifo, 0x101A08FC
 .equ Adr_NX10_xmac_config_rx_nibble_fifo      , 0x101A08FC
 .equ Adr_NX10_xmac0_xmac_config_tx_nibble_fifo, 0x101A0900
 .equ Adr_NX10_xmac_config_tx_nibble_fifo      , 0x101A0900
 .equ Adr_NX10_xmac0_xmac_config_sbu           , 0x101A0904
 .equ Adr_NX10_xmac_config_sbu                 , 0x101A0904
 .equ Adr_NX10_xmac0_xmac_sbu_rate_mul_add     , 0x101A0908
 .equ Adr_NX10_xmac_sbu_rate_mul_add           , 0x101A0908
 .equ Adr_NX10_xmac0_xmac_sbu_rate_mul_start   , 0x101A090C
 .equ Adr_NX10_xmac_sbu_rate_mul_start         , 0x101A090C
 .equ Adr_NX10_xmac0_xmac_sbu_rate_mul         , 0x101A0910
 .equ Adr_NX10_xmac_sbu_rate_mul               , 0x101A0910
 .equ Adr_NX10_xmac0_xmac_start_sample_pos     , 0x101A0914
 .equ Adr_NX10_xmac_start_sample_pos           , 0x101A0914
 .equ Adr_NX10_xmac0_xmac_stop_sample_pos      , 0x101A0918
 .equ Adr_NX10_xmac_stop_sample_pos            , 0x101A0918
 .equ Adr_NX10_xmac0_xmac_config_obu           , 0x101A091C
 .equ Adr_NX10_xmac_config_obu                 , 0x101A091C
 .equ Adr_NX10_xmac0_xmac_obu_rate_mul_add     , 0x101A0920
 .equ Adr_NX10_xmac_obu_rate_mul_add           , 0x101A0920
 .equ Adr_NX10_xmac0_xmac_obu_rate_mul_start   , 0x101A0924
 .equ Adr_NX10_xmac_obu_rate_mul_start         , 0x101A0924
 .equ Adr_NX10_xmac0_xmac_obu_rate_mul         , 0x101A0928
 .equ Adr_NX10_xmac_obu_rate_mul               , 0x101A0928
 .equ Adr_NX10_xmac0_xmac_start_trans_pos      , 0x101A092C
 .equ Adr_NX10_xmac_start_trans_pos            , 0x101A092C
 .equ Adr_NX10_xmac0_xmac_stop_trans_pos       , 0x101A0930
 .equ Adr_NX10_xmac_stop_trans_pos             , 0x101A0930
 .equ Adr_NX10_xmac0_xmac_rpu_count1           , 0x101A0934
 .equ Adr_NX10_xmac_rpu_count1                 , 0x101A0934
 .equ Adr_NX10_xmac0_xmac_rpu_count2           , 0x101A0938
 .equ Adr_NX10_xmac_rpu_count2                 , 0x101A0938
 .equ Adr_NX10_xmac0_xmac_tpu_count1           , 0x101A093C
 .equ Adr_NX10_xmac_tpu_count1                 , 0x101A093C
 .equ Adr_NX10_xmac0_xmac_tpu_count2           , 0x101A0940
 .equ Adr_NX10_xmac_tpu_count2                 , 0x101A0940
 .equ Adr_NX10_xmac0_xmac_rx_count             , 0x101A0944
 .equ Adr_NX10_xmac_rx_count                   , 0x101A0944
 .equ Adr_NX10_xmac0_xmac_tx_count             , 0x101A0948
 .equ Adr_NX10_xmac_tx_count                   , 0x101A0948
 .equ Adr_NX10_xmac0_xmac_rpm_mask0            , 0x101A094C
 .equ Adr_NX10_xmac_rpm_mask0                  , 0x101A094C
 .equ Adr_NX10_xmac0_xmac_rpm_val0             , 0x101A0950
 .equ Adr_NX10_xmac_rpm_val0                   , 0x101A0950
 .equ Adr_NX10_xmac0_xmac_rpm_mask1            , 0x101A0954
 .equ Adr_NX10_xmac_rpm_mask1                  , 0x101A0954
 .equ Adr_NX10_xmac0_xmac_rpm_val1             , 0x101A0958
 .equ Adr_NX10_xmac_rpm_val1                   , 0x101A0958
 .equ Adr_NX10_xmac0_xmac_tpm_mask0            , 0x101A095C
 .equ Adr_NX10_xmac_tpm_mask0                  , 0x101A095C
 .equ Adr_NX10_xmac0_xmac_tpm_val0             , 0x101A0960
 .equ Adr_NX10_xmac_tpm_val0                   , 0x101A0960
 .equ Adr_NX10_xmac0_xmac_tpm_mask1            , 0x101A0964
 .equ Adr_NX10_xmac_tpm_mask1                  , 0x101A0964
 .equ Adr_NX10_xmac0_xmac_tpm_val1             , 0x101A0968
 .equ Adr_NX10_xmac_tpm_val1                   , 0x101A0968
 .equ Adr_NX10_xmac0_xmac_rx_tx_nof_bits       , 0x101A09BC
 .equ Adr_NX10_xmac_rx_tx_nof_bits             , 0x101A09BC
 .equ Adr_NX10_xmac0_xmac_rx_crc32_l           , 0x101A09C0
 .equ Adr_NX10_xmac_rx_crc32_l                 , 0x101A09C0
 .equ Adr_NX10_xmac0_xmac_rx_crc32_h           , 0x101A09C4
 .equ Adr_NX10_xmac_rx_crc32_h                 , 0x101A09C4
 .equ Adr_NX10_xmac0_xmac_rx_crc32_cfg         , 0x101A09C8
 .equ Adr_NX10_xmac_rx_crc32_cfg               , 0x101A09C8
 .equ Adr_NX10_xmac0_xmac_tx_crc32_l           , 0x101A09CC
 .equ Adr_NX10_xmac_tx_crc32_l                 , 0x101A09CC
 .equ Adr_NX10_xmac0_xmac_tx_crc32_h           , 0x101A09D0
 .equ Adr_NX10_xmac_tx_crc32_h                 , 0x101A09D0
 .equ Adr_NX10_xmac0_xmac_tx_crc32_cfg         , 0x101A09D4
 .equ Adr_NX10_xmac_tx_crc32_cfg               , 0x101A09D4
 .equ Adr_NX10_xmac0_xmac_rx_crc_polynomial_l  , 0x101A09D8
 .equ Adr_NX10_xmac_rx_crc_polynomial_l        , 0x101A09D8
 .equ Adr_NX10_xmac0_xmac_rx_crc_polynomial_h  , 0x101A09DC
 .equ Adr_NX10_xmac_rx_crc_polynomial_h        , 0x101A09DC
 .equ Adr_NX10_xmac0_xmac_rx_crc_l             , 0x101A09E0
 .equ Adr_NX10_xmac_rx_crc_l                   , 0x101A09E0
 .equ Adr_NX10_xmac0_xmac_rx_crc_h             , 0x101A09E4
 .equ Adr_NX10_xmac_rx_crc_h                   , 0x101A09E4
 .equ Adr_NX10_xmac0_xmac_rx_crc_cfg           , 0x101A09E8
 .equ Adr_NX10_xmac_rx_crc_cfg                 , 0x101A09E8
 .equ Adr_NX10_xmac0_xmac_tx_crc_polynomial_l  , 0x101A09EC
 .equ Adr_NX10_xmac_tx_crc_polynomial_l        , 0x101A09EC
 .equ Adr_NX10_xmac0_xmac_tx_crc_polynomial_h  , 0x101A09F0
 .equ Adr_NX10_xmac_tx_crc_polynomial_h        , 0x101A09F0
 .equ Adr_NX10_xmac0_xmac_tx_crc_l             , 0x101A09F4
 .equ Adr_NX10_xmac_tx_crc_l                   , 0x101A09F4
 .equ Adr_NX10_xmac0_xmac_tx_crc_h             , 0x101A09F8
 .equ Adr_NX10_xmac_tx_crc_h                   , 0x101A09F8
 .equ Adr_NX10_xmac0_xmac_tx_crc_cfg           , 0x101A09FC
 .equ Adr_NX10_xmac_tx_crc_cfg                 , 0x101A09FC
 .equ Adr_NX10_xmac0_xmac_rpu_hold_pc          , 0x101A0A00
 .equ Adr_NX10_xmac_rpu_hold_pc                , 0x101A0A00
 .equ Adr_NX10_xmac0_xmac_tpu_hold_pc          , 0x101A0A04
 .equ Adr_NX10_xmac_tpu_hold_pc                , 0x101A0A04

@ =====================================================================
@
@ Area pointer_fifo
 .equ Addr_NX10_pointer_fifo, 0x101A4000
 .equ Addr_NX10_pointer_fifo_motion, 0x10124000
@
@ =====================================================================
 .equ Adr_NX10_pointer_fifo_pfifo_base                  , 0x101A4000
 .equ Adr_NX10_pointer_fifo_motion_pfifo_base           , 0x10124000
 .equ Adr_NX10_pointer_fifo_pfifo_border_base           , 0x101A4040
 .equ Adr_NX10_pointer_fifo_motion_pfifo_border_base    , 0x10124040
 .equ Adr_NX10_pointer_fifo_pfifo_reset                 , 0x101A4080
 .equ Adr_NX10_pointer_fifo_motion_pfifo_reset          , 0x10124080
 .equ Adr_NX10_pointer_fifo_pfifo_full                  , 0x101A4084
 .equ Adr_NX10_pointer_fifo_motion_pfifo_full           , 0x10124084
 .equ Adr_NX10_pointer_fifo_pfifo_empty                 , 0x101A4088
 .equ Adr_NX10_pointer_fifo_motion_pfifo_empty          , 0x10124088
 .equ Adr_NX10_pointer_fifo_pfifo_overflow              , 0x101A408C
 .equ Adr_NX10_pointer_fifo_motion_pfifo_overflow       , 0x1012408C
 .equ Adr_NX10_pointer_fifo_pfifo_underrun              , 0x101A4090
 .equ Adr_NX10_pointer_fifo_motion_pfifo_underrun       , 0x10124090
 .equ Adr_NX10_pointer_fifo_pfifo_fill_level_base       , 0x101A40C0
 .equ Adr_NX10_pointer_fifo_motion_pfifo_fill_level_base, 0x101240C0

@ =====================================================================
@
@ Area xpec_irq_registers
 .equ Addr_NX10_xpec_irq_registers, 0x101A4400
@
@ =====================================================================
 .equ Adr_NX10_xpec_irq_registers_irq_xpec0, 0x101A4400
 .equ Adr_NX10_irq_xpec0                   , 0x101A4400

@ =====================================================================
@
@ Area xc_mem_prot
 .equ Addr_NX10_xc_mem_prot, 0x101A4800
@
@ =====================================================================
 .equ Adr_NX10_xc_mem_prot_xpec0_ram_low   , 0x101A4800
 .equ Adr_NX10_xpec0_ram_low               , 0x101A4800
 .equ Adr_NX10_xc_mem_prot_xpec0_ram_high  , 0x101A4810
 .equ Adr_NX10_xpec0_ram_high              , 0x101A4810
 .equ Adr_NX10_xc_mem_prot_prot_key_enable , 0x101A4900
 .equ Adr_NX10_prot_key_enable             , 0x101A4900
 .equ Adr_NX10_xc_mem_prot_prot_key_disable, 0x101A4904
 .equ Adr_NX10_prot_key_disable            , 0x101A4904
 .equ Adr_NX10_xc_mem_prot_prot_status     , 0x101A4908
 .equ Adr_NX10_prot_status                 , 0x101A4908

@ =====================================================================
@
@ Area buf_man
 .equ Addr_NX10_buf_man, 0x101A5600
 .equ Addr_NX10_buf_man_motion, 0x10125600
@
@ =====================================================================
 .equ Adr_NX10_buf_man_buf_man       , 0x101A5600
 .equ Adr_NX10_buf_man_motion_buf_man, 0x10125600

@ =====================================================================
@
@ Area asic_ctrl
 .equ Addr_NX10_asic_ctrl, 0x101C0000
@
@ =====================================================================
 .equ Adr_NX10_asic_ctrl_io_status               , 0x101C0000
 .equ Adr_NX10_io_status                         , 0x101C0000
 .equ Adr_NX10_asic_ctrl_io_config               , 0x101C0004
 .equ Adr_NX10_io_config                         , 0x101C0004
 .equ Adr_NX10_asic_ctrl_io_config_mask          , 0x101C0008
 .equ Adr_NX10_io_config_mask                    , 0x101C0008
 .equ Adr_NX10_asic_ctrl_reset_ctrl              , 0x101C000C
 .equ Adr_NX10_reset_ctrl                        , 0x101C000C
 .equ Adr_NX10_asic_ctrl_phy_control             , 0x101C0010
 .equ Adr_NX10_phy_control                       , 0x101C0010
 .equ Adr_NX10_asic_ctrl_armclk_rate_mul_add     , 0x101C0014
 .equ Adr_NX10_armclk_rate_mul_add               , 0x101C0014
 .equ Adr_NX10_asic_ctrl_usb12clk_rate_mul_add   , 0x101C0018
 .equ Adr_NX10_usb12clk_rate_mul_add             , 0x101C0018
 .equ Adr_NX10_asic_ctrl_adcclk_div              , 0x101C001C
 .equ Adr_NX10_adcclk_div                        , 0x101C001C
 .equ Adr_NX10_asic_ctrl_fb0clk_rate_mul_add     , 0x101C0020
 .equ Adr_NX10_fb0clk_rate_mul_add               , 0x101C0020
 .equ Adr_NX10_asic_ctrl_fb0clk_div              , 0x101C0024
 .equ Adr_NX10_fb0clk_div                        , 0x101C0024
 .equ Adr_NX10_asic_ctrl_clock_enable            , 0x101C0028
 .equ Adr_NX10_clock_enable                      , 0x101C0028
 .equ Adr_NX10_asic_ctrl_clock_enable_mask       , 0x101C002C
 .equ Adr_NX10_clock_enable_mask                 , 0x101C002C
 .equ Adr_NX10_asic_ctrl_misc_asic_ctrl          , 0x101C0030
 .equ Adr_NX10_misc_asic_ctrl                    , 0x101C0030
 .equ Adr_NX10_asic_ctrl_only_porn               , 0x101C0034
 .equ Adr_NX10_only_porn                         , 0x101C0034
 .equ Adr_NX10_asic_ctrl_netx_version            , 0x101C0038
 .equ Adr_NX10_netx_version                      , 0x101C0038
 .equ Adr_NX10_asic_ctrl_rom_wdg                 , 0x101C003C
 .equ Adr_NX10_rom_wdg                           , 0x101C003C
 .equ Adr_NX10_asic_ctrl_sample_at_nres          , 0x101C0040
 .equ Adr_NX10_sample_at_nres                    , 0x101C0040
 .equ Adr_NX10_asic_ctrl_netx_status             , 0x101C0044
 .equ Adr_NX10_netx_status                       , 0x101C0044
 .equ Adr_NX10_asic_ctrl_rdy_run_cfg             , 0x101C0048
 .equ Adr_NX10_rdy_run_cfg                       , 0x101C0048
 .equ Adr_NX10_asic_ctrl_system_status           , 0x101C004C
 .equ Adr_NX10_system_status                     , 0x101C004C
 .equ Adr_NX10_asic_ctrl_netx_lic_id             , 0x101C0050
 .equ Adr_NX10_netx_lic_id                       , 0x101C0050
 .equ Adr_NX10_asic_ctrl_netx_lic_flags0         , 0x101C0054
 .equ Adr_NX10_netx_lic_flags0                   , 0x101C0054
 .equ Adr_NX10_asic_ctrl_netx_lic_flags1         , 0x101C0058
 .equ Adr_NX10_netx_lic_flags1                   , 0x101C0058
 .equ Adr_NX10_asic_ctrl_netx_lic_errors0        , 0x101C005C
 .equ Adr_NX10_netx_lic_errors0                  , 0x101C005C
 .equ Adr_NX10_asic_ctrl_netx_lic_errors1        , 0x101C0060
 .equ Adr_NX10_netx_lic_errors1                  , 0x101C0060
 .equ Adr_NX10_asic_ctrl_asic_ctrl_access_key    , 0x101C0070
 .equ Adr_NX10_asic_ctrl_access_key              , 0x101C0070
 .equ Adr_NX10_asic_ctrl_netx_lock_reserved00    , 0x101C0074
 .equ Adr_NX10_netx_lock_reserved00              , 0x101C0074
 .equ Adr_NX10_asic_ctrl_netx_lock_reserved01    , 0x101C0078
 .equ Adr_NX10_netx_lock_reserved01              , 0x101C0078
 .equ Adr_NX10_asic_ctrl_netx_lock_reserved_start, 0x101C0080
 .equ Adr_NX10_netx_lock_reserved_start          , 0x101C0080
 .equ Adr_NX10_asic_ctrl_netx_lock_reserved_end  , 0x101C00FC
 .equ Adr_NX10_netx_lock_reserved_end            , 0x101C00FC

@ =====================================================================
@
@ Area extmemctrl
 .equ Addr_NX10_extmemctrl, 0x101C0100
@
@ =====================================================================
@ =====================================================================
@
@ Area ext_asyncmem_ctrl
 .equ Addr_NX10_ext_asyncmem_ctrl, 0x101C0100
@
@ =====================================================================
 .equ Adr_NX10_ext_asyncmem_ctrl_extsram0_ctrl   , 0x101C0100
 .equ Adr_NX10_extsram0_ctrl                     , 0x101C0100
 .equ Adr_NX10_ext_asyncmem_ctrl_extsram1_ctrl   , 0x101C0104
 .equ Adr_NX10_extsram1_ctrl                     , 0x101C0104
 .equ Adr_NX10_ext_asyncmem_ctrl_extsram2_ctrl   , 0x101C0108
 .equ Adr_NX10_extsram2_ctrl                     , 0x101C0108
 .equ Adr_NX10_ext_asyncmem_ctrl_extsram3_ctrl   , 0x101C010C
 .equ Adr_NX10_extsram3_ctrl                     , 0x101C010C
 .equ Adr_NX10_ext_asyncmem_ctrl_ext_cs0_apm_ctrl, 0x101C0110
 .equ Adr_NX10_ext_cs0_apm_ctrl                  , 0x101C0110
 .equ Adr_NX10_ext_asyncmem_ctrl_ext_rdy_cfg     , 0x101C0120
 .equ Adr_NX10_ext_rdy_cfg                       , 0x101C0120
 .equ Adr_NX10_ext_asyncmem_ctrl_ext_rdy_status  , 0x101C0124
 .equ Adr_NX10_ext_rdy_status                    , 0x101C0124

@ =====================================================================
@
@ Area ext_sdram_ctrl
 .equ Addr_NX10_ext_sdram_ctrl, 0x101C0140
@
@ =====================================================================
 .equ Adr_NX10_ext_sdram_ctrl_sdram_general_ctrl, 0x101C0140
 .equ Adr_NX10_sdram_general_ctrl               , 0x101C0140
 .equ Adr_NX10_ext_sdram_ctrl_sdram_timing_ctrl , 0x101C0144
 .equ Adr_NX10_sdram_timing_ctrl                , 0x101C0144
 .equ Adr_NX10_ext_sdram_ctrl_sdram_mr          , 0x101C0148
 .equ Adr_NX10_sdram_mr                         , 0x101C0148

@ =====================================================================
@
@ Area watchdog
 .equ Addr_NX10_watchdog, 0x101C0200
@
@ =====================================================================
 .equ Adr_NX10_watchdog_netx_sys_wdg_ctrl       , 0x101C0200
 .equ Adr_NX10_netx_sys_wdg_ctrl                , 0x101C0200
 .equ Adr_NX10_watchdog_netx_sys_wdg            , 0x101C0204
 .equ Adr_NX10_netx_sys_wdg                     , 0x101C0204
 .equ Adr_NX10_watchdog_netx_sys_wdg_irq_timeout, 0x101C0208
 .equ Adr_NX10_netx_sys_wdg_irq_timeout         , 0x101C0208
 .equ Adr_NX10_watchdog_netx_sys_wdg_res_timeout, 0x101C020C
 .equ Adr_NX10_netx_sys_wdg_res_timeout         , 0x101C020C

@ =====================================================================
@
@ Area abort
 .equ Addr_NX10_abort, 0x101C0300
@
@ =====================================================================
@ =====================================================================
@
@ Area gpio
 .equ Addr_NX10_gpio, 0x101C0800
 .equ Addr_NX10_gpio_motion, 0x10140400
@
@ =====================================================================
 .equ Adr_NX10_gpio_gpio_cfg0                , 0x101C0800
 .equ Adr_NX10_gpio_motion_gpio_cfg0         , 0x10140400
 .equ Adr_NX10_gpio_gpio_cfg1                , 0x101C0804
 .equ Adr_NX10_gpio_motion_gpio_cfg1         , 0x10140404
 .equ Adr_NX10_gpio_gpio_cfg2                , 0x101C0808
 .equ Adr_NX10_gpio_motion_gpio_cfg2         , 0x10140408
 .equ Adr_NX10_gpio_gpio_cfg3                , 0x101C080C
 .equ Adr_NX10_gpio_motion_gpio_cfg3         , 0x1014040C
 .equ Adr_NX10_gpio_gpio_cfg4                , 0x101C0810
 .equ Adr_NX10_gpio_motion_gpio_cfg4         , 0x10140410
 .equ Adr_NX10_gpio_gpio_cfg5                , 0x101C0814
 .equ Adr_NX10_gpio_motion_gpio_cfg5         , 0x10140414
 .equ Adr_NX10_gpio_gpio_cfg6                , 0x101C0818
 .equ Adr_NX10_gpio_motion_gpio_cfg6         , 0x10140418
 .equ Adr_NX10_gpio_gpio_cfg7                , 0x101C081C
 .equ Adr_NX10_gpio_motion_gpio_cfg7         , 0x1014041C
 .equ Adr_NX10_gpio_gpio_tc0                 , 0x101C0820
 .equ Adr_NX10_gpio_motion_gpio_tc0          , 0x10140420
 .equ Adr_NX10_gpio_gpio_tc1                 , 0x101C0824
 .equ Adr_NX10_gpio_motion_gpio_tc1          , 0x10140424
 .equ Adr_NX10_gpio_gpio_tc2                 , 0x101C0828
 .equ Adr_NX10_gpio_motion_gpio_tc2          , 0x10140428
 .equ Adr_NX10_gpio_gpio_tc3                 , 0x101C082C
 .equ Adr_NX10_gpio_motion_gpio_tc3          , 0x1014042C
 .equ Adr_NX10_gpio_gpio_tc4                 , 0x101C0830
 .equ Adr_NX10_gpio_motion_gpio_tc4          , 0x10140430
 .equ Adr_NX10_gpio_gpio_tc5                 , 0x101C0834
 .equ Adr_NX10_gpio_motion_gpio_tc5          , 0x10140434
 .equ Adr_NX10_gpio_gpio_tc6                 , 0x101C0838
 .equ Adr_NX10_gpio_motion_gpio_tc6          , 0x10140438
 .equ Adr_NX10_gpio_gpio_tc7                 , 0x101C083C
 .equ Adr_NX10_gpio_motion_gpio_tc7          , 0x1014043C
 .equ Adr_NX10_gpio_gpio_counter0_ctrl       , 0x101C0840
 .equ Adr_NX10_gpio_motion_gpio_counter0_ctrl, 0x10140440
 .equ Adr_NX10_gpio_gpio_counter1_ctrl       , 0x101C0844
 .equ Adr_NX10_gpio_motion_gpio_counter1_ctrl, 0x10140444
 .equ Adr_NX10_gpio_gpio_counter0_max        , 0x101C0848
 .equ Adr_NX10_gpio_motion_gpio_counter0_max , 0x10140448
 .equ Adr_NX10_gpio_gpio_counter1_max        , 0x101C084C
 .equ Adr_NX10_gpio_motion_gpio_counter1_max , 0x1014044C
 .equ Adr_NX10_gpio_gpio_counter0_cnt        , 0x101C0850
 .equ Adr_NX10_gpio_motion_gpio_counter0_cnt , 0x10140450
 .equ Adr_NX10_gpio_gpio_counter1_cnt        , 0x101C0854
 .equ Adr_NX10_gpio_motion_gpio_counter1_cnt , 0x10140454
 .equ Adr_NX10_gpio_gpio_line                , 0x101C0858
 .equ Adr_NX10_gpio_motion_gpio_line         , 0x10140458
 .equ Adr_NX10_gpio_gpio_in                  , 0x101C085C
 .equ Adr_NX10_gpio_motion_gpio_in           , 0x1014045C
 .equ Adr_NX10_gpio_gpio_irq_raw             , 0x101C0860
 .equ Adr_NX10_gpio_motion_gpio_irq_raw      , 0x10140460
 .equ Adr_NX10_gpio_gpio_irq_masked          , 0x101C0864
 .equ Adr_NX10_gpio_motion_gpio_irq_masked   , 0x10140464
 .equ Adr_NX10_gpio_gpio_irq_mask_set        , 0x101C0868
 .equ Adr_NX10_gpio_motion_gpio_irq_mask_set , 0x10140468
 .equ Adr_NX10_gpio_gpio_irq_mask_rst        , 0x101C086C
 .equ Adr_NX10_gpio_motion_gpio_irq_mask_rst , 0x1014046C
 .equ Adr_NX10_gpio_cnt_irq_raw              , 0x101C0870
 .equ Adr_NX10_gpio_motion_cnt_irq_raw       , 0x10140470
 .equ Adr_NX10_gpio_cnt_irq_masked           , 0x101C0874
 .equ Adr_NX10_gpio_motion_cnt_irq_masked    , 0x10140474
 .equ Adr_NX10_gpio_cnt_irq_mask_set         , 0x101C0878
 .equ Adr_NX10_gpio_motion_cnt_irq_mask_set  , 0x10140478
 .equ Adr_NX10_gpio_cnt_irq_mask_rst         , 0x101C087C
 .equ Adr_NX10_gpio_motion_cnt_irq_mask_rst  , 0x1014047C

@ =====================================================================
@
@ Area arm_timer
 .equ Addr_NX10_arm_timer, 0x101C0900
@
@ =====================================================================
 .equ Adr_NX10_arm_timer_arm_timer_config_timer0     , 0x101C0900
 .equ Adr_NX10_arm_timer_config_timer0               , 0x101C0900
 .equ Adr_NX10_arm_timer_arm_timer_config_timer1     , 0x101C0904
 .equ Adr_NX10_arm_timer_config_timer1               , 0x101C0904
 .equ Adr_NX10_arm_timer_arm_timer_preload_timer0    , 0x101C0908
 .equ Adr_NX10_arm_timer_preload_timer0              , 0x101C0908
 .equ Adr_NX10_arm_timer_arm_timer_preload_timer1    , 0x101C090C
 .equ Adr_NX10_arm_timer_preload_timer1              , 0x101C090C
 .equ Adr_NX10_arm_timer_arm_timer_timer0            , 0x101C0910
 .equ Adr_NX10_arm_timer_timer0                      , 0x101C0910
 .equ Adr_NX10_arm_timer_arm_timer_timer1            , 0x101C0914
 .equ Adr_NX10_arm_timer_timer1                      , 0x101C0914
 .equ Adr_NX10_arm_timer_arm_timer_systime_s         , 0x101C0918
 .equ Adr_NX10_arm_timer_systime_s                   , 0x101C0918
 .equ Adr_NX10_arm_timer_arm_timer_systime_ns        , 0x101C091C
 .equ Adr_NX10_arm_timer_systime_ns                  , 0x101C091C
 .equ Adr_NX10_arm_timer_arm_timer_systime_ns_compare, 0x101C0920
 .equ Adr_NX10_arm_timer_systime_ns_compare          , 0x101C0920
 .equ Adr_NX10_arm_timer_arm_timer_systime_s_compare , 0x101C0924
 .equ Adr_NX10_arm_timer_systime_s_compare           , 0x101C0924
 .equ Adr_NX10_arm_timer_arm_timer_irq_raw           , 0x101C0928
 .equ Adr_NX10_arm_timer_irq_raw                     , 0x101C0928
 .equ Adr_NX10_arm_timer_arm_timer_irq_masked        , 0x101C092C
 .equ Adr_NX10_arm_timer_irq_masked                  , 0x101C092C
 .equ Adr_NX10_arm_timer_arm_timer_irq_msk_set       , 0x101C0930
 .equ Adr_NX10_arm_timer_irq_msk_set                 , 0x101C0930
 .equ Adr_NX10_arm_timer_arm_timer_irq_msk_reset     , 0x101C0934
 .equ Adr_NX10_arm_timer_irq_msk_reset               , 0x101C0934

@ =====================================================================
@
@ Area mmio_ctrl
 .equ Addr_NX10_mmio_ctrl, 0x101C0A00
@
@ =====================================================================
 .equ Adr_NX10_mmio_ctrl_mmio0_cfg            , 0x101C0A00
 .equ Adr_NX10_mmio0_cfg                      , 0x101C0A00
 .equ Adr_NX10_mmio_ctrl_mmio1_cfg            , 0x101C0A04
 .equ Adr_NX10_mmio1_cfg                      , 0x101C0A04
 .equ Adr_NX10_mmio_ctrl_mmio2_cfg            , 0x101C0A08
 .equ Adr_NX10_mmio2_cfg                      , 0x101C0A08
 .equ Adr_NX10_mmio_ctrl_mmio3_cfg            , 0x101C0A0C
 .equ Adr_NX10_mmio3_cfg                      , 0x101C0A0C
 .equ Adr_NX10_mmio_ctrl_mmio4_cfg            , 0x101C0A10
 .equ Adr_NX10_mmio4_cfg                      , 0x101C0A10
 .equ Adr_NX10_mmio_ctrl_mmio5_cfg            , 0x101C0A14
 .equ Adr_NX10_mmio5_cfg                      , 0x101C0A14
 .equ Adr_NX10_mmio_ctrl_mmio6_cfg            , 0x101C0A18
 .equ Adr_NX10_mmio6_cfg                      , 0x101C0A18
 .equ Adr_NX10_mmio_ctrl_mmio7_cfg            , 0x101C0A1C
 .equ Adr_NX10_mmio7_cfg                      , 0x101C0A1C
 .equ Adr_NX10_mmio_ctrl_mmio8_cfg            , 0x101C0A20
 .equ Adr_NX10_mmio8_cfg                      , 0x101C0A20
 .equ Adr_NX10_mmio_ctrl_mmio9_cfg            , 0x101C0A24
 .equ Adr_NX10_mmio9_cfg                      , 0x101C0A24
 .equ Adr_NX10_mmio_ctrl_mmio10_cfg           , 0x101C0A28
 .equ Adr_NX10_mmio10_cfg                     , 0x101C0A28
 .equ Adr_NX10_mmio_ctrl_mmio11_cfg           , 0x101C0A2C
 .equ Adr_NX10_mmio11_cfg                     , 0x101C0A2C
 .equ Adr_NX10_mmio_ctrl_mmio12_cfg           , 0x101C0A30
 .equ Adr_NX10_mmio12_cfg                     , 0x101C0A30
 .equ Adr_NX10_mmio_ctrl_mmio13_cfg           , 0x101C0A34
 .equ Adr_NX10_mmio13_cfg                     , 0x101C0A34
 .equ Adr_NX10_mmio_ctrl_mmio14_cfg           , 0x101C0A38
 .equ Adr_NX10_mmio14_cfg                     , 0x101C0A38
 .equ Adr_NX10_mmio_ctrl_mmio15_cfg           , 0x101C0A3C
 .equ Adr_NX10_mmio15_cfg                     , 0x101C0A3C
 .equ Adr_NX10_mmio_ctrl_mmio16_cfg           , 0x101C0A40
 .equ Adr_NX10_mmio16_cfg                     , 0x101C0A40
 .equ Adr_NX10_mmio_ctrl_mmio17_cfg           , 0x101C0A44
 .equ Adr_NX10_mmio17_cfg                     , 0x101C0A44
 .equ Adr_NX10_mmio_ctrl_mmio18_cfg           , 0x101C0A48
 .equ Adr_NX10_mmio18_cfg                     , 0x101C0A48
 .equ Adr_NX10_mmio_ctrl_mmio19_cfg           , 0x101C0A4C
 .equ Adr_NX10_mmio19_cfg                     , 0x101C0A4C
 .equ Adr_NX10_mmio_ctrl_mmio20_cfg           , 0x101C0A50
 .equ Adr_NX10_mmio20_cfg                     , 0x101C0A50
 .equ Adr_NX10_mmio_ctrl_mmio21_cfg           , 0x101C0A54
 .equ Adr_NX10_mmio21_cfg                     , 0x101C0A54
 .equ Adr_NX10_mmio_ctrl_mmio22_cfg           , 0x101C0A58
 .equ Adr_NX10_mmio22_cfg                     , 0x101C0A58
 .equ Adr_NX10_mmio_ctrl_mmio23_cfg           , 0x101C0A5C
 .equ Adr_NX10_mmio23_cfg                     , 0x101C0A5C
 .equ Adr_NX10_mmio_ctrl_mmio_pio_out_line_cfg, 0x101C0A60
 .equ Adr_NX10_mmio_pio_out_line_cfg          , 0x101C0A60
 .equ Adr_NX10_mmio_ctrl_mmio_pio_oe_line_cfg , 0x101C0A64
 .equ Adr_NX10_mmio_pio_oe_line_cfg           , 0x101C0A64
 .equ Adr_NX10_mmio_ctrl_mmio_in_line_status  , 0x101C0A68
 .equ Adr_NX10_mmio_in_line_status            , 0x101C0A68
 .equ Adr_NX10_mmio_ctrl_mmio_is_pio_status   , 0x101C0A6C
 .equ Adr_NX10_mmio_is_pio_status             , 0x101C0A6C

@ =====================================================================
@
@ Area uart
 .equ Addr_NX10_uart0, 0x101C0B00
 .equ Addr_NX10_uart1, 0x101C0B40
@
@ =====================================================================
 .equ Adr_NX10_uart0_uartdr      , 0x101C0B00
 .equ Adr_NX10_uart1_uartdr      , 0x101C0B40
 .equ Adr_NX10_uart0_uartrsr     , 0x101C0B04
 .equ Adr_NX10_uart1_uartrsr     , 0x101C0B44
 .equ Adr_NX10_uart0_uartlcr_h   , 0x101C0B08
 .equ Adr_NX10_uart1_uartlcr_h   , 0x101C0B48
 .equ Adr_NX10_uart0_uartlcr_m   , 0x101C0B0C
 .equ Adr_NX10_uart1_uartlcr_m   , 0x101C0B4C
 .equ Adr_NX10_uart0_uartlcr_l   , 0x101C0B10
 .equ Adr_NX10_uart1_uartlcr_l   , 0x101C0B50
 .equ Adr_NX10_uart0_uartcr      , 0x101C0B14
 .equ Adr_NX10_uart1_uartcr      , 0x101C0B54
 .equ Adr_NX10_uart0_uartfr      , 0x101C0B18
 .equ Adr_NX10_uart1_uartfr      , 0x101C0B58
 .equ Adr_NX10_uart0_uartiir     , 0x101C0B1C
 .equ Adr_NX10_uart1_uartiir     , 0x101C0B5C
 .equ Adr_NX10_uart0_uartilpr    , 0x101C0B20
 .equ Adr_NX10_uart1_uartilpr    , 0x101C0B60
 .equ Adr_NX10_uart0_uartrts     , 0x101C0B24
 .equ Adr_NX10_uart1_uartrts     , 0x101C0B64
 .equ Adr_NX10_uart0_uartforerun , 0x101C0B28
 .equ Adr_NX10_uart1_uartforerun , 0x101C0B68
 .equ Adr_NX10_uart0_uarttrail   , 0x101C0B2C
 .equ Adr_NX10_uart1_uarttrail   , 0x101C0B6C
 .equ Adr_NX10_uart0_uartdrvout  , 0x101C0B30
 .equ Adr_NX10_uart1_uartdrvout  , 0x101C0B70
 .equ Adr_NX10_uart0_uartcr_2    , 0x101C0B34
 .equ Adr_NX10_uart1_uartcr_2    , 0x101C0B74
 .equ Adr_NX10_uart0_uartrxiflsel, 0x101C0B38
 .equ Adr_NX10_uart1_uartrxiflsel, 0x101C0B78
 .equ Adr_NX10_uart0_uarttxiflsel, 0x101C0B3C
 .equ Adr_NX10_uart1_uarttxiflsel, 0x101C0B7C

@ =====================================================================
@
@ Area miimu
 .equ Addr_NX10_miimu, 0x101C0C00
@
@ =====================================================================
 .equ Adr_NX10_miimu_miimu_reg    , 0x101C0C00
 .equ Adr_NX10_miimu_reg          , 0x101C0C00
 .equ Adr_NX10_miimu_miimu_sw_en  , 0x101C0C04
 .equ Adr_NX10_miimu_sw_en        , 0x101C0C04
 .equ Adr_NX10_miimu_miimu_sw_mdc , 0x101C0C08
 .equ Adr_NX10_miimu_sw_mdc       , 0x101C0C08
 .equ Adr_NX10_miimu_miimu_sw_mdo , 0x101C0C0C
 .equ Adr_NX10_miimu_sw_mdo       , 0x101C0C0C
 .equ Adr_NX10_miimu_miimu_sw_mdoe, 0x101C0C10
 .equ Adr_NX10_miimu_sw_mdoe      , 0x101C0C10
 .equ Adr_NX10_miimu_miimu_sw_mdi , 0x101C0C14
 .equ Adr_NX10_miimu_sw_mdi       , 0x101C0C14

@ =====================================================================
@
@ Area hif_io_ctrl
 .equ Addr_NX10_hif_io_ctrl, 0x101C0C40
@
@ =====================================================================
 .equ Adr_NX10_hif_io_ctrl_hif_io_cfg  , 0x101C0C40
 .equ Adr_NX10_hif_io_cfg              , 0x101C0C40
 .equ Adr_NX10_hif_io_ctrl_hif_pio_out0, 0x101C0C44
 .equ Adr_NX10_hif_pio_out0            , 0x101C0C44
 .equ Adr_NX10_hif_io_ctrl_hif_pio_out1, 0x101C0C48
 .equ Adr_NX10_hif_pio_out1            , 0x101C0C48
 .equ Adr_NX10_hif_io_ctrl_hif_pio_oe0 , 0x101C0C4C
 .equ Adr_NX10_hif_pio_oe0             , 0x101C0C4C
 .equ Adr_NX10_hif_io_ctrl_hif_pio_oe1 , 0x101C0C50
 .equ Adr_NX10_hif_pio_oe1             , 0x101C0C50
 .equ Adr_NX10_hif_io_ctrl_hif_pio_in0 , 0x101C0C54
 .equ Adr_NX10_hif_pio_in0             , 0x101C0C54
 .equ Adr_NX10_hif_io_ctrl_hif_pio_in1 , 0x101C0C58
 .equ Adr_NX10_hif_pio_in1             , 0x101C0C58

@ =====================================================================
@
@ Area sqi
 .equ Addr_NX10_sqi, 0x101C0D00
@
@ =====================================================================
 .equ Adr_NX10_sqi_sqi_cr0        , 0x101C0D00
 .equ Adr_NX10_sqi_cr0            , 0x101C0D00
 .equ Adr_NX10_sqi_sqi_cr1        , 0x101C0D04
 .equ Adr_NX10_sqi_cr1            , 0x101C0D04
 .equ Adr_NX10_sqi_sqi_dr         , 0x101C0D08
 .equ Adr_NX10_sqi_dr             , 0x101C0D08
 .equ Adr_NX10_sqi_sqi_sr         , 0x101C0D0C
 .equ Adr_NX10_sqi_sr             , 0x101C0D0C
 .equ Adr_NX10_sqi_sqi_tcr        , 0x101C0D10
 .equ Adr_NX10_sqi_tcr            , 0x101C0D10
 .equ Adr_NX10_sqi_sqi_irq_mask   , 0x101C0D14
 .equ Adr_NX10_sqi_irq_mask       , 0x101C0D14
 .equ Adr_NX10_sqi_sqi_irq_raw    , 0x101C0D18
 .equ Adr_NX10_sqi_irq_raw        , 0x101C0D18
 .equ Adr_NX10_sqi_sqi_irq_masked , 0x101C0D1C
 .equ Adr_NX10_sqi_irq_masked     , 0x101C0D1C
 .equ Adr_NX10_sqi_sqi_irq_clear  , 0x101C0D20
 .equ Adr_NX10_sqi_irq_clear      , 0x101C0D20
 .equ Adr_NX10_sqi_sqi_irq_cpu_sel, 0x101C0D24
 .equ Adr_NX10_sqi_irq_cpu_sel    , 0x101C0D24
 .equ Adr_NX10_sqi_sqi_dmacr      , 0x101C0D28
 .equ Adr_NX10_sqi_dmacr          , 0x101C0D28
 .equ Adr_NX10_sqi_sqi_pio_out    , 0x101C0D30
 .equ Adr_NX10_sqi_pio_out        , 0x101C0D30
 .equ Adr_NX10_sqi_sqi_pio_oe     , 0x101C0D34
 .equ Adr_NX10_sqi_pio_oe         , 0x101C0D34
 .equ Adr_NX10_sqi_sqi_pio_in     , 0x101C0D38
 .equ Adr_NX10_sqi_pio_in         , 0x101C0D38
 .equ Adr_NX10_sqi_sqi_sqirom_cfg , 0x101C0D3C
 .equ Adr_NX10_sqi_sqirom_cfg     , 0x101C0D3C

@ =====================================================================
@
@ Area i2c
 .equ Addr_NX10_i2c, 0x101C0D40
@
@ =====================================================================
 .equ Adr_NX10_i2c_i2c_mcr     , 0x101C0D40
 .equ Adr_NX10_i2c_mcr         , 0x101C0D40
 .equ Adr_NX10_i2c_i2c_scr     , 0x101C0D44
 .equ Adr_NX10_i2c_scr         , 0x101C0D44
 .equ Adr_NX10_i2c_i2c_cmd     , 0x101C0D48
 .equ Adr_NX10_i2c_cmd         , 0x101C0D48
 .equ Adr_NX10_i2c_i2c_mdr     , 0x101C0D4C
 .equ Adr_NX10_i2c_mdr         , 0x101C0D4C
 .equ Adr_NX10_i2c_i2c_sdr     , 0x101C0D50
 .equ Adr_NX10_i2c_sdr         , 0x101C0D50
 .equ Adr_NX10_i2c_i2c_mfifo_cr, 0x101C0D54
 .equ Adr_NX10_i2c_mfifo_cr    , 0x101C0D54
 .equ Adr_NX10_i2c_i2c_sfifo_cr, 0x101C0D58
 .equ Adr_NX10_i2c_sfifo_cr    , 0x101C0D58
 .equ Adr_NX10_i2c_i2c_sr      , 0x101C0D5C
 .equ Adr_NX10_i2c_sr          , 0x101C0D5C
 .equ Adr_NX10_i2c_i2c_irqmsk  , 0x101C0D60
 .equ Adr_NX10_i2c_irqmsk      , 0x101C0D60
 .equ Adr_NX10_i2c_i2c_irqsr   , 0x101C0D64
 .equ Adr_NX10_i2c_irqsr       , 0x101C0D64
 .equ Adr_NX10_i2c_i2c_irqmsked, 0x101C0D68
 .equ Adr_NX10_i2c_irqmsked    , 0x101C0D68
 .equ Adr_NX10_i2c_i2c_dmacr   , 0x101C0D6C
 .equ Adr_NX10_i2c_dmacr       , 0x101C0D6C
 .equ Adr_NX10_i2c_i2c_pio     , 0x101C0D70
 .equ Adr_NX10_i2c_pio         , 0x101C0D70

@ =====================================================================
@
@ Area usb_dev
 .equ Addr_NX10_usb_dev, 0x101C0E00
@
@ =====================================================================
@ =====================================================================
@
@ Area usb_dev_ctrl
 .equ Addr_NX10_usb_dev_ctrl, 0x101C0E00
@
@ =====================================================================
 .equ Adr_NX10_usb_dev_ctrl_usb_dev_cfg            , 0x101C0E00
 .equ Adr_NX10_usb_dev_cfg                         , 0x101C0E00
 .equ Adr_NX10_usb_dev_ctrl_usb_dev_status         , 0x101C0E04
 .equ Adr_NX10_usb_dev_status                      , 0x101C0E04
 .equ Adr_NX10_usb_dev_ctrl_usb_dev_vendor_features, 0x101C0E08
 .equ Adr_NX10_usb_dev_vendor_features             , 0x101C0E08
 .equ Adr_NX10_usb_dev_ctrl_usb_dev_irq_mask       , 0x101C0E0C
 .equ Adr_NX10_usb_dev_irq_mask                    , 0x101C0E0C
 .equ Adr_NX10_usb_dev_ctrl_usb_dev_irq_raw        , 0x101C0E10
 .equ Adr_NX10_usb_dev_irq_raw                     , 0x101C0E10
 .equ Adr_NX10_usb_dev_ctrl_usb_dev_irq_masked     , 0x101C0E14
 .equ Adr_NX10_usb_dev_irq_masked                  , 0x101C0E14

@ =====================================================================
@
@ Area usb_dev_enum_ram
 .equ Addr_NX10_usb_dev_enum_ram, 0x101C0E40
@
@ =====================================================================
 .equ Adr_NX10_usb_dev_enum_ram_usb_dev_enum_ram_descriptors_base       , 0x101C0E40
 .equ Adr_NX10_usb_dev_enum_ram_descriptors_base                        , 0x101C0E40
 .equ Adr_NX10_usb_dev_enum_ram_usb_dev_enum_ram_descriptors_end        , 0x101C0E44
 .equ Adr_NX10_usb_dev_enum_ram_descriptors_end                         , 0x101C0E44
 .equ Adr_NX10_usb_dev_enum_ram_usb_dev_enum_ram_string_descriptors_base, 0x101C0E48
 .equ Adr_NX10_usb_dev_enum_ram_string_descriptors_base                 , 0x101C0E48
 .equ Adr_NX10_usb_dev_enum_ram_usb_dev_enum_ram_string_descriptors_end , 0x101C0E7C
 .equ Adr_NX10_usb_dev_enum_ram_string_descriptors_end                  , 0x101C0E7C

@ =====================================================================
@
@ Area usb_dev_fifo_ctrl
 .equ Addr_NX10_usb_dev_fifo_ctrl, 0x101C0E80
@
@ =====================================================================
 .equ Adr_NX10_usb_dev_fifo_ctrl_usb_dev_fifo_ctrl_conf         , 0x101C0E80
 .equ Adr_NX10_usb_dev_fifo_ctrl_conf                           , 0x101C0E80
 .equ Adr_NX10_usb_dev_fifo_ctrl_usb_dev_fifo_ctrl_out_handshake, 0x101C0E84
 .equ Adr_NX10_usb_dev_fifo_ctrl_out_handshake                  , 0x101C0E84
 .equ Adr_NX10_usb_dev_fifo_ctrl_usb_dev_fifo_ctrl_in_handshake , 0x101C0E88
 .equ Adr_NX10_usb_dev_fifo_ctrl_in_handshake                   , 0x101C0E88
 .equ Adr_NX10_usb_dev_fifo_ctrl_usb_dev_fifo_ctrl_status0      , 0x101C0E8C
 .equ Adr_NX10_usb_dev_fifo_ctrl_status0                        , 0x101C0E8C
 .equ Adr_NX10_usb_dev_fifo_ctrl_usb_dev_fifo_ctrl_status1      , 0x101C0E90
 .equ Adr_NX10_usb_dev_fifo_ctrl_status1                        , 0x101C0E90
 .equ Adr_NX10_usb_dev_fifo_ctrl_usb_dev_fifo_ctrl_status2      , 0x101C0E94
 .equ Adr_NX10_usb_dev_fifo_ctrl_status2                        , 0x101C0E94
 .equ Adr_NX10_usb_dev_fifo_ctrl_usb_dev_fifo_ctrl_status3      , 0x101C0E98
 .equ Adr_NX10_usb_dev_fifo_ctrl_status3                        , 0x101C0E98
 .equ Adr_NX10_usb_dev_fifo_ctrl_usb_dev_fifo_ctrl_status4      , 0x101C0E9C
 .equ Adr_NX10_usb_dev_fifo_ctrl_status4                        , 0x101C0E9C
 .equ Adr_NX10_usb_dev_fifo_ctrl_usb_dev_fifo_ctrl_status5      , 0x101C0EA0
 .equ Adr_NX10_usb_dev_fifo_ctrl_status5                        , 0x101C0EA0
 .equ Adr_NX10_usb_dev_fifo_ctrl_usb_dev_fifo_ctrl_status6      , 0x101C0EA4
 .equ Adr_NX10_usb_dev_fifo_ctrl_status6                        , 0x101C0EA4

@ =====================================================================
@
@ Area usb_dev_fifo
 .equ Addr_NX10_usb_dev_fifo, 0x101C0EC0
@
@ =====================================================================
 .equ Adr_NX10_usb_dev_fifo_usb_dev_fifo0, 0x101C0EC0
 .equ Adr_NX10_usb_dev_fifo0             , 0x101C0EC0
 .equ Adr_NX10_usb_dev_fifo_usb_dev_fifo1, 0x101C0EC4
 .equ Adr_NX10_usb_dev_fifo1             , 0x101C0EC4
 .equ Adr_NX10_usb_dev_fifo_usb_dev_fifo2, 0x101C0EC8
 .equ Adr_NX10_usb_dev_fifo2             , 0x101C0EC8
 .equ Adr_NX10_usb_dev_fifo_usb_dev_fifo3, 0x101C0ECC
 .equ Adr_NX10_usb_dev_fifo3             , 0x101C0ECC
 .equ Adr_NX10_usb_dev_fifo_usb_dev_fifo4, 0x101C0ED0
 .equ Adr_NX10_usb_dev_fifo4             , 0x101C0ED0
 .equ Adr_NX10_usb_dev_fifo_usb_dev_fifo5, 0x101C0ED4
 .equ Adr_NX10_usb_dev_fifo5             , 0x101C0ED4
 .equ Adr_NX10_usb_dev_fifo_usb_dev_fifo6, 0x101C0ED8
 .equ Adr_NX10_usb_dev_fifo6             , 0x101C0ED8

@ =====================================================================
@
@ Area systime
 .equ Addr_NX10_systime, 0x101C1000
@
@ =====================================================================
 .equ Adr_NX10_systime_systime_s          , 0x101C1000
 .equ Adr_NX10_systime_s                  , 0x101C1000
 .equ Adr_NX10_systime_systime_ns         , 0x101C1004
 .equ Adr_NX10_systime_ns                 , 0x101C1004
 .equ Adr_NX10_systime_systime_border     , 0x101C1008
 .equ Adr_NX10_systime_border             , 0x101C1008
 .equ Adr_NX10_systime_systime_count_value, 0x101C100C
 .equ Adr_NX10_systime_count_value        , 0x101C100C

@ =====================================================================
@
@ Area handshake_ctrl
 .equ Addr_NX10_handshake_ctrl, 0x101C1100
@
@ =====================================================================
 .equ Adr_NX10_handshake_ctrl_handshake_base_addr                , 0x101C1100
 .equ Adr_NX10_handshake_base_addr                               , 0x101C1100
 .equ Adr_NX10_handshake_ctrl_handshake_dpm_irq_raw_clear        , 0x101C1110
 .equ Adr_NX10_handshake_dpm_irq_raw_clear                       , 0x101C1110
 .equ Adr_NX10_handshake_ctrl_handshake_dpm_irq_masked           , 0x101C1114
 .equ Adr_NX10_handshake_dpm_irq_masked                          , 0x101C1114
 .equ Adr_NX10_handshake_ctrl_handshake_dpm_irq_msk_set          , 0x101C1118
 .equ Adr_NX10_handshake_dpm_irq_msk_set                         , 0x101C1118
 .equ Adr_NX10_handshake_ctrl_handshake_dpm_irq_msk_reset        , 0x101C111C
 .equ Adr_NX10_handshake_dpm_irq_msk_reset                       , 0x101C111C
 .equ Adr_NX10_handshake_ctrl_handshake_arm_irq_raw_clear        , 0x101C1120
 .equ Adr_NX10_handshake_arm_irq_raw_clear                       , 0x101C1120
 .equ Adr_NX10_handshake_ctrl_handshake_arm_irq_masked           , 0x101C1124
 .equ Adr_NX10_handshake_arm_irq_masked                          , 0x101C1124
 .equ Adr_NX10_handshake_ctrl_handshake_arm_irq_msk_set          , 0x101C1128
 .equ Adr_NX10_handshake_arm_irq_msk_set                         , 0x101C1128
 .equ Adr_NX10_handshake_ctrl_handshake_arm_irq_msk_reset        , 0x101C112C
 .equ Adr_NX10_handshake_arm_irq_msk_reset                       , 0x101C112C
 .equ Adr_NX10_handshake_ctrl_handshake_xpic_irq_raw_clear       , 0x101C1130
 .equ Adr_NX10_handshake_xpic_irq_raw_clear                      , 0x101C1130
 .equ Adr_NX10_handshake_ctrl_handshake_xpic_irq_masked          , 0x101C1134
 .equ Adr_NX10_handshake_xpic_irq_masked                         , 0x101C1134
 .equ Adr_NX10_handshake_ctrl_handshake_xpic_irq_msk_set         , 0x101C1138
 .equ Adr_NX10_handshake_xpic_irq_msk_set                        , 0x101C1138
 .equ Adr_NX10_handshake_ctrl_handshake_xpic_irq_msk_reset       , 0x101C113C
 .equ Adr_NX10_handshake_xpic_irq_msk_reset                      , 0x101C113C
 .equ Adr_NX10_handshake_ctrl_handshake_hsc0_ctrl                , 0x101C1180
 .equ Adr_NX10_handshake_hsc0_ctrl                               , 0x101C1180
 .equ Adr_NX10_handshake_ctrl_handshake_hsc1_ctrl                , 0x101C1184
 .equ Adr_NX10_handshake_hsc1_ctrl                               , 0x101C1184
 .equ Adr_NX10_handshake_ctrl_handshake_hsc2_ctrl                , 0x101C1188
 .equ Adr_NX10_handshake_hsc2_ctrl                               , 0x101C1188
 .equ Adr_NX10_handshake_ctrl_handshake_hsc3_ctrl                , 0x101C118C
 .equ Adr_NX10_handshake_hsc3_ctrl                               , 0x101C118C
 .equ Adr_NX10_handshake_ctrl_handshake_hsc4_ctrl                , 0x101C1190
 .equ Adr_NX10_handshake_hsc4_ctrl                               , 0x101C1190
 .equ Adr_NX10_handshake_ctrl_handshake_hsc5_ctrl                , 0x101C1194
 .equ Adr_NX10_handshake_hsc5_ctrl                               , 0x101C1194
 .equ Adr_NX10_handshake_ctrl_handshake_hsc6_ctrl                , 0x101C1198
 .equ Adr_NX10_handshake_hsc6_ctrl                               , 0x101C1198
 .equ Adr_NX10_handshake_ctrl_handshake_hsc7_ctrl                , 0x101C119C
 .equ Adr_NX10_handshake_hsc7_ctrl                               , 0x101C119C
 .equ Adr_NX10_handshake_ctrl_handshake_hsc8_ctrl                , 0x101C11A0
 .equ Adr_NX10_handshake_hsc8_ctrl                               , 0x101C11A0
 .equ Adr_NX10_handshake_ctrl_handshake_hsc9_ctrl                , 0x101C11A4
 .equ Adr_NX10_handshake_hsc9_ctrl                               , 0x101C11A4
 .equ Adr_NX10_handshake_ctrl_handshake_hsc10_ctrl               , 0x101C11A8
 .equ Adr_NX10_handshake_hsc10_ctrl                              , 0x101C11A8
 .equ Adr_NX10_handshake_ctrl_handshake_hsc11_ctrl               , 0x101C11AC
 .equ Adr_NX10_handshake_hsc11_ctrl                              , 0x101C11AC
 .equ Adr_NX10_handshake_ctrl_handshake_hsc12_ctrl               , 0x101C11B0
 .equ Adr_NX10_handshake_hsc12_ctrl                              , 0x101C11B0
 .equ Adr_NX10_handshake_ctrl_handshake_hsc13_ctrl               , 0x101C11B4
 .equ Adr_NX10_handshake_hsc13_ctrl                              , 0x101C11B4
 .equ Adr_NX10_handshake_ctrl_handshake_hsc14_ctrl               , 0x101C11B8
 .equ Adr_NX10_handshake_hsc14_ctrl                              , 0x101C11B8
 .equ Adr_NX10_handshake_ctrl_handshake_hsc15_ctrl               , 0x101C11BC
 .equ Adr_NX10_handshake_hsc15_ctrl                              , 0x101C11BC
 .equ Adr_NX10_handshake_ctrl_handshake_buf_man0_ctrl            , 0x101C11C0
 .equ Adr_NX10_handshake_buf_man0_ctrl                           , 0x101C11C0
 .equ Adr_NX10_handshake_ctrl_handshake_buf_man0_status_ctrl_netx, 0x101C11C4
 .equ Adr_NX10_handshake_buf_man0_status_ctrl_netx               , 0x101C11C4
 .equ Adr_NX10_handshake_ctrl_handshake_buf_man0_status_ctrl_host, 0x101C11C8
 .equ Adr_NX10_handshake_buf_man0_status_ctrl_host               , 0x101C11C8
 .equ Adr_NX10_handshake_ctrl_handshake_buf_man0_win_map         , 0x101C11CC
 .equ Adr_NX10_handshake_buf_man0_win_map                        , 0x101C11CC
 .equ Adr_NX10_handshake_ctrl_handshake_buf_man1_ctrl            , 0x101C11D0
 .equ Adr_NX10_handshake_buf_man1_ctrl                           , 0x101C11D0
 .equ Adr_NX10_handshake_ctrl_handshake_buf_man1_status_ctrl_netx, 0x101C11D4
 .equ Adr_NX10_handshake_buf_man1_status_ctrl_netx               , 0x101C11D4
 .equ Adr_NX10_handshake_ctrl_handshake_buf_man1_status_ctrl_host, 0x101C11D8
 .equ Adr_NX10_handshake_buf_man1_status_ctrl_host               , 0x101C11D8
 .equ Adr_NX10_handshake_ctrl_handshake_buf_man1_win_map         , 0x101C11DC
 .equ Adr_NX10_handshake_buf_man1_win_map                        , 0x101C11DC

@ =====================================================================
@
@ Area dpm
 .equ Addr_NX10_dpm, 0x101C1200
@
@ =====================================================================
 .equ Adr_NX10_dpm_dpm_cfg0x0                          , 0x101C1200
 .equ Adr_NX10_dpm_cfg0x0                              , 0x101C1200
 .equ Adr_NX10_dpm_dpm_addr_cfg                        , 0x101C1210
 .equ Adr_NX10_dpm_addr_cfg                            , 0x101C1210
 .equ Adr_NX10_dpm_dpm_timing_cfg                      , 0x101C1214
 .equ Adr_NX10_dpm_timing_cfg                          , 0x101C1214
 .equ Adr_NX10_dpm_dpm_rdy_cfg                         , 0x101C1218
 .equ Adr_NX10_dpm_rdy_cfg                             , 0x101C1218
 .equ Adr_NX10_dpm_dpm_status                          , 0x101C121C
 .equ Adr_NX10_dpm_status                              , 0x101C121C
 .equ Adr_NX10_dpm_dpm_status_err_reset                , 0x101C1220
 .equ Adr_NX10_dpm_status_err_reset                    , 0x101C1220
 .equ Adr_NX10_dpm_dpm_status_err_addr                 , 0x101C1224
 .equ Adr_NX10_dpm_status_err_addr                     , 0x101C1224
 .equ Adr_NX10_dpm_dpm_misc_cfg                        , 0x101C1228
 .equ Adr_NX10_dpm_misc_cfg                            , 0x101C1228
 .equ Adr_NX10_dpm_dpm_io_cfg_misc                     , 0x101C122C
 .equ Adr_NX10_dpm_io_cfg_misc                         , 0x101C122C
 .equ Adr_NX10_dpm_dpm_tunnel_cfg                      , 0x101C1238
 .equ Adr_NX10_dpm_tunnel_cfg                          , 0x101C1238
 .equ Adr_NX10_dpm_dpm_itbaddr                         , 0x101C123C
 .equ Adr_NX10_dpm_itbaddr                             , 0x101C123C
 .equ Adr_NX10_dpm_dpm_win1_end                        , 0x101C1240
 .equ Adr_NX10_dpm_win1_end                            , 0x101C1240
 .equ Adr_NX10_dpm_dpm_win1_map                        , 0x101C1244
 .equ Adr_NX10_dpm_win1_map                            , 0x101C1244
 .equ Adr_NX10_dpm_dpm_win2_end                        , 0x101C1248
 .equ Adr_NX10_dpm_win2_end                            , 0x101C1248
 .equ Adr_NX10_dpm_dpm_win2_map                        , 0x101C124C
 .equ Adr_NX10_dpm_win2_map                            , 0x101C124C
 .equ Adr_NX10_dpm_dpm_win3_end                        , 0x101C1250
 .equ Adr_NX10_dpm_win3_end                            , 0x101C1250
 .equ Adr_NX10_dpm_dpm_win3_map                        , 0x101C1254
 .equ Adr_NX10_dpm_win3_map                            , 0x101C1254
 .equ Adr_NX10_dpm_dpm_win4_end                        , 0x101C1258
 .equ Adr_NX10_dpm_win4_end                            , 0x101C1258
 .equ Adr_NX10_dpm_dpm_win4_map                        , 0x101C125C
 .equ Adr_NX10_dpm_win4_map                            , 0x101C125C
 .equ Adr_NX10_dpm_dpm_irq_raw                         , 0x101C1280
 .equ Adr_NX10_dpm_irq_raw                             , 0x101C1280
 .equ Adr_NX10_dpm_dpm_irq_arm_mask_set                , 0x101C1284
 .equ Adr_NX10_dpm_irq_arm_mask_set                    , 0x101C1284
 .equ Adr_NX10_dpm_dpm_irq_arm_mask_reset              , 0x101C1288
 .equ Adr_NX10_dpm_irq_arm_mask_reset                  , 0x101C1288
 .equ Adr_NX10_dpm_dpm_irq_arm_masked                  , 0x101C128C
 .equ Adr_NX10_dpm_irq_arm_masked                      , 0x101C128C
 .equ Adr_NX10_dpm_dpm_irq_xpic_mask_set               , 0x101C1290
 .equ Adr_NX10_dpm_irq_xpic_mask_set                   , 0x101C1290
 .equ Adr_NX10_dpm_dpm_irq_xpic_mask_reset             , 0x101C1294
 .equ Adr_NX10_dpm_irq_xpic_mask_reset                 , 0x101C1294
 .equ Adr_NX10_dpm_dpm_irq_xpic_masked                 , 0x101C1298
 .equ Adr_NX10_dpm_irq_xpic_masked                     , 0x101C1298
 .equ Adr_NX10_dpm_dpm_irq_fiq_mask_set                , 0x101C129C
 .equ Adr_NX10_dpm_irq_fiq_mask_set                    , 0x101C129C
 .equ Adr_NX10_dpm_dpm_irq_fiq_mask_reset              , 0x101C12A0
 .equ Adr_NX10_dpm_irq_fiq_mask_reset                  , 0x101C12A0
 .equ Adr_NX10_dpm_dpm_irq_fiq_masked                  , 0x101C12A4
 .equ Adr_NX10_dpm_irq_fiq_masked                      , 0x101C12A4
 .equ Adr_NX10_dpm_dpm_irq_irq_mask_set                , 0x101C12A8
 .equ Adr_NX10_dpm_irq_irq_mask_set                    , 0x101C12A8
 .equ Adr_NX10_dpm_dpm_irq_irq_mask_reset              , 0x101C12AC
 .equ Adr_NX10_dpm_irq_irq_mask_reset                  , 0x101C12AC
 .equ Adr_NX10_dpm_dpm_irq_irq_masked                  , 0x101C12B0
 .equ Adr_NX10_dpm_irq_irq_masked                      , 0x101C12B0
 .equ Adr_NX10_dpm_dpm_reserved_netx50_wgd_host_timeout, 0x101C12C0
 .equ Adr_NX10_dpm_reserved_netx50_wgd_host_timeout    , 0x101C12C0
 .equ Adr_NX10_dpm_dpm_reserved_netx50_wgd_host_trigger, 0x101C12C4
 .equ Adr_NX10_dpm_reserved_netx50_wgd_host_trigger    , 0x101C12C4
 .equ Adr_NX10_dpm_dpm_reserved_netx50_wgd_netx_timeout, 0x101C12C8
 .equ Adr_NX10_dpm_reserved_netx50_wgd_netx_timeout    , 0x101C12C8
 .equ Adr_NX10_dpm_dpm_sys_sta_bigend16                , 0x101C12CC
 .equ Adr_NX10_dpm_sys_sta_bigend16                    , 0x101C12CC
 .equ Adr_NX10_dpm_dpm_reserved_netx50_timer_ctrl      , 0x101C12D0
 .equ Adr_NX10_dpm_reserved_netx50_timer_ctrl          , 0x101C12D0
 .equ Adr_NX10_dpm_dpm_reserved_netx50_timer_start_val , 0x101C12D4
 .equ Adr_NX10_dpm_reserved_netx50_timer_start_val     , 0x101C12D4
 .equ Adr_NX10_dpm_dpm_sys_sta                         , 0x101C12D8
 .equ Adr_NX10_dpm_sys_sta                             , 0x101C12D8
 .equ Adr_NX10_dpm_dpm_reset_request                   , 0x101C12DC
 .equ Adr_NX10_dpm_reset_request                       , 0x101C12DC
 .equ Adr_NX10_dpm_dpm_firmware_irq_raw                , 0x101C12E0
 .equ Adr_NX10_dpm_firmware_irq_raw                    , 0x101C12E0
 .equ Adr_NX10_dpm_dpm_firmware_irq_mask               , 0x101C12F0
 .equ Adr_NX10_dpm_firmware_irq_mask                   , 0x101C12F0
 .equ Adr_NX10_dpm_dpm_netx_version_bigend16           , 0x101C12F8
 .equ Adr_NX10_dpm_netx_version_bigend16               , 0x101C12F8
 .equ Adr_NX10_dpm_dpm_netx_version                    , 0x101C12FC
 .equ Adr_NX10_dpm_netx_version                        , 0x101C12FC

@ =====================================================================
@
@ Area dmac
 .equ Addr_NX10_dmac, 0x101C5000
@
@ =====================================================================
@ =====================================================================
@
@ Area dmac_ch
 .equ Addr_NX10_dmac_ch0, 0x101C5100
 .equ Addr_NX10_dmac_ch1, 0x101C5120
 .equ Addr_NX10_dmac_ch2, 0x101C5140
@
@ =====================================================================
 .equ Adr_NX10_dmac_ch0_dmac_chsrc_ad , 0x101C5100
 .equ Adr_NX10_dmac_ch1_dmac_chsrc_ad , 0x101C5120
 .equ Adr_NX10_dmac_ch2_dmac_chsrc_ad , 0x101C5140
 .equ Adr_NX10_dmac_ch0_dmac_chdest_ad, 0x101C5104
 .equ Adr_NX10_dmac_ch1_dmac_chdest_ad, 0x101C5124
 .equ Adr_NX10_dmac_ch2_dmac_chdest_ad, 0x101C5144
 .equ Adr_NX10_dmac_ch0_dmac_chlink   , 0x101C5108
 .equ Adr_NX10_dmac_ch1_dmac_chlink   , 0x101C5128
 .equ Adr_NX10_dmac_ch2_dmac_chlink   , 0x101C5148
 .equ Adr_NX10_dmac_ch0_dmac_chctrl   , 0x101C510C
 .equ Adr_NX10_dmac_ch1_dmac_chctrl   , 0x101C512C
 .equ Adr_NX10_dmac_ch2_dmac_chctrl   , 0x101C514C
 .equ Adr_NX10_dmac_ch0_dmac_chcfg    , 0x101C5110
 .equ Adr_NX10_dmac_ch1_dmac_chcfg    , 0x101C5130
 .equ Adr_NX10_dmac_ch2_dmac_chcfg    , 0x101C5150

@ =====================================================================
@
@ Area dmac_reg
 .equ Addr_NX10_dmac_reg, 0x101C5800
@
@ =====================================================================
 .equ Adr_NX10_dmac_reg_dmac_int_status      , 0x101C5800
 .equ Adr_NX10_dmac_int_status               , 0x101C5800
 .equ Adr_NX10_dmac_reg_dmac_inttc_status    , 0x101C5804
 .equ Adr_NX10_dmac_inttc_status             , 0x101C5804
 .equ Adr_NX10_dmac_reg_dmac_inttc_clear     , 0x101C5808
 .equ Adr_NX10_dmac_inttc_clear              , 0x101C5808
 .equ Adr_NX10_dmac_reg_dmac_interr_status   , 0x101C580C
 .equ Adr_NX10_dmac_interr_status            , 0x101C580C
 .equ Adr_NX10_dmac_reg_dmac_interr_clear    , 0x101C5810
 .equ Adr_NX10_dmac_interr_clear             , 0x101C5810
 .equ Adr_NX10_dmac_reg_dmac_rawinttc_status , 0x101C5814
 .equ Adr_NX10_dmac_rawinttc_status          , 0x101C5814
 .equ Adr_NX10_dmac_reg_dmac_rawinterr_status, 0x101C5818
 .equ Adr_NX10_dmac_rawinterr_status         , 0x101C5818
 .equ Adr_NX10_dmac_reg_dmac_enabled_channel , 0x101C581C
 .equ Adr_NX10_dmac_enabled_channel          , 0x101C581C
 .equ Adr_NX10_dmac_reg_dmac_softb_req       , 0x101C5820
 .equ Adr_NX10_dmac_softb_req                , 0x101C5820
 .equ Adr_NX10_dmac_reg_dmac_softs_req       , 0x101C5824
 .equ Adr_NX10_dmac_softs_req                , 0x101C5824
 .equ Adr_NX10_dmac_reg_dmac_softlb_req      , 0x101C5828
 .equ Adr_NX10_dmac_softlb_req               , 0x101C5828
 .equ Adr_NX10_dmac_reg_dmac_softls_req      , 0x101C582C
 .equ Adr_NX10_dmac_softls_req               , 0x101C582C
 .equ Adr_NX10_dmac_reg_dmac_config          , 0x101C5830
 .equ Adr_NX10_dmac_config                   , 0x101C5830
 .equ Adr_NX10_dmac_reg_dmac_sync            , 0x101C5834
 .equ Adr_NX10_dmac_sync                     , 0x101C5834

@ =====================================================================
@
@ Area vic
 .equ Addr_NX10_vic, 0x101FF000
@
@ =====================================================================
 .equ Adr_NX10_vic_vic_irq_status   , 0x101FF000
 .equ Adr_NX10_vic_irq_status       , 0x101FF000
 .equ Adr_NX10_vic_vic_fiq_status   , 0x101FF004
 .equ Adr_NX10_vic_fiq_status       , 0x101FF004
 .equ Adr_NX10_vic_vic_raw_intr     , 0x101FF008
 .equ Adr_NX10_vic_raw_intr         , 0x101FF008
 .equ Adr_NX10_vic_vic_int_select   , 0x101FF00C
 .equ Adr_NX10_vic_int_select       , 0x101FF00C
 .equ Adr_NX10_vic_vic_int_enable   , 0x101FF010
 .equ Adr_NX10_vic_int_enable       , 0x101FF010
 .equ Adr_NX10_vic_vic_int_enclear  , 0x101FF014
 .equ Adr_NX10_vic_int_enclear      , 0x101FF014
 .equ Adr_NX10_vic_vic_softint      , 0x101FF018
 .equ Adr_NX10_vic_softint          , 0x101FF018
 .equ Adr_NX10_vic_vic_softint_clear, 0x101FF01C
 .equ Adr_NX10_vic_softint_clear    , 0x101FF01C
 .equ Adr_NX10_vic_vic_protection   , 0x101FF020
 .equ Adr_NX10_vic_protection       , 0x101FF020
 .equ Adr_NX10_vic_vic_vect_addr    , 0x101FF030
 .equ Adr_NX10_vic_vect_addr        , 0x101FF030
 .equ Adr_NX10_vic_vic_def_vect_addr, 0x101FF034
 .equ Adr_NX10_vic_def_vect_addr    , 0x101FF034
 .equ Adr_NX10_vic_vic_vect_addr0   , 0x101FF100
 .equ Adr_NX10_vic_vect_addr0       , 0x101FF100
 .equ Adr_NX10_vic_vic_vect_addr1   , 0x101FF104
 .equ Adr_NX10_vic_vect_addr1       , 0x101FF104
 .equ Adr_NX10_vic_vic_vect_addr2   , 0x101FF108
 .equ Adr_NX10_vic_vect_addr2       , 0x101FF108
 .equ Adr_NX10_vic_vic_vect_addr3   , 0x101FF10C
 .equ Adr_NX10_vic_vect_addr3       , 0x101FF10C
 .equ Adr_NX10_vic_vic_vect_addr4   , 0x101FF110
 .equ Adr_NX10_vic_vect_addr4       , 0x101FF110
 .equ Adr_NX10_vic_vic_vect_addr5   , 0x101FF114
 .equ Adr_NX10_vic_vect_addr5       , 0x101FF114
 .equ Adr_NX10_vic_vic_vect_addr6   , 0x101FF118
 .equ Adr_NX10_vic_vect_addr6       , 0x101FF118
 .equ Adr_NX10_vic_vic_vect_addr7   , 0x101FF11C
 .equ Adr_NX10_vic_vect_addr7       , 0x101FF11C
 .equ Adr_NX10_vic_vic_vect_addr8   , 0x101FF120
 .equ Adr_NX10_vic_vect_addr8       , 0x101FF120
 .equ Adr_NX10_vic_vic_vect_addr9   , 0x101FF124
 .equ Adr_NX10_vic_vect_addr9       , 0x101FF124
 .equ Adr_NX10_vic_vic_vect_addr10  , 0x101FF128
 .equ Adr_NX10_vic_vect_addr10      , 0x101FF128
 .equ Adr_NX10_vic_vic_vect_addr11  , 0x101FF12C
 .equ Adr_NX10_vic_vect_addr11      , 0x101FF12C
 .equ Adr_NX10_vic_vic_vect_addr12  , 0x101FF130
 .equ Adr_NX10_vic_vect_addr12      , 0x101FF130
 .equ Adr_NX10_vic_vic_vect_addr13  , 0x101FF134
 .equ Adr_NX10_vic_vect_addr13      , 0x101FF134
 .equ Adr_NX10_vic_vic_vect_addr14  , 0x101FF138
 .equ Adr_NX10_vic_vect_addr14      , 0x101FF138
 .equ Adr_NX10_vic_vic_vect_addr15  , 0x101FF13C
 .equ Adr_NX10_vic_vect_addr15      , 0x101FF13C
 .equ Adr_NX10_vic_vic_vect_cntl0   , 0x101FF200
 .equ Adr_NX10_vic_vect_cntl0       , 0x101FF200
 .equ Adr_NX10_vic_vic_vect_cntl1   , 0x101FF204
 .equ Adr_NX10_vic_vect_cntl1       , 0x101FF204
 .equ Adr_NX10_vic_vic_vect_cntl2   , 0x101FF208
 .equ Adr_NX10_vic_vect_cntl2       , 0x101FF208
 .equ Adr_NX10_vic_vic_vect_cntl3   , 0x101FF20C
 .equ Adr_NX10_vic_vect_cntl3       , 0x101FF20C
 .equ Adr_NX10_vic_vic_vect_cntl4   , 0x101FF210
 .equ Adr_NX10_vic_vect_cntl4       , 0x101FF210
 .equ Adr_NX10_vic_vic_vect_cntl5   , 0x101FF214
 .equ Adr_NX10_vic_vect_cntl5       , 0x101FF214
 .equ Adr_NX10_vic_vic_vect_cntl6   , 0x101FF218
 .equ Adr_NX10_vic_vect_cntl6       , 0x101FF218
 .equ Adr_NX10_vic_vic_vect_cntl7   , 0x101FF21C
 .equ Adr_NX10_vic_vect_cntl7       , 0x101FF21C
 .equ Adr_NX10_vic_vic_vect_cntl8   , 0x101FF220
 .equ Adr_NX10_vic_vect_cntl8       , 0x101FF220
 .equ Adr_NX10_vic_vic_vect_cntl9   , 0x101FF224
 .equ Adr_NX10_vic_vect_cntl9       , 0x101FF224
 .equ Adr_NX10_vic_vic_vect_cntl10  , 0x101FF228
 .equ Adr_NX10_vic_vect_cntl10      , 0x101FF228
 .equ Adr_NX10_vic_vic_vect_cntl11  , 0x101FF22C
 .equ Adr_NX10_vic_vect_cntl11      , 0x101FF22C
 .equ Adr_NX10_vic_vic_vect_cntl12  , 0x101FF230
 .equ Adr_NX10_vic_vect_cntl12      , 0x101FF230
 .equ Adr_NX10_vic_vic_vect_cntl13  , 0x101FF234
 .equ Adr_NX10_vic_vect_cntl13      , 0x101FF234
 .equ Adr_NX10_vic_vic_vect_cntl14  , 0x101FF238
 .equ Adr_NX10_vic_vect_cntl14      , 0x101FF238
 .equ Adr_NX10_vic_vic_vect_cntl15  , 0x101FF23C
 .equ Adr_NX10_vic_vect_cntl15      , 0x101FF23C

@ =====================================================================
@
@ Area sqirom_unbuf
 .equ Addr_NX10_sqirom_unbuf, 0x11000000
@
@ =====================================================================
@ =====================================================================
@
@ Area extmemory
 .equ Addr_NX10_extmemory, 0x80000000
@
@ =====================================================================
@ =====================================================================
@
@ Area sdram
 .equ Addr_NX10_sdram, 0x80000000
@
@ =====================================================================
 .equ Adr_NX10_sdram_sdram_base, 0x80000000
 .equ Adr_NX10_sdram_base      , 0x80000000
 .equ Adr_NX10_sdram_sdram_end , 0xBFFFFFFC
 .equ Adr_NX10_sdram_end       , 0xBFFFFFFC

@ =====================================================================
@
@ Area extsram
 .equ Addr_NX10_extsram, 0xC0000000
@
@ =====================================================================
@ =====================================================================
@
@ Area extsram0
 .equ Addr_NX10_extsram0, 0xC0000000
@
@ =====================================================================
 .equ Adr_NX10_extsram0_extsram0_base, 0xC0000000
 .equ Adr_NX10_extsram0_base         , 0xC0000000
 .equ Adr_NX10_extsram0_extsram0_end , 0xC7FFFFFC
 .equ Adr_NX10_extsram0_end          , 0xC7FFFFFC

@ =====================================================================
@
@ Area extsram1
 .equ Addr_NX10_extsram1, 0xC8000000
@
@ =====================================================================
 .equ Adr_NX10_extsram1_extsram1_base, 0xC8000000
 .equ Adr_NX10_extsram1_base         , 0xC8000000
 .equ Adr_NX10_extsram1_extsram1_end , 0xCFFFFFFC
 .equ Adr_NX10_extsram1_end          , 0xCFFFFFFC

@ =====================================================================
@
@ Area extsram2
 .equ Addr_NX10_extsram2, 0xD0000000
@
@ =====================================================================
 .equ Adr_NX10_extsram2_extsram2_base, 0xD0000000
 .equ Adr_NX10_extsram2_base         , 0xD0000000
 .equ Adr_NX10_extsram2_extsram2_end , 0xD7FFFFFC
 .equ Adr_NX10_extsram2_end          , 0xD7FFFFFC

@ =====================================================================
@
@ Area extsram3
 .equ Addr_NX10_extsram3, 0xD8000000
@
@ =====================================================================
 .equ Adr_NX10_extsram3_extsram3_base, 0xD8000000
 .equ Adr_NX10_extsram3_base         , 0xD8000000
 .equ Adr_NX10_extsram3_extsram3_end , 0xDFFFFFFC
 .equ Adr_NX10_extsram3_end          , 0xDFFFFFFC

@ =====================================================================
@
@ Area intlogic_motion_mirror_hi
 .equ Addr_NX10_intlogic_motion_mirror_hi, 0xFFF00000
@
@ =====================================================================
@ =====================================================================
@
@ Area intlogic_sys_mirror_hi
 .equ Addr_NX10_intlogic_sys_mirror_hi, 0xFFF80000
@
@ =====================================================================
@ =====================================================================
@
@ Area extmem_priority_ctrl
@
@ =====================================================================


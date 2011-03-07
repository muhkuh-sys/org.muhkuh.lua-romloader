@ ******************************************************************
@ from regdef.v by regdef2s auto-generated Assembler file           
@ please see the regdef.html file for detailed register description 
@ ******************************************************************

@ =====================================================================
@
@ Area ARMiTCM_hidden_memarea
 .equ Addr_NX50_ARMiTCM_hidden_memarea, 0x00000000
@
@ =====================================================================
@ =====================================================================
@
@ Area sram0_ahbls0_start4
 .equ Addr_NX50_sram0_ahbls0_mirror_nitcm, 0x00000000
@
@ =====================================================================
 .equ Adr_NX50_sram0_ahbls0_mirror_nitcm_intram0_base_start4, 0x00000004
 .equ Adr_NX50_intram0_base_start4                          , 0x00000004
 .equ Adr_NX50_sram0_ahbls0_mirror_nitcm_intram0_end_start4 , 0x00007FFC
 .equ Adr_NX50_intram0_end_start4                           , 0x00007FFC

@ =====================================================================
@
@ Area iTCM
 .equ Addr_NX50_iTCM, 0x00000000
@
@ =====================================================================
 .equ Adr_NX50_iTCM_iTCM_base, 0x00000000
 .equ Adr_NX50_iTCM_base     , 0x00000000
 .equ Adr_NX50_iTCM_iTCM_end , 0x00001FFC
 .equ Adr_NX50_iTCM_end      , 0x00001FFC

@ =====================================================================
@
@ Area sram1_ahbls1
 .equ Addr_NX50_sram1_ahbls1_mirror_nitcm, 0x00008000
 .equ Addr_NX50_sram1_ahbls1, 0x08008000
 .equ Addr_NX50_sram1_ahbls1_mirror_nbuf, 0x10008000
@
@ =====================================================================
 .equ Adr_NX50_sram1_ahbls1_mirror_nitcm_intram1_base, 0x00008000
 .equ Adr_NX50_sram1_ahbls1_intram1_base             , 0x08008000
 .equ Adr_NX50_sram1_ahbls1_mirror_nbuf_intram1_base , 0x10008000
 .equ Adr_NX50_sram1_ahbls1_mirror_nitcm_intram1_end , 0x0000FFFC
 .equ Adr_NX50_sram1_ahbls1_intram1_end              , 0x0800FFFC
 .equ Adr_NX50_sram1_ahbls1_mirror_nbuf_intram1_end  , 0x1000FFFC

@ =====================================================================
@
@ Area sram2_ahbls2
 .equ Addr_NX50_sram2_ahbls2_mirror_nitcm, 0x00010000
 .equ Addr_NX50_sram2_ahbls2, 0x08010000
 .equ Addr_NX50_sram2_ahbls2_mirror_nbuf, 0x10010000
@
@ =====================================================================
 .equ Adr_NX50_sram2_ahbls2_mirror_nitcm_intram2_base, 0x00010000
 .equ Adr_NX50_sram2_ahbls2_intram2_base             , 0x08010000
 .equ Adr_NX50_sram2_ahbls2_mirror_nbuf_intram2_base , 0x10010000
 .equ Adr_NX50_sram2_ahbls2_mirror_nitcm_intram2_end , 0x00017FFC
 .equ Adr_NX50_sram2_ahbls2_intram2_end              , 0x08017FFC
 .equ Adr_NX50_sram2_ahbls2_mirror_nbuf_intram2_end  , 0x10017FFC

@ =====================================================================
@
@ Area ahbls4_mirror_nitcm
 .equ Addr_NX50_ahbls4_mirror_nitcm, 0x00200000
@
@ =====================================================================
@ =====================================================================
@
@ Area boot_rom
 .equ Addr_NX50_boot_rom_mirror_nitcm, 0x00200000
 .equ Addr_NX50_boot_rom, 0x08200000
 .equ Addr_NX50_boot_rom_mirror_nbuf, 0x10200000
 .equ Addr_NX50_boot_rom_mirror_hi, 0xFF200000
@
@ =====================================================================
 .equ Adr_NX50_boot_rom_mirror_nitcm_boot_rom_base, 0x00200000
 .equ Adr_NX50_boot_rom_boot_rom_base             , 0x08200000
 .equ Adr_NX50_boot_rom_mirror_nbuf_boot_rom_base , 0x10200000
 .equ Adr_NX50_boot_rom_mirror_hi_boot_rom_base   , 0xFF200000
 .equ Adr_NX50_boot_rom_mirror_nitcm_boot_rom_end , 0x0020FFFC
 .equ Adr_NX50_boot_rom_boot_rom_end              , 0x0820FFFC
 .equ Adr_NX50_boot_rom_mirror_nbuf_boot_rom_end  , 0x1020FFFC
 .equ Adr_NX50_boot_rom_mirror_hi_boot_rom_end    , 0xFF20FFFC

@ =====================================================================
@
@ Area ARMdTCM_hidden_memarea
 .equ Addr_NX50_ARMdTCM_hidden_memarea, 0x04000000
@
@ =====================================================================
@ =====================================================================
@
@ Area dTCM
 .equ Addr_NX50_dTCM, 0x04000000
@
@ =====================================================================
 .equ Adr_NX50_dTCM_dTCM_base, 0x04000000
 .equ Adr_NX50_dTCM_base     , 0x04000000
 .equ Adr_NX50_dTCM_dTCM_end , 0x04001FFC
 .equ Adr_NX50_dTCM_end      , 0x04001FFC

@ =====================================================================
@
@ Area ARM_buf_memarea
 .equ Addr_NX50_ARM_buf_memarea, 0x08000000
@
@ =====================================================================
@ =====================================================================
@
@ Area sram0_ahbls0
 .equ Addr_NX50_sram0_ahbls0, 0x08000000
 .equ Addr_NX50_sram0_ahbls0_mirror_nbuf, 0x10000000
@
@ =====================================================================
 .equ Adr_NX50_sram0_ahbls0_intram0_base            , 0x08000000
 .equ Adr_NX50_sram0_ahbls0_mirror_nbuf_intram0_base, 0x10000000
 .equ Adr_NX50_sram0_ahbls0_intram0_end             , 0x08007FFC
 .equ Adr_NX50_sram0_ahbls0_mirror_nbuf_intram0_end , 0x10007FFC

@ =====================================================================
@
@ Area ahbls4
 .equ Addr_NX50_ahbls4, 0x08200000
@
@ =====================================================================
@ =====================================================================
@
@ Area intlogic_ahbls5_mirror_buf
 .equ Addr_NX50_intlogic_ahbls5_mirror_buf, 0x0C000000
@
@ =====================================================================
@ =====================================================================
@
@ Area ARM_nbuf_memarea
 .equ Addr_NX50_ARM_nbuf_memarea, 0x10000000
@
@ =====================================================================
@ =====================================================================
@
@ Area ahbls4_mirror_nbuf
 .equ Addr_NX50_ahbls4_mirror_nbuf, 0x10200000
@
@ =====================================================================
@ =====================================================================
@
@ Area ARM_nbuf_regarea
 .equ Addr_NX50_ARM_nbuf_regarea, 0x18000000
@
@ =====================================================================
@ =====================================================================
@
@ Area intlogic_ahbls5
 .equ Addr_NX50_intlogic_ahbls5, 0x1C000000
@
@ =====================================================================
@ =====================================================================
@
@ Area asic_ctrl
 .equ Addr_NX50_asic_ctrl, 0x1C000000
@
@ =====================================================================
 .equ Adr_NX50_asic_ctrl_io_status               , 0x1C000000
 .equ Adr_NX50_io_status                         , 0x1C000000
 .equ Adr_NX50_asic_ctrl_io_config               , 0x1C000004
 .equ Adr_NX50_io_config                         , 0x1C000004
 .equ Adr_NX50_asic_ctrl_io_config_mask          , 0x1C000008
 .equ Adr_NX50_io_config_mask                    , 0x1C000008
 .equ Adr_NX50_asic_ctrl_reset_ctrl              , 0x1C00000C
 .equ Adr_NX50_reset_ctrl                        , 0x1C00000C
 .equ Adr_NX50_asic_ctrl_phy_control             , 0x1C000010
 .equ Adr_NX50_phy_control                       , 0x1C000010
 .equ Adr_NX50_asic_ctrl_armclk_rate_mul_add     , 0x1C000014
 .equ Adr_NX50_armclk_rate_mul_add               , 0x1C000014
 .equ Adr_NX50_asic_ctrl_usbclk_rate_mul_add     , 0x1C000018
 .equ Adr_NX50_usbclk_rate_mul_add               , 0x1C000018
 .equ Adr_NX50_asic_ctrl_fb0clk_rate_mul_add     , 0x1C00001C
 .equ Adr_NX50_fb0clk_rate_mul_add               , 0x1C00001C
 .equ Adr_NX50_asic_ctrl_fb1clk_rate_mul_add     , 0x1C000020
 .equ Adr_NX50_fb1clk_rate_mul_add               , 0x1C000020
 .equ Adr_NX50_asic_ctrl_clock_enable            , 0x1C000024
 .equ Adr_NX50_clock_enable                      , 0x1C000024
 .equ Adr_NX50_asic_ctrl_clock_enable_mask       , 0x1C000028
 .equ Adr_NX50_clock_enable_mask                 , 0x1C000028
 .equ Adr_NX50_asic_ctrl_misc_asic_ctrl          , 0x1C00002C
 .equ Adr_NX50_misc_asic_ctrl                    , 0x1C00002C
 .equ Adr_NX50_asic_ctrl_exmem_priority_lock     , 0x1C000030
 .equ Adr_NX50_exmem_priority_lock               , 0x1C000030
 .equ Adr_NX50_asic_ctrl_netx_version            , 0x1C000034
 .equ Adr_NX50_netx_version                      , 0x1C000034
 .equ Adr_NX50_asic_ctrl_rom_wdg                 , 0x1C000038
 .equ Adr_NX50_rom_wdg                           , 0x1C000038
 .equ Adr_NX50_asic_ctrl_asic_ctrl_access_key    , 0x1C000070
 .equ Adr_NX50_asic_ctrl_access_key              , 0x1C000070
 .equ Adr_NX50_asic_ctrl_netx_lock_reserved00    , 0x1C000074
 .equ Adr_NX50_netx_lock_reserved00              , 0x1C000074
 .equ Adr_NX50_asic_ctrl_netx_lock_reserved01    , 0x1C000078
 .equ Adr_NX50_netx_lock_reserved01              , 0x1C000078
 .equ Adr_NX50_asic_ctrl_netx_lock_reserved_start, 0x1C000080
 .equ Adr_NX50_netx_lock_reserved_start          , 0x1C000080
 .equ Adr_NX50_asic_ctrl_netx_lock_reserved_end  , 0x1C0000FC
 .equ Adr_NX50_netx_lock_reserved_end            , 0x1C0000FC

@ =====================================================================
@
@ Area extmemctrl
 .equ Addr_NX50_extmemctrl, 0x1C000100
@
@ =====================================================================
@ =====================================================================
@
@ Area ext_asyncmem_ctrl
 .equ Addr_NX50_ext_asyncmem_ctrl, 0x1C000100
@
@ =====================================================================
 .equ Adr_NX50_ext_asyncmem_ctrl_extsram0_ctrl, 0x1C000100
 .equ Adr_NX50_extsram0_ctrl                  , 0x1C000100
 .equ Adr_NX50_ext_asyncmem_ctrl_extsram1_ctrl, 0x1C000104
 .equ Adr_NX50_extsram1_ctrl                  , 0x1C000104
 .equ Adr_NX50_ext_asyncmem_ctrl_extsram2_ctrl, 0x1C000108
 .equ Adr_NX50_extsram2_ctrl                  , 0x1C000108
 .equ Adr_NX50_ext_asyncmem_ctrl_extsram3_ctrl, 0x1C00010C
 .equ Adr_NX50_extsram3_ctrl                  , 0x1C00010C

@ =====================================================================
@
@ Area ext_sdram_ctrl
 .equ Addr_NX50_ext_sdram_ctrl, 0x1C000140
@
@ =====================================================================
 .equ Adr_NX50_ext_sdram_ctrl_sdram_general_ctrl, 0x1C000140
 .equ Adr_NX50_sdram_general_ctrl               , 0x1C000140
 .equ Adr_NX50_ext_sdram_ctrl_sdram_timing_ctrl , 0x1C000144
 .equ Adr_NX50_sdram_timing_ctrl                , 0x1C000144
 .equ Adr_NX50_ext_sdram_ctrl_sdram_mr          , 0x1C000148
 .equ Adr_NX50_sdram_mr                         , 0x1C000148
 .equ Adr_NX50_ext_sdram_ctrl_sdram_emr2        , 0x1C00014C
 .equ Adr_NX50_sdram_emr2                       , 0x1C00014C

@ =====================================================================
@
@ Area extmem_priority_ctrl
 .equ Addr_NX50_extmem_priority_ctrl, 0x1C000180
@
@ =====================================================================
 .equ Adr_NX50_extmem_priority_ctrl_extmem_prio_timslot_ctrl   , 0x1C000180
 .equ Adr_NX50_extmem_prio_timslot_ctrl                        , 0x1C000180
 .equ Adr_NX50_extmem_priority_ctrl_extmem_prio_accesstime_ctrl, 0x1C000184
 .equ Adr_NX50_extmem_prio_accesstime_ctrl                     , 0x1C000184

@ =====================================================================
@
@ Area watchdog
 .equ Addr_NX50_watchdog, 0x1C000200
@
@ =====================================================================
 .equ Adr_NX50_watchdog_netx_sys_wdg_ctrl                , 0x1C000200
 .equ Adr_NX50_netx_sys_wdg_ctrl                         , 0x1C000200
 .equ Adr_NX50_watchdog_netx_sys_wdg                     , 0x1C000204
 .equ Adr_NX50_netx_sys_wdg                              , 0x1C000204
 .equ Adr_NX50_watchdog_netx_sys_wdg_irq_timeout         , 0x1C000208
 .equ Adr_NX50_netx_sys_wdg_irq_timeout                  , 0x1C000208
 .equ Adr_NX50_watchdog_netx_sys_wdg_res_timeout         , 0x1C00020C
 .equ Adr_NX50_netx_sys_wdg_res_timeout                  , 0x1C00020C
 .equ Adr_NX50_watchdog_netx_sys_wdg_ctrl_mirror1        , 0x1C000210
 .equ Adr_NX50_netx_sys_wdg_ctrl_mirror1                 , 0x1C000210
 .equ Adr_NX50_watchdog_netx_sys_wdg_mirror1             , 0x1C000214
 .equ Adr_NX50_netx_sys_wdg_mirror1                      , 0x1C000214
 .equ Adr_NX50_watchdog_netx_sys_wdg_irq_timeout_mirror1 , 0x1C000218
 .equ Adr_NX50_netx_sys_wdg_irq_timeout_mirror1          , 0x1C000218
 .equ Adr_NX50_watchdog_netx_sys_wdg_res_timeout_mirror1 , 0x1C00021C
 .equ Adr_NX50_netx_sys_wdg_res_timeout_mirror1          , 0x1C00021C
 .equ Adr_NX50_watchdog_netx_sys_wdg_ctrl_mirror2        , 0x1C000220
 .equ Adr_NX50_netx_sys_wdg_ctrl_mirror2                 , 0x1C000220
 .equ Adr_NX50_watchdog_netx_sys_wdg_mirror2             , 0x1C000224
 .equ Adr_NX50_netx_sys_wdg_mirror2                      , 0x1C000224
 .equ Adr_NX50_watchdog_netx_sys_wdg_irq_timeout_mirror2 , 0x1C000228
 .equ Adr_NX50_netx_sys_wdg_irq_timeout_mirror2          , 0x1C000228
 .equ Adr_NX50_watchdog_netx_sys_wdg_res_timeout_mirror2 , 0x1C00022C
 .equ Adr_NX50_netx_sys_wdg_res_timeout_mirror2          , 0x1C00022C
 .equ Adr_NX50_watchdog_netx_sys_wdg_ctrl_mirror3        , 0x1C000230
 .equ Adr_NX50_netx_sys_wdg_ctrl_mirror3                 , 0x1C000230
 .equ Adr_NX50_watchdog_netx_sys_wdg_mirror3             , 0x1C000234
 .equ Adr_NX50_netx_sys_wdg_mirror3                      , 0x1C000234
 .equ Adr_NX50_watchdog_netx_sys_wdg_irq_timeout_mirror3 , 0x1C000238
 .equ Adr_NX50_netx_sys_wdg_irq_timeout_mirror3          , 0x1C000238
 .equ Adr_NX50_watchdog_netx_sys_wdg_res_timeout_mirror3 , 0x1C00023C
 .equ Adr_NX50_netx_sys_wdg_res_timeout_mirror3          , 0x1C00023C
 .equ Adr_NX50_watchdog_netx_sys_wdg_ctrl_mirror4        , 0x1C000240
 .equ Adr_NX50_netx_sys_wdg_ctrl_mirror4                 , 0x1C000240
 .equ Adr_NX50_watchdog_netx_sys_wdg_mirror4             , 0x1C000244
 .equ Adr_NX50_netx_sys_wdg_mirror4                      , 0x1C000244
 .equ Adr_NX50_watchdog_netx_sys_wdg_irq_timeout_mirror4 , 0x1C000248
 .equ Adr_NX50_netx_sys_wdg_irq_timeout_mirror4          , 0x1C000248
 .equ Adr_NX50_watchdog_netx_sys_wdg_res_timeout_mirror4 , 0x1C00024C
 .equ Adr_NX50_netx_sys_wdg_res_timeout_mirror4          , 0x1C00024C
 .equ Adr_NX50_watchdog_netx_sys_wdg_ctrl_mirror5        , 0x1C000250
 .equ Adr_NX50_netx_sys_wdg_ctrl_mirror5                 , 0x1C000250
 .equ Adr_NX50_watchdog_netx_sys_wdg_mirror5             , 0x1C000254
 .equ Adr_NX50_netx_sys_wdg_mirror5                      , 0x1C000254
 .equ Adr_NX50_watchdog_netx_sys_wdg_irq_timeout_mirror5 , 0x1C000258
 .equ Adr_NX50_netx_sys_wdg_irq_timeout_mirror5          , 0x1C000258
 .equ Adr_NX50_watchdog_netx_sys_wdg_res_timeout_mirror5 , 0x1C00025C
 .equ Adr_NX50_netx_sys_wdg_res_timeout_mirror5          , 0x1C00025C
 .equ Adr_NX50_watchdog_netx_sys_wdg_ctrl_mirror6        , 0x1C000260
 .equ Adr_NX50_netx_sys_wdg_ctrl_mirror6                 , 0x1C000260
 .equ Adr_NX50_watchdog_netx_sys_wdg_mirror6             , 0x1C000264
 .equ Adr_NX50_netx_sys_wdg_mirror6                      , 0x1C000264
 .equ Adr_NX50_watchdog_netx_sys_wdg_irq_timeout_mirror6 , 0x1C000268
 .equ Adr_NX50_netx_sys_wdg_irq_timeout_mirror6          , 0x1C000268
 .equ Adr_NX50_watchdog_netx_sys_wdg_res_timeout_mirror6 , 0x1C00026C
 .equ Adr_NX50_netx_sys_wdg_res_timeout_mirror6          , 0x1C00026C
 .equ Adr_NX50_watchdog_netx_sys_wdg_ctrl_mirror7        , 0x1C000270
 .equ Adr_NX50_netx_sys_wdg_ctrl_mirror7                 , 0x1C000270
 .equ Adr_NX50_watchdog_netx_sys_wdg_mirror7             , 0x1C000274
 .equ Adr_NX50_netx_sys_wdg_mirror7                      , 0x1C000274
 .equ Adr_NX50_watchdog_netx_sys_wdg_irq_timeout_mirror7 , 0x1C000278
 .equ Adr_NX50_netx_sys_wdg_irq_timeout_mirror7          , 0x1C000278
 .equ Adr_NX50_watchdog_netx_sys_wdg_res_timeout_mirror7 , 0x1C00027C
 .equ Adr_NX50_netx_sys_wdg_res_timeout_mirror7          , 0x1C00027C
 .equ Adr_NX50_watchdog_netx_sys_wdg_ctrl_mirror8        , 0x1C000280
 .equ Adr_NX50_netx_sys_wdg_ctrl_mirror8                 , 0x1C000280
 .equ Adr_NX50_watchdog_netx_sys_wdg_mirror8             , 0x1C000284
 .equ Adr_NX50_netx_sys_wdg_mirror8                      , 0x1C000284
 .equ Adr_NX50_watchdog_netx_sys_wdg_irq_timeout_mirror8 , 0x1C000288
 .equ Adr_NX50_netx_sys_wdg_irq_timeout_mirror8          , 0x1C000288
 .equ Adr_NX50_watchdog_netx_sys_wdg_res_timeout_mirror8 , 0x1C00028C
 .equ Adr_NX50_netx_sys_wdg_res_timeout_mirror8          , 0x1C00028C
 .equ Adr_NX50_watchdog_netx_sys_wdg_ctrl_mirror9        , 0x1C000290
 .equ Adr_NX50_netx_sys_wdg_ctrl_mirror9                 , 0x1C000290
 .equ Adr_NX50_watchdog_netx_sys_wdg_mirror9             , 0x1C000294
 .equ Adr_NX50_netx_sys_wdg_mirror9                      , 0x1C000294
 .equ Adr_NX50_watchdog_netx_sys_wdg_irq_timeout_mirror9 , 0x1C000298
 .equ Adr_NX50_netx_sys_wdg_irq_timeout_mirror9          , 0x1C000298
 .equ Adr_NX50_watchdog_netx_sys_wdg_res_timeout_mirror9 , 0x1C00029C
 .equ Adr_NX50_netx_sys_wdg_res_timeout_mirror9          , 0x1C00029C
 .equ Adr_NX50_watchdog_netx_sys_wdg_ctrl_mirror10       , 0x1C0002A0
 .equ Adr_NX50_netx_sys_wdg_ctrl_mirror10                , 0x1C0002A0
 .equ Adr_NX50_watchdog_netx_sys_wdg_mirror10            , 0x1C0002A4
 .equ Adr_NX50_netx_sys_wdg_mirror10                     , 0x1C0002A4
 .equ Adr_NX50_watchdog_netx_sys_wdg_irq_timeout_mirror10, 0x1C0002A8
 .equ Adr_NX50_netx_sys_wdg_irq_timeout_mirror10         , 0x1C0002A8
 .equ Adr_NX50_watchdog_netx_sys_wdg_res_timeout_mirror10, 0x1C0002AC
 .equ Adr_NX50_netx_sys_wdg_res_timeout_mirror10         , 0x1C0002AC
 .equ Adr_NX50_watchdog_netx_sys_wdg_ctrl_mirror11       , 0x1C0002B0
 .equ Adr_NX50_netx_sys_wdg_ctrl_mirror11                , 0x1C0002B0
 .equ Adr_NX50_watchdog_netx_sys_wdg_mirror11            , 0x1C0002B4
 .equ Adr_NX50_netx_sys_wdg_mirror11                     , 0x1C0002B4
 .equ Adr_NX50_watchdog_netx_sys_wdg_irq_timeout_mirror11, 0x1C0002B8
 .equ Adr_NX50_netx_sys_wdg_irq_timeout_mirror11         , 0x1C0002B8
 .equ Adr_NX50_watchdog_netx_sys_wdg_res_timeout_mirror11, 0x1C0002BC
 .equ Adr_NX50_netx_sys_wdg_res_timeout_mirror11         , 0x1C0002BC
 .equ Adr_NX50_watchdog_netx_sys_wdg_ctrl_mirror12       , 0x1C0002C0
 .equ Adr_NX50_netx_sys_wdg_ctrl_mirror12                , 0x1C0002C0
 .equ Adr_NX50_watchdog_netx_sys_wdg_mirror12            , 0x1C0002C4
 .equ Adr_NX50_netx_sys_wdg_mirror12                     , 0x1C0002C4
 .equ Adr_NX50_watchdog_netx_sys_wdg_irq_timeout_mirror12, 0x1C0002C8
 .equ Adr_NX50_netx_sys_wdg_irq_timeout_mirror12         , 0x1C0002C8
 .equ Adr_NX50_watchdog_netx_sys_wdg_res_timeout_mirror12, 0x1C0002CC
 .equ Adr_NX50_netx_sys_wdg_res_timeout_mirror12         , 0x1C0002CC
 .equ Adr_NX50_watchdog_netx_sys_wdg_ctrl_mirror13       , 0x1C0002D0
 .equ Adr_NX50_netx_sys_wdg_ctrl_mirror13                , 0x1C0002D0
 .equ Adr_NX50_watchdog_netx_sys_wdg_mirror13            , 0x1C0002D4
 .equ Adr_NX50_netx_sys_wdg_mirror13                     , 0x1C0002D4
 .equ Adr_NX50_watchdog_netx_sys_wdg_irq_timeout_mirror13, 0x1C0002D8
 .equ Adr_NX50_netx_sys_wdg_irq_timeout_mirror13         , 0x1C0002D8
 .equ Adr_NX50_watchdog_netx_sys_wdg_res_timeout_mirror13, 0x1C0002DC
 .equ Adr_NX50_netx_sys_wdg_res_timeout_mirror13         , 0x1C0002DC
 .equ Adr_NX50_watchdog_netx_sys_wdg_ctrl_mirror14       , 0x1C0002E0
 .equ Adr_NX50_netx_sys_wdg_ctrl_mirror14                , 0x1C0002E0
 .equ Adr_NX50_watchdog_netx_sys_wdg_mirror14            , 0x1C0002E4
 .equ Adr_NX50_netx_sys_wdg_mirror14                     , 0x1C0002E4
 .equ Adr_NX50_watchdog_netx_sys_wdg_irq_timeout_mirror14, 0x1C0002E8
 .equ Adr_NX50_netx_sys_wdg_irq_timeout_mirror14         , 0x1C0002E8
 .equ Adr_NX50_watchdog_netx_sys_wdg_res_timeout_mirror14, 0x1C0002EC
 .equ Adr_NX50_netx_sys_wdg_res_timeout_mirror14         , 0x1C0002EC
 .equ Adr_NX50_watchdog_netx_sys_wdg_ctrl_mirror15       , 0x1C0002F0
 .equ Adr_NX50_netx_sys_wdg_ctrl_mirror15                , 0x1C0002F0
 .equ Adr_NX50_watchdog_netx_sys_wdg_mirror15            , 0x1C0002F4
 .equ Adr_NX50_netx_sys_wdg_mirror15                     , 0x1C0002F4
 .equ Adr_NX50_watchdog_netx_sys_wdg_irq_timeout_mirror15, 0x1C0002F8
 .equ Adr_NX50_netx_sys_wdg_irq_timeout_mirror15         , 0x1C0002F8
 .equ Adr_NX50_watchdog_netx_sys_wdg_res_timeout_mirror15, 0x1C0002FC
 .equ Adr_NX50_netx_sys_wdg_res_timeout_mirror15         , 0x1C0002FC

@ =====================================================================
@
@ Area abort
 .equ Addr_NX50_abort, 0x1C000300
@
@ =====================================================================
@ =====================================================================
@
@ Area gpio
 .equ Addr_NX50_gpio, 0x1C000800
@
@ =====================================================================
 .equ Adr_NX50_gpio_gpio_cfg0         , 0x1C000800
 .equ Adr_NX50_gpio_cfg0              , 0x1C000800
 .equ Adr_NX50_gpio_gpio_cfg1         , 0x1C000804
 .equ Adr_NX50_gpio_cfg1              , 0x1C000804
 .equ Adr_NX50_gpio_gpio_cfg2         , 0x1C000808
 .equ Adr_NX50_gpio_cfg2              , 0x1C000808
 .equ Adr_NX50_gpio_gpio_cfg3         , 0x1C00080C
 .equ Adr_NX50_gpio_cfg3              , 0x1C00080C
 .equ Adr_NX50_gpio_gpio_cfg4         , 0x1C000810
 .equ Adr_NX50_gpio_cfg4              , 0x1C000810
 .equ Adr_NX50_gpio_gpio_cfg5         , 0x1C000814
 .equ Adr_NX50_gpio_cfg5              , 0x1C000814
 .equ Adr_NX50_gpio_gpio_cfg6         , 0x1C000818
 .equ Adr_NX50_gpio_cfg6              , 0x1C000818
 .equ Adr_NX50_gpio_gpio_cfg7         , 0x1C00081C
 .equ Adr_NX50_gpio_cfg7              , 0x1C00081C
 .equ Adr_NX50_gpio_gpio_cfg8         , 0x1C000820
 .equ Adr_NX50_gpio_cfg8              , 0x1C000820
 .equ Adr_NX50_gpio_gpio_cfg9         , 0x1C000824
 .equ Adr_NX50_gpio_cfg9              , 0x1C000824
 .equ Adr_NX50_gpio_gpio_cfg10        , 0x1C000828
 .equ Adr_NX50_gpio_cfg10             , 0x1C000828
 .equ Adr_NX50_gpio_gpio_cfg11        , 0x1C00082C
 .equ Adr_NX50_gpio_cfg11             , 0x1C00082C
 .equ Adr_NX50_gpio_gpio_cfg12        , 0x1C000830
 .equ Adr_NX50_gpio_cfg12             , 0x1C000830
 .equ Adr_NX50_gpio_gpio_cfg13        , 0x1C000834
 .equ Adr_NX50_gpio_cfg13             , 0x1C000834
 .equ Adr_NX50_gpio_gpio_cfg14        , 0x1C000838
 .equ Adr_NX50_gpio_cfg14             , 0x1C000838
 .equ Adr_NX50_gpio_gpio_cfg15        , 0x1C00083C
 .equ Adr_NX50_gpio_cfg15             , 0x1C00083C
 .equ Adr_NX50_gpio_gpio_cfg16        , 0x1C000840
 .equ Adr_NX50_gpio_cfg16             , 0x1C000840
 .equ Adr_NX50_gpio_gpio_cfg17        , 0x1C000844
 .equ Adr_NX50_gpio_cfg17             , 0x1C000844
 .equ Adr_NX50_gpio_gpio_cfg18        , 0x1C000848
 .equ Adr_NX50_gpio_cfg18             , 0x1C000848
 .equ Adr_NX50_gpio_gpio_cfg19        , 0x1C00084C
 .equ Adr_NX50_gpio_cfg19             , 0x1C00084C
 .equ Adr_NX50_gpio_gpio_cfg20        , 0x1C000850
 .equ Adr_NX50_gpio_cfg20             , 0x1C000850
 .equ Adr_NX50_gpio_gpio_cfg21        , 0x1C000854
 .equ Adr_NX50_gpio_cfg21             , 0x1C000854
 .equ Adr_NX50_gpio_gpio_cfg22        , 0x1C000858
 .equ Adr_NX50_gpio_cfg22             , 0x1C000858
 .equ Adr_NX50_gpio_gpio_cfg23        , 0x1C00085C
 .equ Adr_NX50_gpio_cfg23             , 0x1C00085C
 .equ Adr_NX50_gpio_gpio_cfg24        , 0x1C000860
 .equ Adr_NX50_gpio_cfg24             , 0x1C000860
 .equ Adr_NX50_gpio_gpio_cfg25        , 0x1C000864
 .equ Adr_NX50_gpio_cfg25             , 0x1C000864
 .equ Adr_NX50_gpio_gpio_cfg26        , 0x1C000868
 .equ Adr_NX50_gpio_cfg26             , 0x1C000868
 .equ Adr_NX50_gpio_gpio_cfg27        , 0x1C00086C
 .equ Adr_NX50_gpio_cfg27             , 0x1C00086C
 .equ Adr_NX50_gpio_gpio_cfg28        , 0x1C000870
 .equ Adr_NX50_gpio_cfg28             , 0x1C000870
 .equ Adr_NX50_gpio_gpio_cfg29        , 0x1C000874
 .equ Adr_NX50_gpio_cfg29             , 0x1C000874
 .equ Adr_NX50_gpio_gpio_cfg30        , 0x1C000878
 .equ Adr_NX50_gpio_cfg30             , 0x1C000878
 .equ Adr_NX50_gpio_gpio_cfg31        , 0x1C00087C
 .equ Adr_NX50_gpio_cfg31             , 0x1C00087C
 .equ Adr_NX50_gpio_gpio_tc0          , 0x1C000880
 .equ Adr_NX50_gpio_tc0               , 0x1C000880
 .equ Adr_NX50_gpio_gpio_tc1          , 0x1C000884
 .equ Adr_NX50_gpio_tc1               , 0x1C000884
 .equ Adr_NX50_gpio_gpio_tc2          , 0x1C000888
 .equ Adr_NX50_gpio_tc2               , 0x1C000888
 .equ Adr_NX50_gpio_gpio_tc3          , 0x1C00088C
 .equ Adr_NX50_gpio_tc3               , 0x1C00088C
 .equ Adr_NX50_gpio_gpio_tc4          , 0x1C000890
 .equ Adr_NX50_gpio_tc4               , 0x1C000890
 .equ Adr_NX50_gpio_gpio_tc5          , 0x1C000894
 .equ Adr_NX50_gpio_tc5               , 0x1C000894
 .equ Adr_NX50_gpio_gpio_tc6          , 0x1C000898
 .equ Adr_NX50_gpio_tc6               , 0x1C000898
 .equ Adr_NX50_gpio_gpio_tc7          , 0x1C00089C
 .equ Adr_NX50_gpio_tc7               , 0x1C00089C
 .equ Adr_NX50_gpio_gpio_tc8          , 0x1C0008A0
 .equ Adr_NX50_gpio_tc8               , 0x1C0008A0
 .equ Adr_NX50_gpio_gpio_tc9          , 0x1C0008A4
 .equ Adr_NX50_gpio_tc9               , 0x1C0008A4
 .equ Adr_NX50_gpio_gpio_tc10         , 0x1C0008A8
 .equ Adr_NX50_gpio_tc10              , 0x1C0008A8
 .equ Adr_NX50_gpio_gpio_tc11         , 0x1C0008AC
 .equ Adr_NX50_gpio_tc11              , 0x1C0008AC
 .equ Adr_NX50_gpio_gpio_tc12         , 0x1C0008B0
 .equ Adr_NX50_gpio_tc12              , 0x1C0008B0
 .equ Adr_NX50_gpio_gpio_tc13         , 0x1C0008B4
 .equ Adr_NX50_gpio_tc13              , 0x1C0008B4
 .equ Adr_NX50_gpio_gpio_tc14         , 0x1C0008B8
 .equ Adr_NX50_gpio_tc14              , 0x1C0008B8
 .equ Adr_NX50_gpio_gpio_tc15         , 0x1C0008BC
 .equ Adr_NX50_gpio_tc15              , 0x1C0008BC
 .equ Adr_NX50_gpio_gpio_tc16         , 0x1C0008C0
 .equ Adr_NX50_gpio_tc16              , 0x1C0008C0
 .equ Adr_NX50_gpio_gpio_tc17         , 0x1C0008C4
 .equ Adr_NX50_gpio_tc17              , 0x1C0008C4
 .equ Adr_NX50_gpio_gpio_tc18         , 0x1C0008C8
 .equ Adr_NX50_gpio_tc18              , 0x1C0008C8
 .equ Adr_NX50_gpio_gpio_tc19         , 0x1C0008CC
 .equ Adr_NX50_gpio_tc19              , 0x1C0008CC
 .equ Adr_NX50_gpio_gpio_tc20         , 0x1C0008D0
 .equ Adr_NX50_gpio_tc20              , 0x1C0008D0
 .equ Adr_NX50_gpio_gpio_tc21         , 0x1C0008D4
 .equ Adr_NX50_gpio_tc21              , 0x1C0008D4
 .equ Adr_NX50_gpio_gpio_tc22         , 0x1C0008D8
 .equ Adr_NX50_gpio_tc22              , 0x1C0008D8
 .equ Adr_NX50_gpio_gpio_tc23         , 0x1C0008DC
 .equ Adr_NX50_gpio_tc23              , 0x1C0008DC
 .equ Adr_NX50_gpio_gpio_tc24         , 0x1C0008E0
 .equ Adr_NX50_gpio_tc24              , 0x1C0008E0
 .equ Adr_NX50_gpio_gpio_tc25         , 0x1C0008E4
 .equ Adr_NX50_gpio_tc25              , 0x1C0008E4
 .equ Adr_NX50_gpio_gpio_tc26         , 0x1C0008E8
 .equ Adr_NX50_gpio_tc26              , 0x1C0008E8
 .equ Adr_NX50_gpio_gpio_tc27         , 0x1C0008EC
 .equ Adr_NX50_gpio_tc27              , 0x1C0008EC
 .equ Adr_NX50_gpio_gpio_tc28         , 0x1C0008F0
 .equ Adr_NX50_gpio_tc28              , 0x1C0008F0
 .equ Adr_NX50_gpio_gpio_tc29         , 0x1C0008F4
 .equ Adr_NX50_gpio_tc29              , 0x1C0008F4
 .equ Adr_NX50_gpio_gpio_tc30         , 0x1C0008F8
 .equ Adr_NX50_gpio_tc30              , 0x1C0008F8
 .equ Adr_NX50_gpio_gpio_tc31         , 0x1C0008FC
 .equ Adr_NX50_gpio_tc31              , 0x1C0008FC
 .equ Adr_NX50_gpio_gpio_counter0_ctrl, 0x1C000900
 .equ Adr_NX50_gpio_counter0_ctrl     , 0x1C000900
 .equ Adr_NX50_gpio_gpio_counter1_ctrl, 0x1C000904
 .equ Adr_NX50_gpio_counter1_ctrl     , 0x1C000904
 .equ Adr_NX50_gpio_gpio_counter2_ctrl, 0x1C000908
 .equ Adr_NX50_gpio_counter2_ctrl     , 0x1C000908
 .equ Adr_NX50_gpio_gpio_counter3_ctrl, 0x1C00090C
 .equ Adr_NX50_gpio_counter3_ctrl     , 0x1C00090C
 .equ Adr_NX50_gpio_gpio_counter4_ctrl, 0x1C000910
 .equ Adr_NX50_gpio_counter4_ctrl     , 0x1C000910
 .equ Adr_NX50_gpio_gpio_counter0_max , 0x1C000914
 .equ Adr_NX50_gpio_counter0_max      , 0x1C000914
 .equ Adr_NX50_gpio_gpio_counter1_max , 0x1C000918
 .equ Adr_NX50_gpio_counter1_max      , 0x1C000918
 .equ Adr_NX50_gpio_gpio_counter2_max , 0x1C00091C
 .equ Adr_NX50_gpio_counter2_max      , 0x1C00091C
 .equ Adr_NX50_gpio_gpio_counter3_max , 0x1C000920
 .equ Adr_NX50_gpio_counter3_max      , 0x1C000920
 .equ Adr_NX50_gpio_gpio_counter4_max , 0x1C000924
 .equ Adr_NX50_gpio_counter4_max      , 0x1C000924
 .equ Adr_NX50_gpio_gpio_counter0_cnt , 0x1C000928
 .equ Adr_NX50_gpio_counter0_cnt      , 0x1C000928
 .equ Adr_NX50_gpio_gpio_counter1_cnt , 0x1C00092C
 .equ Adr_NX50_gpio_counter1_cnt      , 0x1C00092C
 .equ Adr_NX50_gpio_gpio_counter2_cnt , 0x1C000930
 .equ Adr_NX50_gpio_counter2_cnt      , 0x1C000930
 .equ Adr_NX50_gpio_gpio_counter3_cnt , 0x1C000934
 .equ Adr_NX50_gpio_counter3_cnt      , 0x1C000934
 .equ Adr_NX50_gpio_gpio_counter4_cnt , 0x1C000938
 .equ Adr_NX50_gpio_counter4_cnt      , 0x1C000938
 .equ Adr_NX50_gpio_gpio_systime_cmp  , 0x1C00093C
 .equ Adr_NX50_gpio_systime_cmp       , 0x1C00093C
 .equ Adr_NX50_gpio_gpio_line         , 0x1C000940
 .equ Adr_NX50_gpio_line              , 0x1C000940
 .equ Adr_NX50_gpio_gpio_in           , 0x1C000944
 .equ Adr_NX50_gpio_in                , 0x1C000944
 .equ Adr_NX50_gpio_gpio_irq_raw      , 0x1C000948
 .equ Adr_NX50_gpio_irq_raw           , 0x1C000948
 .equ Adr_NX50_gpio_gpio_irq_masked   , 0x1C00094C
 .equ Adr_NX50_gpio_irq_masked        , 0x1C00094C
 .equ Adr_NX50_gpio_gpio_irq_mask_set , 0x1C000950
 .equ Adr_NX50_gpio_irq_mask_set      , 0x1C000950
 .equ Adr_NX50_gpio_gpio_irq_mask_rst , 0x1C000954
 .equ Adr_NX50_gpio_irq_mask_rst      , 0x1C000954
 .equ Adr_NX50_gpio_cnt_irq_raw       , 0x1C000958
 .equ Adr_NX50_cnt_irq_raw            , 0x1C000958
 .equ Adr_NX50_gpio_cnt_irq_masked    , 0x1C00095C
 .equ Adr_NX50_cnt_irq_masked         , 0x1C00095C
 .equ Adr_NX50_gpio_cnt_irq_mask_set  , 0x1C000960
 .equ Adr_NX50_cnt_irq_mask_set       , 0x1C000960
 .equ Adr_NX50_gpio_cnt_irq_mask_rst  , 0x1C000964
 .equ Adr_NX50_cnt_irq_mask_rst       , 0x1C000964

@ =====================================================================
@
@ Area iolink
 .equ Addr_NX50_iolink0, 0x1C000880
 .equ Addr_NX50_iolink1, 0x1C000890
 .equ Addr_NX50_iolink2, 0x1C0008A0
 .equ Addr_NX50_iolink3, 0x1C0008B0
 .equ Addr_NX50_iolink4, 0x1C0008C0
 .equ Addr_NX50_iolink5, 0x1C0008D0
 .equ Addr_NX50_iolink6, 0x1C0008E0
 .equ Addr_NX50_iolink7, 0x1C0008F0
@
@ =====================================================================
 .equ Adr_NX50_iolink0_iolink_cfg         , 0x1C000880
 .equ Adr_NX50_iolink1_iolink_cfg         , 0x1C000890
 .equ Adr_NX50_iolink2_iolink_cfg         , 0x1C0008A0
 .equ Adr_NX50_iolink3_iolink_cfg         , 0x1C0008B0
 .equ Adr_NX50_iolink4_iolink_cfg         , 0x1C0008C0
 .equ Adr_NX50_iolink5_iolink_cfg         , 0x1C0008D0
 .equ Adr_NX50_iolink6_iolink_cfg         , 0x1C0008E0
 .equ Adr_NX50_iolink7_iolink_cfg         , 0x1C0008F0
 .equ Adr_NX50_iolink0_iolink_tx_frame_1st, 0x1C000884
 .equ Adr_NX50_iolink1_iolink_tx_frame_1st, 0x1C000894
 .equ Adr_NX50_iolink2_iolink_tx_frame_1st, 0x1C0008A4
 .equ Adr_NX50_iolink3_iolink_tx_frame_1st, 0x1C0008B4
 .equ Adr_NX50_iolink4_iolink_tx_frame_1st, 0x1C0008C4
 .equ Adr_NX50_iolink5_iolink_tx_frame_1st, 0x1C0008D4
 .equ Adr_NX50_iolink6_iolink_tx_frame_1st, 0x1C0008E4
 .equ Adr_NX50_iolink7_iolink_tx_frame_1st, 0x1C0008F4
 .equ Adr_NX50_iolink0_iolink_tx_frame_2nd, 0x1C000888
 .equ Adr_NX50_iolink1_iolink_tx_frame_2nd, 0x1C000898
 .equ Adr_NX50_iolink2_iolink_tx_frame_2nd, 0x1C0008A8
 .equ Adr_NX50_iolink3_iolink_tx_frame_2nd, 0x1C0008B8
 .equ Adr_NX50_iolink4_iolink_tx_frame_2nd, 0x1C0008C8
 .equ Adr_NX50_iolink5_iolink_tx_frame_2nd, 0x1C0008D8
 .equ Adr_NX50_iolink6_iolink_tx_frame_2nd, 0x1C0008E8
 .equ Adr_NX50_iolink7_iolink_tx_frame_2nd, 0x1C0008F8
 .equ Adr_NX50_iolink0_iolink_rx_frame_1st, 0x1C00088C
 .equ Adr_NX50_iolink1_iolink_rx_frame_1st, 0x1C00089C
 .equ Adr_NX50_iolink2_iolink_rx_frame_1st, 0x1C0008AC
 .equ Adr_NX50_iolink3_iolink_rx_frame_1st, 0x1C0008BC
 .equ Adr_NX50_iolink4_iolink_rx_frame_1st, 0x1C0008CC
 .equ Adr_NX50_iolink5_iolink_rx_frame_1st, 0x1C0008DC
 .equ Adr_NX50_iolink6_iolink_rx_frame_1st, 0x1C0008EC
 .equ Adr_NX50_iolink7_iolink_rx_frame_1st, 0x1C0008FC

@ =====================================================================
@
@ Area pio
 .equ Addr_NX50_pio, 0x1C000A00
@
@ =====================================================================
 .equ Adr_NX50_pio_pio_in , 0x1C000A00
 .equ Adr_NX50_pio_in     , 0x1C000A00
 .equ Adr_NX50_pio_pio_out, 0x1C000A04
 .equ Adr_NX50_pio_out    , 0x1C000A04
 .equ Adr_NX50_pio_pio_oe , 0x1C000A08
 .equ Adr_NX50_pio_oe     , 0x1C000A08

@ =====================================================================
@
@ Area uart
 .equ Addr_NX50_uart0, 0x1C000B00
 .equ Addr_NX50_uart1, 0x1C000B40
 .equ Addr_NX50_uart2, 0x1C000B80
@
@ =====================================================================
 .equ Adr_NX50_uart0_uartdr      , 0x1C000B00
 .equ Adr_NX50_uart1_uartdr      , 0x1C000B40
 .equ Adr_NX50_uart2_uartdr      , 0x1C000B80
 .equ Adr_NX50_uart0_uartrsr     , 0x1C000B04
 .equ Adr_NX50_uart1_uartrsr     , 0x1C000B44
 .equ Adr_NX50_uart2_uartrsr     , 0x1C000B84
 .equ Adr_NX50_uart0_uartlcr_h   , 0x1C000B08
 .equ Adr_NX50_uart1_uartlcr_h   , 0x1C000B48
 .equ Adr_NX50_uart2_uartlcr_h   , 0x1C000B88
 .equ Adr_NX50_uart0_uartlcr_m   , 0x1C000B0C
 .equ Adr_NX50_uart1_uartlcr_m   , 0x1C000B4C
 .equ Adr_NX50_uart2_uartlcr_m   , 0x1C000B8C
 .equ Adr_NX50_uart0_uartlcr_l   , 0x1C000B10
 .equ Adr_NX50_uart1_uartlcr_l   , 0x1C000B50
 .equ Adr_NX50_uart2_uartlcr_l   , 0x1C000B90
 .equ Adr_NX50_uart0_uartcr      , 0x1C000B14
 .equ Adr_NX50_uart1_uartcr      , 0x1C000B54
 .equ Adr_NX50_uart2_uartcr      , 0x1C000B94
 .equ Adr_NX50_uart0_uartfr      , 0x1C000B18
 .equ Adr_NX50_uart1_uartfr      , 0x1C000B58
 .equ Adr_NX50_uart2_uartfr      , 0x1C000B98
 .equ Adr_NX50_uart0_uartiir     , 0x1C000B1C
 .equ Adr_NX50_uart1_uartiir     , 0x1C000B5C
 .equ Adr_NX50_uart2_uartiir     , 0x1C000B9C
 .equ Adr_NX50_uart0_uartilpr    , 0x1C000B20
 .equ Adr_NX50_uart1_uartilpr    , 0x1C000B60
 .equ Adr_NX50_uart2_uartilpr    , 0x1C000BA0
 .equ Adr_NX50_uart0_uartrts     , 0x1C000B24
 .equ Adr_NX50_uart1_uartrts     , 0x1C000B64
 .equ Adr_NX50_uart2_uartrts     , 0x1C000BA4
 .equ Adr_NX50_uart0_uartforerun , 0x1C000B28
 .equ Adr_NX50_uart1_uartforerun , 0x1C000B68
 .equ Adr_NX50_uart2_uartforerun , 0x1C000BA8
 .equ Adr_NX50_uart0_uarttrail   , 0x1C000B2C
 .equ Adr_NX50_uart1_uarttrail   , 0x1C000B6C
 .equ Adr_NX50_uart2_uarttrail   , 0x1C000BAC
 .equ Adr_NX50_uart0_uartdrvout  , 0x1C000B30
 .equ Adr_NX50_uart1_uartdrvout  , 0x1C000B70
 .equ Adr_NX50_uart2_uartdrvout  , 0x1C000BB0
 .equ Adr_NX50_uart0_uartcr_2    , 0x1C000B34
 .equ Adr_NX50_uart1_uartcr_2    , 0x1C000B74
 .equ Adr_NX50_uart2_uartcr_2    , 0x1C000BB4
 .equ Adr_NX50_uart0_uartrxiflsel, 0x1C000B38
 .equ Adr_NX50_uart1_uartrxiflsel, 0x1C000B78
 .equ Adr_NX50_uart2_uartrxiflsel, 0x1C000BB8
 .equ Adr_NX50_uart0_uarttxiflsel, 0x1C000B3C
 .equ Adr_NX50_uart1_uarttxiflsel, 0x1C000B7C
 .equ Adr_NX50_uart2_uarttxiflsel, 0x1C000BBC

@ =====================================================================
@
@ Area miimu
 .equ Addr_NX50_miimu, 0x1C000C00
@
@ =====================================================================
 .equ Adr_NX50_miimu_miimu_reg    , 0x1C000C00
 .equ Adr_NX50_miimu_reg          , 0x1C000C00
 .equ Adr_NX50_miimu_miimu_sw_en  , 0x1C000C04
 .equ Adr_NX50_miimu_sw_en        , 0x1C000C04
 .equ Adr_NX50_miimu_miimu_sw_mdc , 0x1C000C08
 .equ Adr_NX50_miimu_sw_mdc       , 0x1C000C08
 .equ Adr_NX50_miimu_miimu_sw_mdo , 0x1C000C0C
 .equ Adr_NX50_miimu_sw_mdo       , 0x1C000C0C
 .equ Adr_NX50_miimu_miimu_sw_mdoe, 0x1C000C10
 .equ Adr_NX50_miimu_sw_mdoe      , 0x1C000C10
 .equ Adr_NX50_miimu_miimu_sw_mdi , 0x1C000C14
 .equ Adr_NX50_miimu_sw_mdi       , 0x1C000C14

@ =====================================================================
@
@ Area spi
 .equ Addr_NX50_spi0, 0x1C000D00
 .equ Addr_NX50_spi1, 0x1C000D40
@
@ =====================================================================
 .equ Adr_NX50_spi0_spi_cr0                       , 0x1C000D00
 .equ Adr_NX50_spi1_spi_cr0                       , 0x1C000D40
 .equ Adr_NX50_spi0_spi_cr1                       , 0x1C000D04
 .equ Adr_NX50_spi1_spi_cr1                       , 0x1C000D44
 .equ Adr_NX50_spi0_spi_dr                        , 0x1C000D08
 .equ Adr_NX50_spi1_spi_dr                        , 0x1C000D48
 .equ Adr_NX50_spi0_spi_sr                        , 0x1C000D0C
 .equ Adr_NX50_spi1_spi_sr                        , 0x1C000D4C
 .equ Adr_NX50_spi0_spi_cpsr                      , 0x1C000D10
 .equ Adr_NX50_spi1_spi_cpsr                      , 0x1C000D50
 .equ Adr_NX50_spi0_spi_imsc                      , 0x1C000D14
 .equ Adr_NX50_spi1_spi_imsc                      , 0x1C000D54
 .equ Adr_NX50_spi0_spi_ris                       , 0x1C000D18
 .equ Adr_NX50_spi1_spi_ris                       , 0x1C000D58
 .equ Adr_NX50_spi0_spi_mis                       , 0x1C000D1C
 .equ Adr_NX50_spi1_spi_mis                       , 0x1C000D5C
 .equ Adr_NX50_spi0_spi_icr                       , 0x1C000D20
 .equ Adr_NX50_spi1_spi_icr                       , 0x1C000D60
 .equ Adr_NX50_spi0_spi_dmacr                     , 0x1C000D24
 .equ Adr_NX50_spi1_spi_dmacr                     , 0x1C000D64
 .equ Adr_NX50_spi0_spi_data_register             , 0x1C000D30
 .equ Adr_NX50_spi1_spi_data_register             , 0x1C000D70
 .equ Adr_NX50_spi0_spi_status_register           , 0x1C000D34
 .equ Adr_NX50_spi1_spi_status_register           , 0x1C000D74
 .equ Adr_NX50_spi0_spi_control_register          , 0x1C000D38
 .equ Adr_NX50_spi1_spi_control_register          , 0x1C000D78
 .equ Adr_NX50_spi0_spi_interrupt_control_register, 0x1C000D3C
 .equ Adr_NX50_spi1_spi_interrupt_control_register, 0x1C000D7C

@ =====================================================================
@
@ Area i2c
 .equ Addr_NX50_i2c, 0x1C000E00
@
@ =====================================================================
 .equ Adr_NX50_i2c_i2c_mcr     , 0x1C000E00
 .equ Adr_NX50_i2c_mcr         , 0x1C000E00
 .equ Adr_NX50_i2c_i2c_scr     , 0x1C000E04
 .equ Adr_NX50_i2c_scr         , 0x1C000E04
 .equ Adr_NX50_i2c_i2c_cmd     , 0x1C000E08
 .equ Adr_NX50_i2c_cmd         , 0x1C000E08
 .equ Adr_NX50_i2c_i2c_mdr     , 0x1C000E0C
 .equ Adr_NX50_i2c_mdr         , 0x1C000E0C
 .equ Adr_NX50_i2c_i2c_sdr     , 0x1C000E10
 .equ Adr_NX50_i2c_sdr         , 0x1C000E10
 .equ Adr_NX50_i2c_i2c_mfifo_cr, 0x1C000E14
 .equ Adr_NX50_i2c_mfifo_cr    , 0x1C000E14
 .equ Adr_NX50_i2c_i2c_sfifo_cr, 0x1C000E18
 .equ Adr_NX50_i2c_sfifo_cr    , 0x1C000E18
 .equ Adr_NX50_i2c_i2c_sr      , 0x1C000E1C
 .equ Adr_NX50_i2c_sr          , 0x1C000E1C
 .equ Adr_NX50_i2c_i2c_irqmsk  , 0x1C000E20
 .equ Adr_NX50_i2c_irqmsk      , 0x1C000E20
 .equ Adr_NX50_i2c_i2c_irqsr   , 0x1C000E24
 .equ Adr_NX50_i2c_irqsr       , 0x1C000E24
 .equ Adr_NX50_i2c_i2c_irqmsked, 0x1C000E28
 .equ Adr_NX50_i2c_irqmsked    , 0x1C000E28
 .equ Adr_NX50_i2c_i2c_dmacr   , 0x1C000E2C
 .equ Adr_NX50_i2c_dmacr       , 0x1C000E2C

@ =====================================================================
@
@ Area ccdc
 .equ Addr_NX50_ccdc, 0x1C000F00
@
@ =====================================================================
 .equ Adr_NX50_ccdc_ccdc_config             , 0x1C000F00
 .equ Adr_NX50_ccdc_config                  , 0x1C000F00
 .equ Adr_NX50_ccdc_ccdc_horiz_start_stop   , 0x1C000F04
 .equ Adr_NX50_ccdc_horiz_start_stop        , 0x1C000F04
 .equ Adr_NX50_ccdc_ccdc_verti_start_stop   , 0x1C000F08
 .equ Adr_NX50_ccdc_verti_start_stop        , 0x1C000F08
 .equ Adr_NX50_ccdc_ccdc_horiz_verti_counter, 0x1C000F0C
 .equ Adr_NX50_ccdc_horiz_verti_counter     , 0x1C000F0C
 .equ Adr_NX50_ccdc_ccdc_brightness         , 0x1C000F10
 .equ Adr_NX50_ccdc_brightness              , 0x1C000F10
 .equ Adr_NX50_ccdc_ccdc_fifo_0             , 0x1C000F14
 .equ Adr_NX50_ccdc_fifo_0                  , 0x1C000F14
 .equ Adr_NX50_ccdc_ccdc_fifo_1             , 0x1C000F18
 .equ Adr_NX50_ccdc_fifo_1                  , 0x1C000F18
 .equ Adr_NX50_ccdc_ccdc_fifo_2             , 0x1C000F1C
 .equ Adr_NX50_ccdc_fifo_2                  , 0x1C000F1C
 .equ Adr_NX50_ccdc_ccdc_byte_0_pos         , 0x1C000F20
 .equ Adr_NX50_ccdc_byte_0_pos              , 0x1C000F20
 .equ Adr_NX50_ccdc_ccdc_byte_1_pos         , 0x1C000F24
 .equ Adr_NX50_ccdc_byte_1_pos              , 0x1C000F24
 .equ Adr_NX50_ccdc_ccdc_byte_2_pos         , 0x1C000F28
 .equ Adr_NX50_ccdc_byte_2_pos              , 0x1C000F28

@ =====================================================================
@
@ Area crc
 .equ Addr_NX50_crc, 0x1C001000
@
@ =====================================================================
 .equ Adr_NX50_crc_crc_crc       , 0x1C001000
 .equ Adr_NX50_crc_crc           , 0x1C001000
 .equ Adr_NX50_crc_crc_data_in   , 0x1C001004
 .equ Adr_NX50_crc_data_in       , 0x1C001004
 .equ Adr_NX50_crc_crc_polynomial, 0x1C001008
 .equ Adr_NX50_crc_polynomial    , 0x1C001008
 .equ Adr_NX50_crc_crc_config    , 0x1C00100C
 .equ Adr_NX50_crc_config        , 0x1C00100C

@ =====================================================================
@
@ Area systime
 .equ Addr_NX50_systime, 0x1C001100
@
@ =====================================================================
 .equ Adr_NX50_systime_systime_ns              , 0x1C001100
 .equ Adr_NX50_systime_ns                      , 0x1C001100
 .equ Adr_NX50_systime_systime_s               , 0x1C001104
 .equ Adr_NX50_systime_s                       , 0x1C001104
 .equ Adr_NX50_systime_systime_border          , 0x1C001108
 .equ Adr_NX50_systime_border                  , 0x1C001108
 .equ Adr_NX50_systime_systime_count_value     , 0x1C00110C
 .equ Adr_NX50_systime_count_value             , 0x1C00110C
 .equ Adr_NX50_systime_systime_s_compare_value , 0x1C001110
 .equ Adr_NX50_systime_s_compare_value         , 0x1C001110
 .equ Adr_NX50_systime_systime_s_compare_enable, 0x1C001114
 .equ Adr_NX50_systime_s_compare_enable        , 0x1C001114
 .equ Adr_NX50_systime_systime_s_compare_irq   , 0x1C001118
 .equ Adr_NX50_systime_s_compare_irq           , 0x1C001118

@ =====================================================================
@
@ Area mmio_ctrl
 .equ Addr_NX50_mmio_ctrl, 0x1C001300
@
@ =====================================================================
 .equ Adr_NX50_mmio_ctrl_mmio0_cfg , 0x1C001300
 .equ Adr_NX50_mmio0_cfg           , 0x1C001300
 .equ Adr_NX50_mmio_ctrl_mmio1_cfg , 0x1C001304
 .equ Adr_NX50_mmio1_cfg           , 0x1C001304
 .equ Adr_NX50_mmio_ctrl_mmio2_cfg , 0x1C001308
 .equ Adr_NX50_mmio2_cfg           , 0x1C001308
 .equ Adr_NX50_mmio_ctrl_mmio3_cfg , 0x1C00130C
 .equ Adr_NX50_mmio3_cfg           , 0x1C00130C
 .equ Adr_NX50_mmio_ctrl_mmio4_cfg , 0x1C001310
 .equ Adr_NX50_mmio4_cfg           , 0x1C001310
 .equ Adr_NX50_mmio_ctrl_mmio5_cfg , 0x1C001314
 .equ Adr_NX50_mmio5_cfg           , 0x1C001314
 .equ Adr_NX50_mmio_ctrl_mmio6_cfg , 0x1C001318
 .equ Adr_NX50_mmio6_cfg           , 0x1C001318
 .equ Adr_NX50_mmio_ctrl_mmio7_cfg , 0x1C00131C
 .equ Adr_NX50_mmio7_cfg           , 0x1C00131C
 .equ Adr_NX50_mmio_ctrl_mmio8_cfg , 0x1C001320
 .equ Adr_NX50_mmio8_cfg           , 0x1C001320
 .equ Adr_NX50_mmio_ctrl_mmio9_cfg , 0x1C001324
 .equ Adr_NX50_mmio9_cfg           , 0x1C001324
 .equ Adr_NX50_mmio_ctrl_mmio10_cfg, 0x1C001328
 .equ Adr_NX50_mmio10_cfg          , 0x1C001328
 .equ Adr_NX50_mmio_ctrl_mmio11_cfg, 0x1C00132C
 .equ Adr_NX50_mmio11_cfg          , 0x1C00132C
 .equ Adr_NX50_mmio_ctrl_mmio12_cfg, 0x1C001330
 .equ Adr_NX50_mmio12_cfg          , 0x1C001330
 .equ Adr_NX50_mmio_ctrl_mmio13_cfg, 0x1C001334
 .equ Adr_NX50_mmio13_cfg          , 0x1C001334
 .equ Adr_NX50_mmio_ctrl_mmio14_cfg, 0x1C001338
 .equ Adr_NX50_mmio14_cfg          , 0x1C001338
 .equ Adr_NX50_mmio_ctrl_mmio15_cfg, 0x1C00133C
 .equ Adr_NX50_mmio15_cfg          , 0x1C00133C
 .equ Adr_NX50_mmio_ctrl_mmio16_cfg, 0x1C001340
 .equ Adr_NX50_mmio16_cfg          , 0x1C001340
 .equ Adr_NX50_mmio_ctrl_mmio17_cfg, 0x1C001344
 .equ Adr_NX50_mmio17_cfg          , 0x1C001344
 .equ Adr_NX50_mmio_ctrl_mmio18_cfg, 0x1C001348
 .equ Adr_NX50_mmio18_cfg          , 0x1C001348
 .equ Adr_NX50_mmio_ctrl_mmio19_cfg, 0x1C00134C
 .equ Adr_NX50_mmio19_cfg          , 0x1C00134C
 .equ Adr_NX50_mmio_ctrl_mmio20_cfg, 0x1C001350
 .equ Adr_NX50_mmio20_cfg          , 0x1C001350
 .equ Adr_NX50_mmio_ctrl_mmio21_cfg, 0x1C001354
 .equ Adr_NX50_mmio21_cfg          , 0x1C001354
 .equ Adr_NX50_mmio_ctrl_mmio22_cfg, 0x1C001358
 .equ Adr_NX50_mmio22_cfg          , 0x1C001358
 .equ Adr_NX50_mmio_ctrl_mmio23_cfg, 0x1C00135C
 .equ Adr_NX50_mmio23_cfg          , 0x1C00135C
 .equ Adr_NX50_mmio_ctrl_mmio24_cfg, 0x1C001360
 .equ Adr_NX50_mmio24_cfg          , 0x1C001360
 .equ Adr_NX50_mmio_ctrl_mmio25_cfg, 0x1C001364
 .equ Adr_NX50_mmio25_cfg          , 0x1C001364
 .equ Adr_NX50_mmio_ctrl_mmio26_cfg, 0x1C001368
 .equ Adr_NX50_mmio26_cfg          , 0x1C001368
 .equ Adr_NX50_mmio_ctrl_mmio27_cfg, 0x1C00136C
 .equ Adr_NX50_mmio27_cfg          , 0x1C00136C
 .equ Adr_NX50_mmio_ctrl_mmio28_cfg, 0x1C001370
 .equ Adr_NX50_mmio28_cfg          , 0x1C001370
 .equ Adr_NX50_mmio_ctrl_mmio29_cfg, 0x1C001374
 .equ Adr_NX50_mmio29_cfg          , 0x1C001374
 .equ Adr_NX50_mmio_ctrl_mmio30_cfg, 0x1C001378
 .equ Adr_NX50_mmio30_cfg          , 0x1C001378
 .equ Adr_NX50_mmio_ctrl_mmio31_cfg, 0x1C00137C
 .equ Adr_NX50_mmio31_cfg          , 0x1C00137C
 .equ Adr_NX50_mmio_ctrl_mmio32_cfg, 0x1C001380
 .equ Adr_NX50_mmio32_cfg          , 0x1C001380
 .equ Adr_NX50_mmio_ctrl_mmio33_cfg, 0x1C001384
 .equ Adr_NX50_mmio33_cfg          , 0x1C001384
 .equ Adr_NX50_mmio_ctrl_mmio34_cfg, 0x1C001388
 .equ Adr_NX50_mmio34_cfg          , 0x1C001388
 .equ Adr_NX50_mmio_ctrl_mmio35_cfg, 0x1C00138C
 .equ Adr_NX50_mmio35_cfg          , 0x1C00138C
 .equ Adr_NX50_mmio_ctrl_mmio36_cfg, 0x1C001390
 .equ Adr_NX50_mmio36_cfg          , 0x1C001390
 .equ Adr_NX50_mmio_ctrl_mmio37_cfg, 0x1C001394
 .equ Adr_NX50_mmio37_cfg          , 0x1C001394
 .equ Adr_NX50_mmio_ctrl_mmio38_cfg, 0x1C001398
 .equ Adr_NX50_mmio38_cfg          , 0x1C001398
 .equ Adr_NX50_mmio_ctrl_mmio39_cfg, 0x1C00139C
 .equ Adr_NX50_mmio39_cfg          , 0x1C00139C

@ =====================================================================
@
@ Area hif
 .equ Addr_NX50_hif, 0x1C003000
@
@ =====================================================================
@ =====================================================================
@
@ Area host_controlled_dma_register_block
 .equ Addr_NX50_host_controlled_dma_register_block, 0x1C003000
@
@ =====================================================================
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch0_host_start    , 0x1C003000
 .equ Adr_NX50_dpmhs_ch0_host_start                                       , 0x1C003000
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch0_netx_start    , 0x1C003004
 .equ Adr_NX50_dpmhs_ch0_netx_start                                       , 0x1C003004
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch0_dma_ctrl      , 0x1C003008
 .equ Adr_NX50_dpmhs_ch0_dma_ctrl                                         , 0x1C003008
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch0_host_mbx_base , 0x1C00300C
 .equ Adr_NX50_dpmhs_ch0_host_mbx_base                                    , 0x1C00300C
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch0_buf_ctrl      , 0x1C003010
 .equ Adr_NX50_dpmhs_ch0_buf_ctrl                                         , 0x1C003010
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch0_host_buf3_base, 0x1C003014
 .equ Adr_NX50_dpmhs_ch0_host_buf3_base                                   , 0x1C003014
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch0_host_buf2_base, 0x1C003018
 .equ Adr_NX50_dpmhs_ch0_host_buf2_base                                   , 0x1C003018
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch0_host_buf1_base, 0x1C00301C
 .equ Adr_NX50_dpmhs_ch0_host_buf1_base                                   , 0x1C00301C
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch1_host_start    , 0x1C003020
 .equ Adr_NX50_dpmhs_ch1_host_start                                       , 0x1C003020
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch1_netx_start    , 0x1C003024
 .equ Adr_NX50_dpmhs_ch1_netx_start                                       , 0x1C003024
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch1_dma_ctrl      , 0x1C003028
 .equ Adr_NX50_dpmhs_ch1_dma_ctrl                                         , 0x1C003028
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch1_host_mbx_base , 0x1C00302C
 .equ Adr_NX50_dpmhs_ch1_host_mbx_base                                    , 0x1C00302C
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch1_buf_ctrl      , 0x1C003030
 .equ Adr_NX50_dpmhs_ch1_buf_ctrl                                         , 0x1C003030
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch1_host_buf3_base, 0x1C003034
 .equ Adr_NX50_dpmhs_ch1_host_buf3_base                                   , 0x1C003034
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch1_host_buf2_base, 0x1C003038
 .equ Adr_NX50_dpmhs_ch1_host_buf2_base                                   , 0x1C003038
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch1_host_buf1_base, 0x1C00303C
 .equ Adr_NX50_dpmhs_ch1_host_buf1_base                                   , 0x1C00303C
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch2_host_start    , 0x1C003040
 .equ Adr_NX50_dpmhs_ch2_host_start                                       , 0x1C003040
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch2_netx_start    , 0x1C003044
 .equ Adr_NX50_dpmhs_ch2_netx_start                                       , 0x1C003044
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch2_dma_ctrl      , 0x1C003048
 .equ Adr_NX50_dpmhs_ch2_dma_ctrl                                         , 0x1C003048
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch2_host_mbx_base , 0x1C00304C
 .equ Adr_NX50_dpmhs_ch2_host_mbx_base                                    , 0x1C00304C
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch2_buf_ctrl      , 0x1C003050
 .equ Adr_NX50_dpmhs_ch2_buf_ctrl                                         , 0x1C003050
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch2_host_buf3_base, 0x1C003054
 .equ Adr_NX50_dpmhs_ch2_host_buf3_base                                   , 0x1C003054
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch2_host_buf2_base, 0x1C003058
 .equ Adr_NX50_dpmhs_ch2_host_buf2_base                                   , 0x1C003058
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch2_host_buf1_base, 0x1C00305C
 .equ Adr_NX50_dpmhs_ch2_host_buf1_base                                   , 0x1C00305C
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch3_host_start    , 0x1C003060
 .equ Adr_NX50_dpmhs_ch3_host_start                                       , 0x1C003060
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch3_netx_start    , 0x1C003064
 .equ Adr_NX50_dpmhs_ch3_netx_start                                       , 0x1C003064
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch3_dma_ctrl      , 0x1C003068
 .equ Adr_NX50_dpmhs_ch3_dma_ctrl                                         , 0x1C003068
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch3_host_mbx_base , 0x1C00306C
 .equ Adr_NX50_dpmhs_ch3_host_mbx_base                                    , 0x1C00306C
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch3_buf_ctrl      , 0x1C003070
 .equ Adr_NX50_dpmhs_ch3_buf_ctrl                                         , 0x1C003070
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch3_host_buf3_base, 0x1C003074
 .equ Adr_NX50_dpmhs_ch3_host_buf3_base                                   , 0x1C003074
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch3_host_buf2_base, 0x1C003078
 .equ Adr_NX50_dpmhs_ch3_host_buf2_base                                   , 0x1C003078
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch3_host_buf1_base, 0x1C00307C
 .equ Adr_NX50_dpmhs_ch3_host_buf1_base                                   , 0x1C00307C
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch4_host_start    , 0x1C003080
 .equ Adr_NX50_dpmhs_ch4_host_start                                       , 0x1C003080
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch4_netx_start    , 0x1C003084
 .equ Adr_NX50_dpmhs_ch4_netx_start                                       , 0x1C003084
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch4_dma_ctrl      , 0x1C003088
 .equ Adr_NX50_dpmhs_ch4_dma_ctrl                                         , 0x1C003088
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch4_host_mbx_base , 0x1C00308C
 .equ Adr_NX50_dpmhs_ch4_host_mbx_base                                    , 0x1C00308C
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch4_buf_ctrl      , 0x1C003090
 .equ Adr_NX50_dpmhs_ch4_buf_ctrl                                         , 0x1C003090
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch4_host_buf3_base, 0x1C003094
 .equ Adr_NX50_dpmhs_ch4_host_buf3_base                                   , 0x1C003094
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch4_host_buf2_base, 0x1C003098
 .equ Adr_NX50_dpmhs_ch4_host_buf2_base                                   , 0x1C003098
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch4_host_buf1_base, 0x1C00309C
 .equ Adr_NX50_dpmhs_ch4_host_buf1_base                                   , 0x1C00309C
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch5_host_start    , 0x1C0030A0
 .equ Adr_NX50_dpmhs_ch5_host_start                                       , 0x1C0030A0
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch5_netx_start    , 0x1C0030A4
 .equ Adr_NX50_dpmhs_ch5_netx_start                                       , 0x1C0030A4
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch5_dma_ctrl      , 0x1C0030A8
 .equ Adr_NX50_dpmhs_ch5_dma_ctrl                                         , 0x1C0030A8
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch5_host_mbx_base , 0x1C0030AC
 .equ Adr_NX50_dpmhs_ch5_host_mbx_base                                    , 0x1C0030AC
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch5_buf_ctrl      , 0x1C0030B0
 .equ Adr_NX50_dpmhs_ch5_buf_ctrl                                         , 0x1C0030B0
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch5_host_buf3_base, 0x1C0030B4
 .equ Adr_NX50_dpmhs_ch5_host_buf3_base                                   , 0x1C0030B4
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch5_host_buf2_base, 0x1C0030B8
 .equ Adr_NX50_dpmhs_ch5_host_buf2_base                                   , 0x1C0030B8
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch5_host_buf1_base, 0x1C0030BC
 .equ Adr_NX50_dpmhs_ch5_host_buf1_base                                   , 0x1C0030BC
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch6_host_start    , 0x1C0030C0
 .equ Adr_NX50_dpmhs_ch6_host_start                                       , 0x1C0030C0
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch6_netx_start    , 0x1C0030C4
 .equ Adr_NX50_dpmhs_ch6_netx_start                                       , 0x1C0030C4
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch6_dma_ctrl      , 0x1C0030C8
 .equ Adr_NX50_dpmhs_ch6_dma_ctrl                                         , 0x1C0030C8
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch6_host_mbx_base , 0x1C0030CC
 .equ Adr_NX50_dpmhs_ch6_host_mbx_base                                    , 0x1C0030CC
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch6_buf_ctrl      , 0x1C0030D0
 .equ Adr_NX50_dpmhs_ch6_buf_ctrl                                         , 0x1C0030D0
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch6_host_buf3_base, 0x1C0030D4
 .equ Adr_NX50_dpmhs_ch6_host_buf3_base                                   , 0x1C0030D4
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch6_host_buf2_base, 0x1C0030D8
 .equ Adr_NX50_dpmhs_ch6_host_buf2_base                                   , 0x1C0030D8
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch6_host_buf1_base, 0x1C0030DC
 .equ Adr_NX50_dpmhs_ch6_host_buf1_base                                   , 0x1C0030DC
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch7_host_start    , 0x1C0030E0
 .equ Adr_NX50_dpmhs_ch7_host_start                                       , 0x1C0030E0
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch7_netx_start    , 0x1C0030E4
 .equ Adr_NX50_dpmhs_ch7_netx_start                                       , 0x1C0030E4
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch7_dma_ctrl      , 0x1C0030E8
 .equ Adr_NX50_dpmhs_ch7_dma_ctrl                                         , 0x1C0030E8
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch7_host_mbx_base , 0x1C0030EC
 .equ Adr_NX50_dpmhs_ch7_host_mbx_base                                    , 0x1C0030EC
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch7_buf_ctrl      , 0x1C0030F0
 .equ Adr_NX50_dpmhs_ch7_buf_ctrl                                         , 0x1C0030F0
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch7_host_buf3_base, 0x1C0030F4
 .equ Adr_NX50_dpmhs_ch7_host_buf3_base                                   , 0x1C0030F4
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch7_host_buf2_base, 0x1C0030F8
 .equ Adr_NX50_dpmhs_ch7_host_buf2_base                                   , 0x1C0030F8
 .equ Adr_NX50_host_controlled_dma_register_block_dpmhs_ch7_host_buf1_base, 0x1C0030FC
 .equ Adr_NX50_dpmhs_ch7_host_buf1_base                                   , 0x1C0030FC

@ =====================================================================
@
@ Area host_controlled_global_register_block
 .equ Addr_NX50_host_controlled_global_register_block, 0x1C003100
@
@ =====================================================================
 .equ Adr_NX50_host_controlled_global_register_block_pci_base             , 0x1C0031BC
 .equ Adr_NX50_pci_base                                                   , 0x1C0031BC
 .equ Adr_NX50_host_controlled_global_register_block_wdg_host             , 0x1C0031C0
 .equ Adr_NX50_wdg_host                                                   , 0x1C0031C0
 .equ Adr_NX50_host_controlled_global_register_block_dpmhs_wdg_host_trig  , 0x1C0031C4
 .equ Adr_NX50_dpmhs_wdg_host_trig                                        , 0x1C0031C4
 .equ Adr_NX50_host_controlled_global_register_block_dpmhs_wdg_arm_timeout, 0x1C0031C8
 .equ Adr_NX50_dpmhs_wdg_arm_timeout                                      , 0x1C0031C8
 .equ Adr_NX50_host_controlled_global_register_block_cyclic_tmr_control   , 0x1C0031D0
 .equ Adr_NX50_cyclic_tmr_control                                         , 0x1C0031D0
 .equ Adr_NX50_host_controlled_global_register_block_cyclic_tmr_reload    , 0x1C0031D4
 .equ Adr_NX50_cyclic_tmr_reload                                          , 0x1C0031D4
 .equ Adr_NX50_host_controlled_global_register_block_sta_host             , 0x1C0031D8
 .equ Adr_NX50_sta_host                                                   , 0x1C0031D8
 .equ Adr_NX50_host_controlled_global_register_block_res_reg              , 0x1C0031DC
 .equ Adr_NX50_res_reg                                                    , 0x1C0031DC
 .equ Adr_NX50_host_controlled_global_register_block_irq_reg_host         , 0x1C0031E0
 .equ Adr_NX50_irq_reg_host                                               , 0x1C0031E0
 .equ Adr_NX50_host_controlled_global_register_block_irq_reg_host1        , 0x1C0031E4
 .equ Adr_NX50_irq_reg_host1                                              , 0x1C0031E4
 .equ Adr_NX50_host_controlled_global_register_block_dpmhs_int_en0        , 0x1C0031F0
 .equ Adr_NX50_dpmhs_int_en0                                              , 0x1C0031F0
 .equ Adr_NX50_host_controlled_global_register_block_adr_dpmhs_int_en1    , 0x1C0031F4
 .equ Adr_NX50_adr_dpmhs_int_en1                                          , 0x1C0031F4

@ =====================================================================
@
@ Area host_controlled_handshake_register_block
 .equ Addr_NX50_host_controlled_handshake_register_block, 0x1C003200
@
@ =====================================================================
 .equ Adr_NX50_host_controlled_handshake_register_block_dpmhs_hs_data0 , 0x1C003200
 .equ Adr_NX50_dpmhs_hs_data0                                          , 0x1C003200
 .equ Adr_NX50_host_controlled_handshake_register_block_dpmhs_hs_data1 , 0x1C003204
 .equ Adr_NX50_dpmhs_hs_data1                                          , 0x1C003204
 .equ Adr_NX50_host_controlled_handshake_register_block_dpmhs_hs_data2 , 0x1C003208
 .equ Adr_NX50_dpmhs_hs_data2                                          , 0x1C003208
 .equ Adr_NX50_host_controlled_handshake_register_block_dpmhs_hs_data3 , 0x1C00320C
 .equ Adr_NX50_dpmhs_hs_data3                                          , 0x1C00320C
 .equ Adr_NX50_host_controlled_handshake_register_block_dpmhs_hs_data4 , 0x1C003210
 .equ Adr_NX50_dpmhs_hs_data4                                          , 0x1C003210
 .equ Adr_NX50_host_controlled_handshake_register_block_dpmhs_hs_data5 , 0x1C003214
 .equ Adr_NX50_dpmhs_hs_data5                                          , 0x1C003214
 .equ Adr_NX50_host_controlled_handshake_register_block_dpmhs_hs_data6 , 0x1C003218
 .equ Adr_NX50_dpmhs_hs_data6                                          , 0x1C003218
 .equ Adr_NX50_host_controlled_handshake_register_block_dpmhs_hs_data7 , 0x1C00321C
 .equ Adr_NX50_dpmhs_hs_data7                                          , 0x1C00321C
 .equ Adr_NX50_host_controlled_handshake_register_block_dpmhs_hs_data8 , 0x1C003220
 .equ Adr_NX50_dpmhs_hs_data8                                          , 0x1C003220
 .equ Adr_NX50_host_controlled_handshake_register_block_dpmhs_hs_data9 , 0x1C003224
 .equ Adr_NX50_dpmhs_hs_data9                                          , 0x1C003224
 .equ Adr_NX50_host_controlled_handshake_register_block_dpmhs_hs_data10, 0x1C003228
 .equ Adr_NX50_dpmhs_hs_data10                                         , 0x1C003228
 .equ Adr_NX50_host_controlled_handshake_register_block_dpmhs_hs_data11, 0x1C00322C
 .equ Adr_NX50_dpmhs_hs_data11                                         , 0x1C00322C
 .equ Adr_NX50_host_controlled_handshake_register_block_dpmhs_hs_data12, 0x1C003230
 .equ Adr_NX50_dpmhs_hs_data12                                         , 0x1C003230
 .equ Adr_NX50_host_controlled_handshake_register_block_dpmhs_hs_data13, 0x1C003234
 .equ Adr_NX50_dpmhs_hs_data13                                         , 0x1C003234
 .equ Adr_NX50_host_controlled_handshake_register_block_dpmhs_hs_data14, 0x1C003238
 .equ Adr_NX50_dpmhs_hs_data14                                         , 0x1C003238
 .equ Adr_NX50_host_controlled_handshake_register_block_dpmhs_hs_data15, 0x1C00323C
 .equ Adr_NX50_dpmhs_hs_data15                                         , 0x1C00323C

@ =====================================================================
@
@ Area netx_controlled_dma_register_block
 .equ Addr_NX50_netx_controlled_dma_register_block, 0x1C003300
@
@ =====================================================================
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch0_host_start    , 0x1C003300
 .equ Adr_NX50_dpmas_ch0_host_start                                       , 0x1C003300
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch0_netx_start    , 0x1C003304
 .equ Adr_NX50_dpmas_ch0_netx_start                                       , 0x1C003304
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch0_dma_ctrl      , 0x1C003308
 .equ Adr_NX50_dpmas_ch0_dma_ctrl                                         , 0x1C003308
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch0_mbx_base      , 0x1C00330C
 .equ Adr_NX50_dpmas_ch0_mbx_base                                         , 0x1C00330C
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch0_buf_ctrl      , 0x1C003310
 .equ Adr_NX50_dpmas_ch0_buf_ctrl                                         , 0x1C003310
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch0_netx_buf_base3, 0x1C003314
 .equ Adr_NX50_dpmas_ch0_netx_buf_base3                                   , 0x1C003314
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch0_netx_buf_base2, 0x1C003318
 .equ Adr_NX50_dpmas_ch0_netx_buf_base2                                   , 0x1C003318
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch0_netx_buf_base1, 0x1C00331C
 .equ Adr_NX50_dpmas_ch0_netx_buf_base1                                   , 0x1C00331C
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch1_host_start    , 0x1C003320
 .equ Adr_NX50_dpmas_ch1_host_start                                       , 0x1C003320
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch1_netx_start    , 0x1C003324
 .equ Adr_NX50_dpmas_ch1_netx_start                                       , 0x1C003324
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch1_dma_ctrl      , 0x1C003328
 .equ Adr_NX50_dpmas_ch1_dma_ctrl                                         , 0x1C003328
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch1_mbx_base      , 0x1C00332C
 .equ Adr_NX50_dpmas_ch1_mbx_base                                         , 0x1C00332C
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch1_buf_ctrl      , 0x1C003330
 .equ Adr_NX50_dpmas_ch1_buf_ctrl                                         , 0x1C003330
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch1_netx_buf_base3, 0x1C003334
 .equ Adr_NX50_dpmas_ch1_netx_buf_base3                                   , 0x1C003334
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch1_netx_buf_base2, 0x1C003338
 .equ Adr_NX50_dpmas_ch1_netx_buf_base2                                   , 0x1C003338
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch1_netx_buf_base1, 0x1C00333C
 .equ Adr_NX50_dpmas_ch1_netx_buf_base1                                   , 0x1C00333C
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch2_host_start    , 0x1C003340
 .equ Adr_NX50_dpmas_ch2_host_start                                       , 0x1C003340
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch2_netx_start    , 0x1C003344
 .equ Adr_NX50_dpmas_ch2_netx_start                                       , 0x1C003344
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch2_dma_ctrl      , 0x1C003348
 .equ Adr_NX50_dpmas_ch2_dma_ctrl                                         , 0x1C003348
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch2_mbx_base      , 0x1C00334C
 .equ Adr_NX50_dpmas_ch2_mbx_base                                         , 0x1C00334C
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch2_buf_ctrl      , 0x1C003350
 .equ Adr_NX50_dpmas_ch2_buf_ctrl                                         , 0x1C003350
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch2_netx_buf_base3, 0x1C003354
 .equ Adr_NX50_dpmas_ch2_netx_buf_base3                                   , 0x1C003354
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch2_netx_buf_base2, 0x1C003358
 .equ Adr_NX50_dpmas_ch2_netx_buf_base2                                   , 0x1C003358
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch2_netx_buf_base1, 0x1C00335C
 .equ Adr_NX50_dpmas_ch2_netx_buf_base1                                   , 0x1C00335C
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch3_host_start    , 0x1C003360
 .equ Adr_NX50_dpmas_ch3_host_start                                       , 0x1C003360
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch3_netx_start    , 0x1C003364
 .equ Adr_NX50_dpmas_ch3_netx_start                                       , 0x1C003364
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch3_dma_ctrl      , 0x1C003368
 .equ Adr_NX50_dpmas_ch3_dma_ctrl                                         , 0x1C003368
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch3_mbx_base      , 0x1C00336C
 .equ Adr_NX50_dpmas_ch3_mbx_base                                         , 0x1C00336C
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch3_buf_ctrl      , 0x1C003370
 .equ Adr_NX50_dpmas_ch3_buf_ctrl                                         , 0x1C003370
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch3_netx_buf_base3, 0x1C003374
 .equ Adr_NX50_dpmas_ch3_netx_buf_base3                                   , 0x1C003374
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch3_netx_buf_base2, 0x1C003378
 .equ Adr_NX50_dpmas_ch3_netx_buf_base2                                   , 0x1C003378
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch3_netx_buf_base1, 0x1C00337C
 .equ Adr_NX50_dpmas_ch3_netx_buf_base1                                   , 0x1C00337C
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch4_host_start    , 0x1C003380
 .equ Adr_NX50_dpmas_ch4_host_start                                       , 0x1C003380
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch4_netx_start    , 0x1C003384
 .equ Adr_NX50_dpmas_ch4_netx_start                                       , 0x1C003384
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch4_dma_ctrl      , 0x1C003388
 .equ Adr_NX50_dpmas_ch4_dma_ctrl                                         , 0x1C003388
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch4_mbx_base      , 0x1C00338C
 .equ Adr_NX50_dpmas_ch4_mbx_base                                         , 0x1C00338C
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch4_buf_ctrl      , 0x1C003390
 .equ Adr_NX50_dpmas_ch4_buf_ctrl                                         , 0x1C003390
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch4_netx_buf_base3, 0x1C003394
 .equ Adr_NX50_dpmas_ch4_netx_buf_base3                                   , 0x1C003394
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch4_netx_buf_base2, 0x1C003398
 .equ Adr_NX50_dpmas_ch4_netx_buf_base2                                   , 0x1C003398
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch4_netx_buf_base1, 0x1C00339C
 .equ Adr_NX50_dpmas_ch4_netx_buf_base1                                   , 0x1C00339C
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch5_host_start    , 0x1C0033A0
 .equ Adr_NX50_dpmas_ch5_host_start                                       , 0x1C0033A0
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch5_netx_start    , 0x1C0033A4
 .equ Adr_NX50_dpmas_ch5_netx_start                                       , 0x1C0033A4
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch5_dma_ctrl      , 0x1C0033A8
 .equ Adr_NX50_dpmas_ch5_dma_ctrl                                         , 0x1C0033A8
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch5_mbx_base      , 0x1C0033AC
 .equ Adr_NX50_dpmas_ch5_mbx_base                                         , 0x1C0033AC
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch5_buf_ctrl      , 0x1C0033B0
 .equ Adr_NX50_dpmas_ch5_buf_ctrl                                         , 0x1C0033B0
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch5_netx_buf_base3, 0x1C0033B4
 .equ Adr_NX50_dpmas_ch5_netx_buf_base3                                   , 0x1C0033B4
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch5_netx_buf_base2, 0x1C0033B8
 .equ Adr_NX50_dpmas_ch5_netx_buf_base2                                   , 0x1C0033B8
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch5_netx_buf_base1, 0x1C0033BC
 .equ Adr_NX50_dpmas_ch5_netx_buf_base1                                   , 0x1C0033BC
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch6_host_start    , 0x1C0033C0
 .equ Adr_NX50_dpmas_ch6_host_start                                       , 0x1C0033C0
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch6_netx_start    , 0x1C0033C4
 .equ Adr_NX50_dpmas_ch6_netx_start                                       , 0x1C0033C4
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch6_dma_ctrl      , 0x1C0033C8
 .equ Adr_NX50_dpmas_ch6_dma_ctrl                                         , 0x1C0033C8
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch6_mbx_base      , 0x1C0033CC
 .equ Adr_NX50_dpmas_ch6_mbx_base                                         , 0x1C0033CC
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch6_buf_ctrl      , 0x1C0033D0
 .equ Adr_NX50_dpmas_ch6_buf_ctrl                                         , 0x1C0033D0
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch6_netx_buf_base3, 0x1C0033D4
 .equ Adr_NX50_dpmas_ch6_netx_buf_base3                                   , 0x1C0033D4
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch6_netx_buf_base2, 0x1C0033D8
 .equ Adr_NX50_dpmas_ch6_netx_buf_base2                                   , 0x1C0033D8
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch6_netx_buf_base1, 0x1C0033DC
 .equ Adr_NX50_dpmas_ch6_netx_buf_base1                                   , 0x1C0033DC
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch7_host_start    , 0x1C0033E0
 .equ Adr_NX50_dpmas_ch7_host_start                                       , 0x1C0033E0
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch7_netx_start    , 0x1C0033E4
 .equ Adr_NX50_dpmas_ch7_netx_start                                       , 0x1C0033E4
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch7_dma_ctrl      , 0x1C0033E8
 .equ Adr_NX50_dpmas_ch7_dma_ctrl                                         , 0x1C0033E8
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch7_mbx_base      , 0x1C0033EC
 .equ Adr_NX50_dpmas_ch7_mbx_base                                         , 0x1C0033EC
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch7_buf_ctrl      , 0x1C0033F0
 .equ Adr_NX50_dpmas_ch7_buf_ctrl                                         , 0x1C0033F0
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch7_netx_buf_base3, 0x1C0033F4
 .equ Adr_NX50_dpmas_ch7_netx_buf_base3                                   , 0x1C0033F4
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch7_netx_buf_base2, 0x1C0033F8
 .equ Adr_NX50_dpmas_ch7_netx_buf_base2                                   , 0x1C0033F8
 .equ Adr_NX50_netx_controlled_dma_register_block_dpmas_ch7_netx_buf_base1, 0x1C0033FC
 .equ Adr_NX50_dpmas_ch7_netx_buf_base1                                   , 0x1C0033FC

@ =====================================================================
@
@ Area netx_controlled_global_register_block_1
 .equ Addr_NX50_netx_controlled_global_register_block_1, 0x1C003400
@
@ =====================================================================
 .equ Adr_NX50_netx_controlled_global_register_block_1_pci_window_low0      , 0x1C003480
 .equ Adr_NX50_pci_window_low0                                              , 0x1C003480
 .equ Adr_NX50_netx_controlled_global_register_block_1_pci_window_high0     , 0x1C003484
 .equ Adr_NX50_pci_window_high0                                             , 0x1C003484
 .equ Adr_NX50_netx_controlled_global_register_block_1_pci_window_low1      , 0x1C003488
 .equ Adr_NX50_pci_window_low1                                              , 0x1C003488
 .equ Adr_NX50_netx_controlled_global_register_block_1_pci_window_high1     , 0x1C00348C
 .equ Adr_NX50_pci_window_high1                                             , 0x1C00348C
 .equ Adr_NX50_netx_controlled_global_register_block_1_pci_window_low2      , 0x1C003490
 .equ Adr_NX50_pci_window_low2                                              , 0x1C003490
 .equ Adr_NX50_netx_controlled_global_register_block_1_pci_window_high2     , 0x1C003494
 .equ Adr_NX50_pci_window_high2                                             , 0x1C003494
 .equ Adr_NX50_netx_controlled_global_register_block_1_pci_window_low3      , 0x1C003498
 .equ Adr_NX50_pci_window_low3                                              , 0x1C003498
 .equ Adr_NX50_netx_controlled_global_register_block_1_pci_window_high3     , 0x1C00349C
 .equ Adr_NX50_pci_window_high3                                             , 0x1C00349C
 .equ Adr_NX50_netx_controlled_global_register_block_1_pci_io_base          , 0x1C0034A0
 .equ Adr_NX50_pci_io_base                                                  , 0x1C0034A0
 .equ Adr_NX50_netx_controlled_global_register_block_1_pci_rom_base         , 0x1C0034A4
 .equ Adr_NX50_pci_rom_base                                                 , 0x1C0034A4
 .equ Adr_NX50_netx_controlled_global_register_block_1_arb_ctrl             , 0x1C0034A8
 .equ Adr_NX50_arb_ctrl                                                     , 0x1C0034A8
 .equ Adr_NX50_netx_controlled_global_register_block_1_pci_config           , 0x1C0034AC
 .equ Adr_NX50_pci_config                                                   , 0x1C0034AC
 .equ Adr_NX50_netx_controlled_global_register_block_1_sfr_base_addr        , 0x1C0034B8
 .equ Adr_NX50_sfr_base_addr                                                , 0x1C0034B8
 .equ Adr_NX50_netx_controlled_global_register_block_1_cis_base             , 0x1C0034BC
 .equ Adr_NX50_cis_base                                                     , 0x1C0034BC
 .equ Adr_NX50_netx_controlled_global_register_block_1_wdg_netx             , 0x1C0034C0
 .equ Adr_NX50_wdg_netx                                                     , 0x1C0034C0
 .equ Adr_NX50_netx_controlled_global_register_block_1_dpmas_wdg_arm_timeout, 0x1C0034C8
 .equ Adr_NX50_dpmas_wdg_arm_timeout                                        , 0x1C0034C8
 .equ Adr_NX50_netx_controlled_global_register_block_1_dpmas_wdg_arm_trig   , 0x1C0034CC
 .equ Adr_NX50_dpmas_wdg_arm_trig                                           , 0x1C0034CC
 .equ Adr_NX50_netx_controlled_global_register_block_1_sta_netx             , 0x1C0034D8
 .equ Adr_NX50_sta_netx                                                     , 0x1C0034D8
 .equ Adr_NX50_netx_controlled_global_register_block_1_irq_reg_netx         , 0x1C0034E0
 .equ Adr_NX50_irq_reg_netx                                                 , 0x1C0034E0
 .equ Adr_NX50_netx_controlled_global_register_block_1_irq_reg_netx1        , 0x1C0034E4
 .equ Adr_NX50_irq_reg_netx1                                                , 0x1C0034E4
 .equ Adr_NX50_netx_controlled_global_register_block_1_irq_reg_netx2        , 0x1C0034E8
 .equ Adr_NX50_irq_reg_netx2                                                , 0x1C0034E8
 .equ Adr_NX50_netx_controlled_global_register_block_1_irq_reg_netx3        , 0x1C0034EC
 .equ Adr_NX50_irq_reg_netx3                                                , 0x1C0034EC
 .equ Adr_NX50_netx_controlled_global_register_block_1_dpmas_int_en0        , 0x1C0034F0
 .equ Adr_NX50_dpmas_int_en0                                                , 0x1C0034F0
 .equ Adr_NX50_netx_controlled_global_register_block_1_dpmas_int_en1        , 0x1C0034F4
 .equ Adr_NX50_dpmas_int_en1                                                , 0x1C0034F4
 .equ Adr_NX50_netx_controlled_global_register_block_1_dpmas_int_en2        , 0x1C0034F8
 .equ Adr_NX50_dpmas_int_en2                                                , 0x1C0034F8
 .equ Adr_NX50_netx_controlled_global_register_block_1_dpmas_int_en3        , 0x1C0034FC
 .equ Adr_NX50_dpmas_int_en3                                                , 0x1C0034FC

@ =====================================================================
@
@ Area netx_controlled_handshake_register_block
 .equ Addr_NX50_netx_controlled_handshake_register_block, 0x1C003500
@
@ =====================================================================
 .equ Adr_NX50_netx_controlled_handshake_register_block_handshake_ch0 , 0x1C003500
 .equ Adr_NX50_handshake_ch0                                          , 0x1C003500
 .equ Adr_NX50_netx_controlled_handshake_register_block_handshake_ch1 , 0x1C003504
 .equ Adr_NX50_handshake_ch1                                          , 0x1C003504
 .equ Adr_NX50_netx_controlled_handshake_register_block_handshake_ch2 , 0x1C003508
 .equ Adr_NX50_handshake_ch2                                          , 0x1C003508
 .equ Adr_NX50_netx_controlled_handshake_register_block_handshake_ch3 , 0x1C00350C
 .equ Adr_NX50_handshake_ch3                                          , 0x1C00350C
 .equ Adr_NX50_netx_controlled_handshake_register_block_handshake_ch4 , 0x1C003510
 .equ Adr_NX50_handshake_ch4                                          , 0x1C003510
 .equ Adr_NX50_netx_controlled_handshake_register_block_handshake_ch5 , 0x1C003514
 .equ Adr_NX50_handshake_ch5                                          , 0x1C003514
 .equ Adr_NX50_netx_controlled_handshake_register_block_handshake_ch6 , 0x1C003518
 .equ Adr_NX50_handshake_ch6                                          , 0x1C003518
 .equ Adr_NX50_netx_controlled_handshake_register_block_handshake_ch7 , 0x1C00351C
 .equ Adr_NX50_handshake_ch7                                          , 0x1C00351C
 .equ Adr_NX50_netx_controlled_handshake_register_block_handshake_ch8 , 0x1C003520
 .equ Adr_NX50_handshake_ch8                                          , 0x1C003520
 .equ Adr_NX50_netx_controlled_handshake_register_block_handshake_ch9 , 0x1C003524
 .equ Adr_NX50_handshake_ch9                                          , 0x1C003524
 .equ Adr_NX50_netx_controlled_handshake_register_block_handshake_ch10, 0x1C003528
 .equ Adr_NX50_handshake_ch10                                         , 0x1C003528
 .equ Adr_NX50_netx_controlled_handshake_register_block_handshake_ch11, 0x1C00352C
 .equ Adr_NX50_handshake_ch11                                         , 0x1C00352C
 .equ Adr_NX50_netx_controlled_handshake_register_block_handshake_ch12, 0x1C003530
 .equ Adr_NX50_handshake_ch12                                         , 0x1C003530
 .equ Adr_NX50_netx_controlled_handshake_register_block_handshake_ch13, 0x1C003534
 .equ Adr_NX50_handshake_ch13                                         , 0x1C003534
 .equ Adr_NX50_netx_controlled_handshake_register_block_handshake_ch14, 0x1C003538
 .equ Adr_NX50_handshake_ch14                                         , 0x1C003538
 .equ Adr_NX50_netx_controlled_handshake_register_block_handshake_ch15, 0x1C00353C
 .equ Adr_NX50_handshake_ch15                                         , 0x1C00353C

@ =====================================================================
@
@ Area netx_controlled_global_register_block_2
 .equ Addr_NX50_netx_controlled_global_register_block_2, 0x1C003600
@
@ =====================================================================
 .equ Adr_NX50_netx_controlled_global_register_block_2_clk_reg          , 0x1C003604
 .equ Adr_NX50_clk_reg                                                  , 0x1C003604
 .equ Adr_NX50_netx_controlled_global_register_block_2_if_conf1         , 0x1C003608
 .equ Adr_NX50_if_conf1                                                 , 0x1C003608
 .equ Adr_NX50_netx_controlled_global_register_block_2_if_conf2         , 0x1C00360C
 .equ Adr_NX50_if_conf2                                                 , 0x1C00360C
 .equ Adr_NX50_netx_controlled_global_register_block_2_exp_bus_reg      , 0x1C003610
 .equ Adr_NX50_exp_bus_reg                                              , 0x1C003610
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_ext_config1, 0x1C003614
 .equ Adr_NX50_dpmas_ext_config1                                        , 0x1C003614
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_ext_config2, 0x1C003618
 .equ Adr_NX50_dpmas_ext_config2                                        , 0x1C003618
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_ext_config3, 0x1C00361C
 .equ Adr_NX50_dpmas_ext_config3                                        , 0x1C00361C
 .equ Adr_NX50_netx_controlled_global_register_block_2_io_reg_mode0     , 0x1C003620
 .equ Adr_NX50_io_reg_mode0                                             , 0x1C003620
 .equ Adr_NX50_netx_controlled_global_register_block_2_io_reg_drv_en0   , 0x1C003624
 .equ Adr_NX50_io_reg_drv_en0                                           , 0x1C003624
 .equ Adr_NX50_netx_controlled_global_register_block_2_io_reg_data0     , 0x1C003628
 .equ Adr_NX50_io_reg_data0                                             , 0x1C003628
 .equ Adr_NX50_netx_controlled_global_register_block_2_io_reg_reserved0 , 0x1C00362C
 .equ Adr_NX50_io_reg_reserved0                                         , 0x1C00362C
 .equ Adr_NX50_netx_controlled_global_register_block_2_io_reg_mode1     , 0x1C003630
 .equ Adr_NX50_io_reg_mode1                                             , 0x1C003630
 .equ Adr_NX50_netx_controlled_global_register_block_2_io_reg_drv_en1   , 0x1C003634
 .equ Adr_NX50_io_reg_drv_en1                                           , 0x1C003634
 .equ Adr_NX50_netx_controlled_global_register_block_2_io_reg_data1     , 0x1C003638
 .equ Adr_NX50_io_reg_data1                                             , 0x1C003638
 .equ Adr_NX50_netx_controlled_global_register_block_2_io_reg_reserved1 , 0x1C00363C
 .equ Adr_NX50_io_reg_reserved1                                         , 0x1C00363C
 .equ Adr_NX50_netx_controlled_global_register_block_2_mb_ctrl          , 0x1C003640
 .equ Adr_NX50_mb_ctrl                                                  , 0x1C003640
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_db_mapping0, 0x1C003644
 .equ Adr_NX50_dpmas_db_mapping0                                        , 0x1C003644
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_db_end1    , 0x1C003648
 .equ Adr_NX50_dpmas_db_end1                                            , 0x1C003648
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_db_mapping1, 0x1C00364C
 .equ Adr_NX50_dpmas_db_mapping1                                        , 0x1C00364C
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_db_end2    , 0x1C003650
 .equ Adr_NX50_dpmas_db_end2                                            , 0x1C003650
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_db_mapping2, 0x1C003654
 .equ Adr_NX50_dpmas_db_mapping2                                        , 0x1C003654
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_db_end3    , 0x1C003658
 .equ Adr_NX50_dpmas_db_end3                                            , 0x1C003658
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_db_mapping3, 0x1C00365C
 .equ Adr_NX50_dpmas_db_mapping3                                        , 0x1C00365C
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_db_end4    , 0x1C003660
 .equ Adr_NX50_dpmas_db_end4                                            , 0x1C003660
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_db_mapping4, 0x1C003664
 .equ Adr_NX50_dpmas_db_mapping4                                        , 0x1C003664
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_db_end5    , 0x1C003668
 .equ Adr_NX50_dpmas_db_end5                                            , 0x1C003668
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_db_mapping5, 0x1C00366C
 .equ Adr_NX50_dpmas_db_mapping5                                        , 0x1C00366C
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_db_end6    , 0x1C003670
 .equ Adr_NX50_dpmas_db_end6                                            , 0x1C003670
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_db_mapping6, 0x1C003674
 .equ Adr_NX50_dpmas_db_mapping6                                        , 0x1C003674
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_db_end7    , 0x1C003678
 .equ Adr_NX50_dpmas_db_end7                                            , 0x1C003678
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_db_mapping7, 0x1C00367C
 .equ Adr_NX50_dpmas_db_mapping7                                        , 0x1C00367C
 .equ Adr_NX50_netx_controlled_global_register_block_2_hs_ctrl          , 0x1C003680
 .equ Adr_NX50_hs_ctrl                                                  , 0x1C003680
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_hscr1      , 0x1C003684
 .equ Adr_NX50_dpmas_hscr1                                              , 0x1C003684
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_hscr2      , 0x1C003688
 .equ Adr_NX50_dpmas_hscr2                                              , 0x1C003688
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_hscr3      , 0x1C00368C
 .equ Adr_NX50_dpmas_hscr3                                              , 0x1C00368C
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_hscr4      , 0x1C003690
 .equ Adr_NX50_dpmas_hscr4                                              , 0x1C003690
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_hscr5      , 0x1C003694
 .equ Adr_NX50_dpmas_hscr5                                              , 0x1C003694
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_hscr6      , 0x1C003698
 .equ Adr_NX50_dpmas_hscr6                                              , 0x1C003698
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_hscr7      , 0x1C00369C
 .equ Adr_NX50_dpmas_hscr7                                              , 0x1C00369C
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_hscr8      , 0x1C0036A0
 .equ Adr_NX50_dpmas_hscr8                                              , 0x1C0036A0
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_hscr9      , 0x1C0036A4
 .equ Adr_NX50_dpmas_hscr9                                              , 0x1C0036A4
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_hscr10     , 0x1C0036A8
 .equ Adr_NX50_dpmas_hscr10                                             , 0x1C0036A8
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_hscr11     , 0x1C0036AC
 .equ Adr_NX50_dpmas_hscr11                                             , 0x1C0036AC
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_hscr12     , 0x1C0036B0
 .equ Adr_NX50_dpmas_hscr12                                             , 0x1C0036B0
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_hscr13     , 0x1C0036B4
 .equ Adr_NX50_dpmas_hscr13                                             , 0x1C0036B4
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_hscr14     , 0x1C0036B8
 .equ Adr_NX50_dpmas_hscr14                                             , 0x1C0036B8
 .equ Adr_NX50_netx_controlled_global_register_block_2_dpmas_hscr15     , 0x1C0036BC
 .equ Adr_NX50_dpmas_hscr15                                             , 0x1C0036BC

@ =====================================================================
@
@ Area netx_controlled_pci_configuration_shadow_register_block
 .equ Addr_NX50_netx_controlled_pci_configuration_shadow_register_block, 0x1C003700
@
@ =====================================================================
 .equ Adr_NX50_netx_controlled_pci_configuration_shadow_register_block_pci_regs_addr         , 0x1C003700
 .equ Adr_NX50_pci_regs_addr                                                                 , 0x1C003700
 .equ Adr_NX50_netx_controlled_pci_configuration_shadow_register_block_dpmas_pci_conf_rd_data, 0x1C003704
 .equ Adr_NX50_dpmas_pci_conf_rd_data                                                        , 0x1C003704
 .equ Adr_NX50_netx_controlled_pci_configuration_shadow_register_block_dpmas_pci_conf_wr_ctrl, 0x1C003708
 .equ Adr_NX50_dpmas_pci_conf_wr_ctrl                                                        , 0x1C003708
 .equ Adr_NX50_netx_controlled_pci_configuration_shadow_register_block_dpmas_pci_conf_wr_data, 0x1C00370C
 .equ Adr_NX50_dpmas_pci_conf_wr_data                                                        , 0x1C00370C

@ =====================================================================
@
@ Area dmac
 .equ Addr_NX50_dmac, 0x1C005000
@
@ =====================================================================
@ =====================================================================
@
@ Area dmac_ch
 .equ Addr_NX50_dmac_ch0, 0x1C005100
 .equ Addr_NX50_dmac_ch1, 0x1C005120
 .equ Addr_NX50_dmac_ch2, 0x1C005140
 .equ Addr_NX50_dmac_ch3, 0x1C005160
@
@ =====================================================================
 .equ Adr_NX50_dmac_ch0_dmac_chsrc_ad , 0x1C005100
 .equ Adr_NX50_dmac_ch1_dmac_chsrc_ad , 0x1C005120
 .equ Adr_NX50_dmac_ch2_dmac_chsrc_ad , 0x1C005140
 .equ Adr_NX50_dmac_ch3_dmac_chsrc_ad , 0x1C005160
 .equ Adr_NX50_dmac_ch0_dmac_chdest_ad, 0x1C005104
 .equ Adr_NX50_dmac_ch1_dmac_chdest_ad, 0x1C005124
 .equ Adr_NX50_dmac_ch2_dmac_chdest_ad, 0x1C005144
 .equ Adr_NX50_dmac_ch3_dmac_chdest_ad, 0x1C005164
 .equ Adr_NX50_dmac_ch0_dmac_chlink   , 0x1C005108
 .equ Adr_NX50_dmac_ch1_dmac_chlink   , 0x1C005128
 .equ Adr_NX50_dmac_ch2_dmac_chlink   , 0x1C005148
 .equ Adr_NX50_dmac_ch3_dmac_chlink   , 0x1C005168
 .equ Adr_NX50_dmac_ch0_dmac_chctrl   , 0x1C00510C
 .equ Adr_NX50_dmac_ch1_dmac_chctrl   , 0x1C00512C
 .equ Adr_NX50_dmac_ch2_dmac_chctrl   , 0x1C00514C
 .equ Adr_NX50_dmac_ch3_dmac_chctrl   , 0x1C00516C
 .equ Adr_NX50_dmac_ch0_dmac_chcfg    , 0x1C005110
 .equ Adr_NX50_dmac_ch1_dmac_chcfg    , 0x1C005130
 .equ Adr_NX50_dmac_ch2_dmac_chcfg    , 0x1C005150
 .equ Adr_NX50_dmac_ch3_dmac_chcfg    , 0x1C005170

@ =====================================================================
@
@ Area dmac_reg
 .equ Addr_NX50_dmac_reg, 0x1C005800
@
@ =====================================================================
 .equ Adr_NX50_dmac_reg_dmac_int_status      , 0x1C005800
 .equ Adr_NX50_dmac_int_status               , 0x1C005800
 .equ Adr_NX50_dmac_reg_dmac_inttc_status    , 0x1C005804
 .equ Adr_NX50_dmac_inttc_status             , 0x1C005804
 .equ Adr_NX50_dmac_reg_dmac_inttc_clear     , 0x1C005808
 .equ Adr_NX50_dmac_inttc_clear              , 0x1C005808
 .equ Adr_NX50_dmac_reg_dmac_interr_status   , 0x1C00580C
 .equ Adr_NX50_dmac_interr_status            , 0x1C00580C
 .equ Adr_NX50_dmac_reg_dmac_interr_clear    , 0x1C005810
 .equ Adr_NX50_dmac_interr_clear             , 0x1C005810
 .equ Adr_NX50_dmac_reg_dmac_rawinttc_status , 0x1C005814
 .equ Adr_NX50_dmac_rawinttc_status          , 0x1C005814
 .equ Adr_NX50_dmac_reg_dmac_rawinterr_status, 0x1C005818
 .equ Adr_NX50_dmac_rawinterr_status         , 0x1C005818
 .equ Adr_NX50_dmac_reg_dmac_enabled_channel , 0x1C00581C
 .equ Adr_NX50_dmac_enabled_channel          , 0x1C00581C
 .equ Adr_NX50_dmac_reg_dmac_softb_req       , 0x1C005820
 .equ Adr_NX50_dmac_softb_req                , 0x1C005820
 .equ Adr_NX50_dmac_reg_dmac_softs_req       , 0x1C005824
 .equ Adr_NX50_dmac_softs_req                , 0x1C005824
 .equ Adr_NX50_dmac_reg_dmac_softlb_req      , 0x1C005828
 .equ Adr_NX50_dmac_softlb_req               , 0x1C005828
 .equ Adr_NX50_dmac_reg_dmac_softls_req      , 0x1C00582C
 .equ Adr_NX50_dmac_softls_req               , 0x1C00582C
 .equ Adr_NX50_dmac_reg_dmac_config          , 0x1C005830
 .equ Adr_NX50_dmac_config                   , 0x1C005830
 .equ Adr_NX50_dmac_reg_dmac_sync            , 0x1C005834
 .equ Adr_NX50_dmac_sync                     , 0x1C005834
 .equ Adr_NX50_dmac_reg_dmac_rng_ctrl        , 0x1C005838
 .equ Adr_NX50_dmac_rng_ctrl                 , 0x1C005838
 .equ Adr_NX50_dmac_reg_dmac_rng_seed        , 0x1C00583C
 .equ Adr_NX50_dmac_rng_seed                 , 0x1C00583C
 .equ Adr_NX50_dmac_reg_dmac_rng_num         , 0x1C005840
 .equ Adr_NX50_dmac_rng_num                  , 0x1C005840

@ =====================================================================
@
@ Area usb_core
 .equ Addr_NX50_usb_core, 0x1C020000
@
@ =====================================================================
 .equ Adr_NX50_usb_core_ID               , 0x1C020000
 .equ Adr_NX50_ID                        , 0x1C020000
 .equ Adr_NX50_usb_core_USB_CTRL         , 0x1C020004
 .equ Adr_NX50_USB_CTRL                  , 0x1C020004
 .equ Adr_NX50_usb_core_FRM_TIMER        , 0x1C020008
 .equ Adr_NX50_FRM_TIMER                 , 0x1C020008
 .equ Adr_NX50_usb_core_MAIN_EV          , 0x1C02000C
 .equ Adr_NX50_MAIN_EV                   , 0x1C02000C
 .equ Adr_NX50_usb_core_MAIN_EM          , 0x1C020010
 .equ Adr_NX50_MAIN_EM                   , 0x1C020010
 .equ Adr_NX50_usb_core_PIPE_EV          , 0x1C020014
 .equ Adr_NX50_PIPE_EV                   , 0x1C020014
 .equ Adr_NX50_usb_core_PIPE_EM          , 0x1C020018
 .equ Adr_NX50_PIPE_EM                   , 0x1C020018
 .equ Adr_NX50_usb_core_PIPE_SEL         , 0x1C020024
 .equ Adr_NX50_PIPE_SEL                  , 0x1C020024
 .equ Adr_NX50_usb_core_PORT_STAT        , 0x1C02002C
 .equ Adr_NX50_PORT_STAT                 , 0x1C02002C
 .equ Adr_NX50_usb_core_PORT_CTRL        , 0x1C020030
 .equ Adr_NX50_PORT_CTRL                 , 0x1C020030
 .equ Adr_NX50_usb_core_PSC_EV           , 0x1C020034
 .equ Adr_NX50_PSC_EV                    , 0x1C020034
 .equ Adr_NX50_usb_core_PSC_EM           , 0x1C020038
 .equ Adr_NX50_PSC_EM                    , 0x1C020038
 .equ Adr_NX50_usb_core_PIPE_CTRL        , 0x1C020040
 .equ Adr_NX50_PIPE_CTRL                 , 0x1C020040
 .equ Adr_NX50_usb_core_PIPE_CFG         , 0x1C020044
 .equ Adr_NX50_PIPE_CFG                  , 0x1C020044
 .equ Adr_NX50_usb_core_PIPE_ADDR        , 0x1C020048
 .equ Adr_NX50_PIPE_ADDR                 , 0x1C020048
 .equ Adr_NX50_usb_core_PIPE_STAT        , 0x1C02004C
 .equ Adr_NX50_PIPE_STAT                 , 0x1C02004C
 .equ Adr_NX50_usb_core_PIPE_DATA_PTR    , 0x1C020050
 .equ Adr_NX50_PIPE_DATA_PTR             , 0x1C020050
 .equ Adr_NX50_usb_core_PIPE_DATA_TBYTES , 0x1C020054
 .equ Adr_NX50_PIPE_DATA_TBYTES          , 0x1C020054
 .equ Adr_NX50_usb_core_PIPE_ADATA_PTR   , 0x1C020058
 .equ Adr_NX50_PIPE_ADATA_PTR            , 0x1C020058
 .equ Adr_NX50_usb_core_PIPE_ADATA_TBYTES, 0x1C02005C
 .equ Adr_NX50_PIPE_ADATA_TBYTES         , 0x1C02005C
 .equ Adr_NX50_usb_core_DEBUG_CTRL       , 0x1C020060
 .equ Adr_NX50_DEBUG_CTRL                , 0x1C020060
 .equ Adr_NX50_usb_core_DEBUG_PID        , 0x1C020064
 .equ Adr_NX50_DEBUG_PID                 , 0x1C020064
 .equ Adr_NX50_usb_core_DEBUG_STAT       , 0x1C020068
 .equ Adr_NX50_DEBUG_STAT                , 0x1C020068
 .equ Adr_NX50_usb_core_TEST             , 0x1C02006C
 .equ Adr_NX50_TEST                      , 0x1C02006C
 .equ Adr_NX50_usb_core_MAIN_CFG         , 0x1C020080
 .equ Adr_NX50_MAIN_CFG                  , 0x1C020080
 .equ Adr_NX50_usb_core_MODE_CFG         , 0x1C020084
 .equ Adr_NX50_MODE_CFG                  , 0x1C020084
 .equ Adr_NX50_usb_core_usb_core_ctrl    , 0x1C020088
 .equ Adr_NX50_usb_core_ctrl             , 0x1C020088

@ =====================================================================
@
@ Area usb_fifo
 .equ Addr_NX50_usb_fifo, 0x1C030000
@
@ =====================================================================
 .equ Adr_NX50_usb_fifo_USB_FIFO_BASE, 0x1C030000
 .equ Adr_NX50_USB_FIFO_BASE         , 0x1C030000

@ =====================================================================
@
@ Area xc
 .equ Addr_NX50_xc, 0x1C040000
@
@ =====================================================================
 .equ Adr_NX50_xc_xc_base, 0x1C040000
 .equ Adr_NX50_xc_base   , 0x1C040000

@ =====================================================================
@
@ Area xpec
 .equ Addr_NX50_xpec0, 0x1C040000
 .equ Addr_NX50_xpec1, 0x1C048000
@
@ =====================================================================
 .equ Adr_NX50_xpec0_xpec_r0        , 0x1C040000
 .equ Adr_NX50_xpec1_xpec_r0        , 0x1C048000
 .equ Adr_NX50_xpec0_xpec_r1        , 0x1C040004
 .equ Adr_NX50_xpec1_xpec_r1        , 0x1C048004
 .equ Adr_NX50_xpec0_xpec_r2        , 0x1C040008
 .equ Adr_NX50_xpec1_xpec_r2        , 0x1C048008
 .equ Adr_NX50_xpec0_xpec_r3        , 0x1C04000C
 .equ Adr_NX50_xpec1_xpec_r3        , 0x1C04800C
 .equ Adr_NX50_xpec0_xpec_r4        , 0x1C040010
 .equ Adr_NX50_xpec1_xpec_r4        , 0x1C048010
 .equ Adr_NX50_xpec0_xpec_r5        , 0x1C040014
 .equ Adr_NX50_xpec1_xpec_r5        , 0x1C048014
 .equ Adr_NX50_xpec0_xpec_r6        , 0x1C040018
 .equ Adr_NX50_xpec1_xpec_r6        , 0x1C048018
 .equ Adr_NX50_xpec0_xpec_r7        , 0x1C04001C
 .equ Adr_NX50_xpec1_xpec_r7        , 0x1C04801C
 .equ Adr_NX50_xpec0_range01        , 0x1C040020
 .equ Adr_NX50_xpec1_range01        , 0x1C048020
 .equ Adr_NX50_xpec0_range23        , 0x1C040024
 .equ Adr_NX50_xpec1_range23        , 0x1C048024
 .equ Adr_NX50_xpec0_range45        , 0x1C040028
 .equ Adr_NX50_xpec1_range45        , 0x1C048028
 .equ Adr_NX50_xpec0_range67        , 0x1C04002C
 .equ Adr_NX50_xpec1_range67        , 0x1C04802C
 .equ Adr_NX50_xpec0_timer0         , 0x1C040030
 .equ Adr_NX50_xpec1_timer0         , 0x1C048030
 .equ Adr_NX50_xpec0_timer1         , 0x1C040034
 .equ Adr_NX50_xpec1_timer1         , 0x1C048034
 .equ Adr_NX50_xpec0_timer2         , 0x1C040038
 .equ Adr_NX50_xpec1_timer2         , 0x1C048038
 .equ Adr_NX50_xpec0_timer3         , 0x1C04003C
 .equ Adr_NX50_xpec1_timer3         , 0x1C04803C
 .equ Adr_NX50_xpec0_urx_count      , 0x1C040040
 .equ Adr_NX50_xpec1_urx_count      , 0x1C048040
 .equ Adr_NX50_xpec0_utx_count      , 0x1C040044
 .equ Adr_NX50_xpec1_utx_count      , 0x1C048044
 .equ Adr_NX50_xpec0_xpec_pc        , 0x1C040048
 .equ Adr_NX50_xpec1_xpec_pc        , 0x1C048048
 .equ Adr_NX50_xpec0_zero           , 0x1C04004C
 .equ Adr_NX50_xpec1_zero           , 0x1C04804C
 .equ Adr_NX50_xpec0_xpec_statcfg   , 0x1C040050
 .equ Adr_NX50_xpec1_xpec_statcfg   , 0x1C048050
 .equ Adr_NX50_xpec0_ec_maska       , 0x1C040054
 .equ Adr_NX50_xpec1_ec_maska       , 0x1C048054
 .equ Adr_NX50_xpec0_ec_maskb       , 0x1C040058
 .equ Adr_NX50_xpec1_ec_maskb       , 0x1C048058
 .equ Adr_NX50_xpec0_ec_mask0       , 0x1C04005C
 .equ Adr_NX50_xpec1_ec_mask0       , 0x1C04805C
 .equ Adr_NX50_xpec0_ec_mask1       , 0x1C040060
 .equ Adr_NX50_xpec1_ec_mask1       , 0x1C048060
 .equ Adr_NX50_xpec0_ec_mask2       , 0x1C040064
 .equ Adr_NX50_xpec1_ec_mask2       , 0x1C048064
 .equ Adr_NX50_xpec0_ec_mask3       , 0x1C040068
 .equ Adr_NX50_xpec1_ec_mask3       , 0x1C048068
 .equ Adr_NX50_xpec0_ec_mask4       , 0x1C04006C
 .equ Adr_NX50_xpec1_ec_mask4       , 0x1C04806C
 .equ Adr_NX50_xpec0_ec_mask5       , 0x1C040070
 .equ Adr_NX50_xpec1_ec_mask5       , 0x1C048070
 .equ Adr_NX50_xpec0_ec_mask6       , 0x1C040074
 .equ Adr_NX50_xpec1_ec_mask6       , 0x1C048074
 .equ Adr_NX50_xpec0_ec_mask7       , 0x1C040078
 .equ Adr_NX50_xpec1_ec_mask7       , 0x1C048078
 .equ Adr_NX50_xpec0_ec_mask8       , 0x1C04007C
 .equ Adr_NX50_xpec1_ec_mask8       , 0x1C04807C
 .equ Adr_NX50_xpec0_ec_mask9       , 0x1C040080
 .equ Adr_NX50_xpec1_ec_mask9       , 0x1C048080
 .equ Adr_NX50_xpec0_timer4         , 0x1C040084
 .equ Adr_NX50_xpec1_timer4         , 0x1C048084
 .equ Adr_NX50_xpec0_timer5         , 0x1C040088
 .equ Adr_NX50_xpec1_timer5         , 0x1C048088
 .equ Adr_NX50_xpec0_irq            , 0x1C04008C
 .equ Adr_NX50_xpec1_irq            , 0x1C04808C
 .equ Adr_NX50_xpec0_xpec_systime_ns, 0x1C040090
 .equ Adr_NX50_xpec1_xpec_systime_ns, 0x1C048090
 .equ Adr_NX50_xpec0_fifo_data      , 0x1C040094
 .equ Adr_NX50_xpec1_fifo_data      , 0x1C048094
 .equ Adr_NX50_xpec0_xpec_systime_s , 0x1C040098
 .equ Adr_NX50_xpec1_xpec_systime_s , 0x1C048098
 .equ Adr_NX50_xpec0_xpec_adc       , 0x1C04009C
 .equ Adr_NX50_xpec1_xpec_adc       , 0x1C04809C
 .equ Adr_NX50_xpec0_xpec_sr0       , 0x1C0400A0
 .equ Adr_NX50_xpec1_xpec_sr0       , 0x1C0480A0
 .equ Adr_NX50_xpec0_xpec_sr1       , 0x1C0400A4
 .equ Adr_NX50_xpec1_xpec_sr1       , 0x1C0480A4
 .equ Adr_NX50_xpec0_xpec_sr2       , 0x1C0400A8
 .equ Adr_NX50_xpec1_xpec_sr2       , 0x1C0480A8
 .equ Adr_NX50_xpec0_xpec_sr3       , 0x1C0400AC
 .equ Adr_NX50_xpec1_xpec_sr3       , 0x1C0480AC
 .equ Adr_NX50_xpec0_xpec_sr4       , 0x1C0400B0
 .equ Adr_NX50_xpec1_xpec_sr4       , 0x1C0480B0
 .equ Adr_NX50_xpec0_xpec_sr5       , 0x1C0400B4
 .equ Adr_NX50_xpec1_xpec_sr5       , 0x1C0480B4
 .equ Adr_NX50_xpec0_xpec_sr6       , 0x1C0400B8
 .equ Adr_NX50_xpec1_xpec_sr6       , 0x1C0480B8
 .equ Adr_NX50_xpec0_xpec_sr7       , 0x1C0400BC
 .equ Adr_NX50_xpec1_xpec_sr7       , 0x1C0480BC
 .equ Adr_NX50_xpec0_xpec_sr8       , 0x1C0400C0
 .equ Adr_NX50_xpec1_xpec_sr8       , 0x1C0480C0
 .equ Adr_NX50_xpec0_xpec_sr9       , 0x1C0400C4
 .equ Adr_NX50_xpec1_xpec_sr9       , 0x1C0480C4
 .equ Adr_NX50_xpec0_xpec_sr10      , 0x1C0400C8
 .equ Adr_NX50_xpec1_xpec_sr10      , 0x1C0480C8
 .equ Adr_NX50_xpec0_xpec_sr11      , 0x1C0400CC
 .equ Adr_NX50_xpec1_xpec_sr11      , 0x1C0480CC
 .equ Adr_NX50_xpec0_xpec_sr12      , 0x1C0400D0
 .equ Adr_NX50_xpec1_xpec_sr12      , 0x1C0480D0
 .equ Adr_NX50_xpec0_xpec_sr13      , 0x1C0400D4
 .equ Adr_NX50_xpec1_xpec_sr13      , 0x1C0480D4
 .equ Adr_NX50_xpec0_xpec_sr14      , 0x1C0400D8
 .equ Adr_NX50_xpec1_xpec_sr14      , 0x1C0480D8
 .equ Adr_NX50_xpec0_xpec_sr15      , 0x1C0400DC
 .equ Adr_NX50_xpec1_xpec_sr15      , 0x1C0480DC
 .equ Adr_NX50_xpec0_statcfg0       , 0x1C0400E0
 .equ Adr_NX50_xpec1_statcfg0       , 0x1C0480E0
 .equ Adr_NX50_xpec0_statcfg1       , 0x1C0400E4
 .equ Adr_NX50_xpec1_statcfg1       , 0x1C0480E4
 .equ Adr_NX50_xpec0_statcfg2       , 0x1C0400E8
 .equ Adr_NX50_xpec1_statcfg2       , 0x1C0480E8
 .equ Adr_NX50_xpec0_statcfg3       , 0x1C0400EC
 .equ Adr_NX50_xpec1_statcfg3       , 0x1C0480EC
 .equ Adr_NX50_xpec0_urtx0          , 0x1C0400F0
 .equ Adr_NX50_xpec1_urtx0          , 0x1C0480F0
 .equ Adr_NX50_xpec0_urtx1          , 0x1C0400F4
 .equ Adr_NX50_xpec1_urtx1          , 0x1C0480F4
 .equ Adr_NX50_xpec0_urtx2          , 0x1C0400F8
 .equ Adr_NX50_xpec1_urtx2          , 0x1C0480F8
 .equ Adr_NX50_xpec0_urtx3          , 0x1C0400FC
 .equ Adr_NX50_xpec1_urtx3          , 0x1C0480FC
 .equ Adr_NX50_xpec0_xpu_hold_pc    , 0x1C040100
 .equ Adr_NX50_xpec1_xpu_hold_pc    , 0x1C048100
 .equ Adr_NX50_xpec0_pram_start     , 0x1C044000
 .equ Adr_NX50_xpec1_pram_start     , 0x1C04C000
 .equ Adr_NX50_xpec0_dram_start     , 0x1C046000
 .equ Adr_NX50_xpec1_dram_start     , 0x1C04E000

@ =====================================================================
@
@ Area xmac
 .equ Addr_NX50_xmac0, 0x1C060000
 .equ Addr_NX50_xmac1, 0x1C061000
@
@ =====================================================================
 .equ Adr_NX50_xmac0_xmac_rpu_program_start  , 0x1C060000
 .equ Adr_NX50_xmac1_xmac_rpu_program_start  , 0x1C061000
 .equ Adr_NX50_xmac0_xmac_rpu_program_end    , 0x1C0603FF
 .equ Adr_NX50_xmac1_xmac_rpu_program_end    , 0x1C0613FF
 .equ Adr_NX50_xmac0_xmac_tpu_program_start  , 0x1C060400
 .equ Adr_NX50_xmac1_xmac_tpu_program_start  , 0x1C061400
 .equ Adr_NX50_xmac0_xmac_tpu_program_end    , 0x1C0607FF
 .equ Adr_NX50_xmac1_xmac_tpu_program_end    , 0x1C0617FF
 .equ Adr_NX50_xmac0_xmac_sr0                , 0x1C060800
 .equ Adr_NX50_xmac1_xmac_sr0                , 0x1C061800
 .equ Adr_NX50_xmac0_xmac_sr1                , 0x1C060804
 .equ Adr_NX50_xmac1_xmac_sr1                , 0x1C061804
 .equ Adr_NX50_xmac0_xmac_sr2                , 0x1C060808
 .equ Adr_NX50_xmac1_xmac_sr2                , 0x1C061808
 .equ Adr_NX50_xmac0_xmac_sr3                , 0x1C06080C
 .equ Adr_NX50_xmac1_xmac_sr3                , 0x1C06180C
 .equ Adr_NX50_xmac0_xmac_sr4                , 0x1C060810
 .equ Adr_NX50_xmac1_xmac_sr4                , 0x1C061810
 .equ Adr_NX50_xmac0_xmac_sr5                , 0x1C060814
 .equ Adr_NX50_xmac1_xmac_sr5                , 0x1C061814
 .equ Adr_NX50_xmac0_xmac_sr6                , 0x1C060818
 .equ Adr_NX50_xmac1_xmac_sr6                , 0x1C061818
 .equ Adr_NX50_xmac0_xmac_sr7                , 0x1C06081C
 .equ Adr_NX50_xmac1_xmac_sr7                , 0x1C06181C
 .equ Adr_NX50_xmac0_xmac_sr8                , 0x1C060820
 .equ Adr_NX50_xmac1_xmac_sr8                , 0x1C061820
 .equ Adr_NX50_xmac0_xmac_sr9                , 0x1C060824
 .equ Adr_NX50_xmac1_xmac_sr9                , 0x1C061824
 .equ Adr_NX50_xmac0_xmac_sr10               , 0x1C060828
 .equ Adr_NX50_xmac1_xmac_sr10               , 0x1C061828
 .equ Adr_NX50_xmac0_xmac_sr11               , 0x1C06082C
 .equ Adr_NX50_xmac1_xmac_sr11               , 0x1C06182C
 .equ Adr_NX50_xmac0_xmac_sr12               , 0x1C060830
 .equ Adr_NX50_xmac1_xmac_sr12               , 0x1C061830
 .equ Adr_NX50_xmac0_xmac_sr13               , 0x1C060834
 .equ Adr_NX50_xmac1_xmac_sr13               , 0x1C061834
 .equ Adr_NX50_xmac0_xmac_sr14               , 0x1C060838
 .equ Adr_NX50_xmac1_xmac_sr14               , 0x1C061838
 .equ Adr_NX50_xmac0_xmac_sr15               , 0x1C06083C
 .equ Adr_NX50_xmac1_xmac_sr15               , 0x1C06183C
 .equ Adr_NX50_xmac0_xmac_status_shared0     , 0x1C060840
 .equ Adr_NX50_xmac1_xmac_status_shared0     , 0x1C061840
 .equ Adr_NX50_xmac0_xmac_config_shared0     , 0x1C060844
 .equ Adr_NX50_xmac1_xmac_config_shared0     , 0x1C061844
 .equ Adr_NX50_xmac0_xmac_status_shared1     , 0x1C060848
 .equ Adr_NX50_xmac1_xmac_status_shared1     , 0x1C061848
 .equ Adr_NX50_xmac0_xmac_config_shared1     , 0x1C06084C
 .equ Adr_NX50_xmac1_xmac_config_shared1     , 0x1C06184C
 .equ Adr_NX50_xmac0_xmac_urx_utx0           , 0x1C060860
 .equ Adr_NX50_xmac1_xmac_urx_utx0           , 0x1C061860
 .equ Adr_NX50_xmac0_xmac_urx_utx1           , 0x1C060864
 .equ Adr_NX50_xmac1_xmac_urx_utx1           , 0x1C061864
 .equ Adr_NX50_xmac0_xmac_urx                , 0x1C060870
 .equ Adr_NX50_xmac1_xmac_urx                , 0x1C061870
 .equ Adr_NX50_xmac0_xmac_utx                , 0x1C060874
 .equ Adr_NX50_xmac1_xmac_utx                , 0x1C061874
 .equ Adr_NX50_xmac0_xmac_rx                 , 0x1C060878
 .equ Adr_NX50_xmac1_xmac_rx                 , 0x1C061878
 .equ Adr_NX50_xmac0_xmac_rx_hw              , 0x1C06087C
 .equ Adr_NX50_xmac1_xmac_rx_hw              , 0x1C06187C
 .equ Adr_NX50_xmac0_xmac_rx_hw_count        , 0x1C060880
 .equ Adr_NX50_xmac1_xmac_rx_hw_count        , 0x1C061880
 .equ Adr_NX50_xmac0_xmac_tx                 , 0x1C060884
 .equ Adr_NX50_xmac1_xmac_tx                 , 0x1C061884
 .equ Adr_NX50_xmac0_xmac_tx_hw              , 0x1C060888
 .equ Adr_NX50_xmac1_xmac_tx_hw              , 0x1C061888
 .equ Adr_NX50_xmac0_xmac_tx_hw_count        , 0x1C06088C
 .equ Adr_NX50_xmac1_xmac_tx_hw_count        , 0x1C06188C
 .equ Adr_NX50_xmac0_xmac_tx_sent            , 0x1C060890
 .equ Adr_NX50_xmac1_xmac_tx_sent            , 0x1C061890
 .equ Adr_NX50_xmac0_xmac_rpu_pc             , 0x1C060894
 .equ Adr_NX50_xmac1_xmac_rpu_pc             , 0x1C061894
 .equ Adr_NX50_xmac0_xmac_tpu_pc             , 0x1C060898
 .equ Adr_NX50_xmac1_xmac_tpu_pc             , 0x1C061898
 .equ Adr_NX50_xmac0_xmac_wr0                , 0x1C06089C
 .equ Adr_NX50_xmac1_xmac_wr0                , 0x1C06189C
 .equ Adr_NX50_xmac0_xmac_wr1                , 0x1C0608A0
 .equ Adr_NX50_xmac1_xmac_wr1                , 0x1C0618A0
 .equ Adr_NX50_xmac0_xmac_wr2                , 0x1C0608A4
 .equ Adr_NX50_xmac1_xmac_wr2                , 0x1C0618A4
 .equ Adr_NX50_xmac0_xmac_wr3                , 0x1C0608A8
 .equ Adr_NX50_xmac1_xmac_wr3                , 0x1C0618A8
 .equ Adr_NX50_xmac0_xmac_wr4                , 0x1C0608AC
 .equ Adr_NX50_xmac1_xmac_wr4                , 0x1C0618AC
 .equ Adr_NX50_xmac0_xmac_wr5                , 0x1C0608B0
 .equ Adr_NX50_xmac1_xmac_wr5                , 0x1C0618B0
 .equ Adr_NX50_xmac0_xmac_wr6                , 0x1C0608B4
 .equ Adr_NX50_xmac1_xmac_wr6                , 0x1C0618B4
 .equ Adr_NX50_xmac0_xmac_wr7                , 0x1C0608B8
 .equ Adr_NX50_xmac1_xmac_wr7                , 0x1C0618B8
 .equ Adr_NX50_xmac0_xmac_wr8                , 0x1C0608BC
 .equ Adr_NX50_xmac1_xmac_wr8                , 0x1C0618BC
 .equ Adr_NX50_xmac0_xmac_wr9                , 0x1C0608C0
 .equ Adr_NX50_xmac1_xmac_wr9                , 0x1C0618C0
 .equ Adr_NX50_xmac0_xmac_sys_time           , 0x1C0608C4
 .equ Adr_NX50_xmac1_xmac_sys_time           , 0x1C0618C4
 .equ Adr_NX50_xmac0_xmac_sys_time_upper     , 0x1C0608C8
 .equ Adr_NX50_xmac1_xmac_sys_time_upper     , 0x1C0618C8
 .equ Adr_NX50_xmac0_xmac_cmp0_status        , 0x1C0608CC
 .equ Adr_NX50_xmac1_xmac_cmp0_status        , 0x1C0618CC
 .equ Adr_NX50_xmac0_xmac_cmp1_status        , 0x1C0608D0
 .equ Adr_NX50_xmac1_xmac_cmp1_status        , 0x1C0618D0
 .equ Adr_NX50_xmac0_xmac_cmp2_status        , 0x1C0608D4
 .equ Adr_NX50_xmac1_xmac_cmp2_status        , 0x1C0618D4
 .equ Adr_NX50_xmac0_xmac_cmp3_status        , 0x1C0608D8
 .equ Adr_NX50_xmac1_xmac_cmp3_status        , 0x1C0618D8
 .equ Adr_NX50_xmac0_xmac_alu_flags          , 0x1C0608DC
 .equ Adr_NX50_xmac1_xmac_alu_flags          , 0x1C0618DC
 .equ Adr_NX50_xmac0_xmac_status_int         , 0x1C0608E0
 .equ Adr_NX50_xmac1_xmac_status_int         , 0x1C0618E0
 .equ Adr_NX50_xmac0_xmac_stat_bits          , 0x1C0608E4
 .equ Adr_NX50_xmac1_xmac_stat_bits          , 0x1C0618E4
 .equ Adr_NX50_xmac0_xmac_status_mii         , 0x1C0608E8
 .equ Adr_NX50_xmac1_xmac_status_mii         , 0x1C0618E8
 .equ Adr_NX50_xmac0_xmac_status_mii2        , 0x1C0608EC
 .equ Adr_NX50_xmac1_xmac_status_mii2        , 0x1C0618EC
 .equ Adr_NX50_xmac0_xmac_config_mii         , 0x1C0608F0
 .equ Adr_NX50_xmac1_xmac_config_mii         , 0x1C0618F0
 .equ Adr_NX50_xmac0_xmac_config_nibble_fifo , 0x1C0608F4
 .equ Adr_NX50_xmac1_xmac_config_nibble_fifo , 0x1C0618F4
 .equ Adr_NX50_xmac0_xmac_config_sbu         , 0x1C0608F8
 .equ Adr_NX50_xmac1_xmac_config_sbu         , 0x1C0618F8
 .equ Adr_NX50_xmac0_xmac_sbu_rate_mul_add   , 0x1C0608FC
 .equ Adr_NX50_xmac1_xmac_sbu_rate_mul_add   , 0x1C0618FC
 .equ Adr_NX50_xmac0_xmac_sbu_rate_mul_start , 0x1C060900
 .equ Adr_NX50_xmac1_xmac_sbu_rate_mul_start , 0x1C061900
 .equ Adr_NX50_xmac0_xmac_sbu_rate_mul       , 0x1C060904
 .equ Adr_NX50_xmac1_xmac_sbu_rate_mul       , 0x1C061904
 .equ Adr_NX50_xmac0_xmac_start_sample_pos   , 0x1C060908
 .equ Adr_NX50_xmac1_xmac_start_sample_pos   , 0x1C061908
 .equ Adr_NX50_xmac0_xmac_stop_sample_pos    , 0x1C06090C
 .equ Adr_NX50_xmac1_xmac_stop_sample_pos    , 0x1C06190C
 .equ Adr_NX50_xmac0_xmac_config_obu         , 0x1C060910
 .equ Adr_NX50_xmac1_xmac_config_obu         , 0x1C061910
 .equ Adr_NX50_xmac0_xmac_obu_rate_mul_add   , 0x1C060914
 .equ Adr_NX50_xmac1_xmac_obu_rate_mul_add   , 0x1C061914
 .equ Adr_NX50_xmac0_xmac_obu_rate_mul_start , 0x1C060918
 .equ Adr_NX50_xmac1_xmac_obu_rate_mul_start , 0x1C061918
 .equ Adr_NX50_xmac0_xmac_obu_rate_mul       , 0x1C06091C
 .equ Adr_NX50_xmac1_xmac_obu_rate_mul       , 0x1C06191C
 .equ Adr_NX50_xmac0_xmac_start_trans_pos    , 0x1C060920
 .equ Adr_NX50_xmac1_xmac_start_trans_pos    , 0x1C061920
 .equ Adr_NX50_xmac0_xmac_stop_trans_pos     , 0x1C060924
 .equ Adr_NX50_xmac1_xmac_stop_trans_pos     , 0x1C061924
 .equ Adr_NX50_xmac0_xmac_rpu_count1         , 0x1C060928
 .equ Adr_NX50_xmac1_xmac_rpu_count1         , 0x1C061928
 .equ Adr_NX50_xmac0_xmac_rpu_count2         , 0x1C06092C
 .equ Adr_NX50_xmac1_xmac_rpu_count2         , 0x1C06192C
 .equ Adr_NX50_xmac0_xmac_tpu_count1         , 0x1C060930
 .equ Adr_NX50_xmac1_xmac_tpu_count1         , 0x1C061930
 .equ Adr_NX50_xmac0_xmac_tpu_count2         , 0x1C060934
 .equ Adr_NX50_xmac1_xmac_tpu_count2         , 0x1C061934
 .equ Adr_NX50_xmac0_xmac_rx_count           , 0x1C060938
 .equ Adr_NX50_xmac1_xmac_rx_count           , 0x1C061938
 .equ Adr_NX50_xmac0_xmac_tx_count           , 0x1C06093C
 .equ Adr_NX50_xmac1_xmac_tx_count           , 0x1C06193C
 .equ Adr_NX50_xmac0_xmac_rpm_mask0          , 0x1C060940
 .equ Adr_NX50_xmac1_xmac_rpm_mask0          , 0x1C061940
 .equ Adr_NX50_xmac0_xmac_rpm_val0           , 0x1C060944
 .equ Adr_NX50_xmac1_xmac_rpm_val0           , 0x1C061944
 .equ Adr_NX50_xmac0_xmac_rpm_mask1          , 0x1C060948
 .equ Adr_NX50_xmac1_xmac_rpm_mask1          , 0x1C061948
 .equ Adr_NX50_xmac0_xmac_rpm_val1           , 0x1C06094C
 .equ Adr_NX50_xmac1_xmac_rpm_val1           , 0x1C06194C
 .equ Adr_NX50_xmac0_xmac_tpm_mask0          , 0x1C060950
 .equ Adr_NX50_xmac1_xmac_tpm_mask0          , 0x1C061950
 .equ Adr_NX50_xmac0_xmac_tpm_val0           , 0x1C060954
 .equ Adr_NX50_xmac1_xmac_tpm_val0           , 0x1C061954
 .equ Adr_NX50_xmac0_xmac_tpm_mask1          , 0x1C060958
 .equ Adr_NX50_xmac1_xmac_tpm_mask1          , 0x1C061958
 .equ Adr_NX50_xmac0_xmac_tpm_val1           , 0x1C06095C
 .equ Adr_NX50_xmac1_xmac_tpm_val1           , 0x1C06195C
 .equ Adr_NX50_xmac0_xmac_rx_tx_nof_bits     , 0x1C0609D0
 .equ Adr_NX50_xmac1_xmac_rx_tx_nof_bits     , 0x1C0619D0
 .equ Adr_NX50_xmac0_xmac_rx_crc_polynomial_l, 0x1C0609D8
 .equ Adr_NX50_xmac1_xmac_rx_crc_polynomial_l, 0x1C0619D8
 .equ Adr_NX50_xmac0_xmac_rx_crc_polynomial_h, 0x1C0609DC
 .equ Adr_NX50_xmac1_xmac_rx_crc_polynomial_h, 0x1C0619DC
 .equ Adr_NX50_xmac0_xmac_rx_crc_l           , 0x1C0609E0
 .equ Adr_NX50_xmac1_xmac_rx_crc_l           , 0x1C0619E0
 .equ Adr_NX50_xmac0_xmac_rx_crc_h           , 0x1C0609E4
 .equ Adr_NX50_xmac1_xmac_rx_crc_h           , 0x1C0619E4
 .equ Adr_NX50_xmac0_xmac_rx_crc_cfg         , 0x1C0609E8
 .equ Adr_NX50_xmac1_xmac_rx_crc_cfg         , 0x1C0619E8
 .equ Adr_NX50_xmac0_xmac_tx_crc_polynomial_l, 0x1C0609EC
 .equ Adr_NX50_xmac1_xmac_tx_crc_polynomial_l, 0x1C0619EC
 .equ Adr_NX50_xmac0_xmac_tx_crc_polynomial_h, 0x1C0609F0
 .equ Adr_NX50_xmac1_xmac_tx_crc_polynomial_h, 0x1C0619F0
 .equ Adr_NX50_xmac0_xmac_tx_crc_l           , 0x1C0609F4
 .equ Adr_NX50_xmac1_xmac_tx_crc_l           , 0x1C0619F4
 .equ Adr_NX50_xmac0_xmac_tx_crc_h           , 0x1C0609F8
 .equ Adr_NX50_xmac1_xmac_tx_crc_h           , 0x1C0619F8
 .equ Adr_NX50_xmac0_xmac_tx_crc_cfg         , 0x1C0609FC
 .equ Adr_NX50_xmac1_xmac_tx_crc_cfg         , 0x1C0619FC
 .equ Adr_NX50_xmac0_xmac_rpu_hold_pc        , 0x1C060A00
 .equ Adr_NX50_xmac1_xmac_rpu_hold_pc        , 0x1C061A00
 .equ Adr_NX50_xmac0_xmac_tpu_hold_pc        , 0x1C060A04
 .equ Adr_NX50_xmac1_xmac_tpu_hold_pc        , 0x1C061A04

@ =====================================================================
@
@ Area pointer_fifo
 .equ Addr_NX50_pointer_fifo, 0x1C064000
@
@ =====================================================================
 .equ Adr_NX50_pointer_fifo_pfifo_base           , 0x1C064000
 .equ Adr_NX50_pfifo_base                        , 0x1C064000
 .equ Adr_NX50_pointer_fifo_pfifo_border_base    , 0x1C064080
 .equ Adr_NX50_pfifo_border_base                 , 0x1C064080
 .equ Adr_NX50_pointer_fifo_pfifo_reset          , 0x1C064100
 .equ Adr_NX50_pfifo_reset                       , 0x1C064100
 .equ Adr_NX50_pointer_fifo_pfifo_full           , 0x1C064104
 .equ Adr_NX50_pfifo_full                        , 0x1C064104
 .equ Adr_NX50_pointer_fifo_pfifo_empty          , 0x1C064108
 .equ Adr_NX50_pfifo_empty                       , 0x1C064108
 .equ Adr_NX50_pointer_fifo_pfifo_overflow       , 0x1C06410C
 .equ Adr_NX50_pfifo_overflow                    , 0x1C06410C
 .equ Adr_NX50_pointer_fifo_pfifo_underrun       , 0x1C064110
 .equ Adr_NX50_pfifo_underrun                    , 0x1C064110
 .equ Adr_NX50_pointer_fifo_pfifo_fill_level_base, 0x1C064180
 .equ Adr_NX50_pfifo_fill_level_base             , 0x1C064180

@ =====================================================================
@
@ Area xpec_irq_registers
 .equ Addr_NX50_xpec_irq_registers, 0x1C064400
@
@ =====================================================================
 .equ Adr_NX50_xpec_irq_registers_irq_xpec0, 0x1C064400
 .equ Adr_NX50_irq_xpec0                   , 0x1C064400
 .equ Adr_NX50_xpec_irq_registers_irq_xpec1, 0x1C064404
 .equ Adr_NX50_irq_xpec1                   , 0x1C064404

@ =====================================================================
@
@ Area xc_mem_prot
 .equ Addr_NX50_xc_mem_prot, 0x1C064800
@
@ =====================================================================
 .equ Adr_NX50_xc_mem_prot_xpec0_ram_low   , 0x1C064800
 .equ Adr_NX50_xpec0_ram_low               , 0x1C064800
 .equ Adr_NX50_xc_mem_prot_xpec1_ram_low   , 0x1C064804
 .equ Adr_NX50_xpec1_ram_low               , 0x1C064804
 .equ Adr_NX50_xc_mem_prot_xpec0_ram_high  , 0x1C064810
 .equ Adr_NX50_xpec0_ram_high              , 0x1C064810
 .equ Adr_NX50_xc_mem_prot_xpec1_ram_high  , 0x1C064814
 .equ Adr_NX50_xpec1_ram_high              , 0x1C064814
 .equ Adr_NX50_xc_mem_prot_prot_key_enable , 0x1C064900
 .equ Adr_NX50_prot_key_enable             , 0x1C064900
 .equ Adr_NX50_xc_mem_prot_prot_key_disable, 0x1C064904
 .equ Adr_NX50_prot_key_disable            , 0x1C064904
 .equ Adr_NX50_xc_mem_prot_prot_status     , 0x1C064908
 .equ Adr_NX50_prot_status                 , 0x1C064908

@ =====================================================================
@
@ Area xc_debug
 .equ Addr_NX50_xc_debug, 0x1C064C00
@
@ =====================================================================
 .equ Adr_NX50_xc_debug_xc_debug_config, 0x1C064C00
 .equ Adr_NX50_xc_debug_config         , 0x1C064C00

@ =====================================================================
@
@ Area trigger_sample_unit
 .equ Addr_NX50_trigger_sample_unit, 0x1C065000
@
@ =====================================================================
 .equ Adr_NX50_trigger_sample_unit_trigger_sample_config     , 0x1C065000
 .equ Adr_NX50_trigger_sample_config                         , 0x1C065000
 .equ Adr_NX50_trigger_sample_unit_trigger_sample_status     , 0x1C065004
 .equ Adr_NX50_trigger_sample_status                         , 0x1C065004
 .equ Adr_NX50_trigger_sample_unit_trigger_sample_irq        , 0x1C065008
 .equ Adr_NX50_trigger_sample_irq                            , 0x1C065008
 .equ Adr_NX50_trigger_sample_unit_trigger_activate          , 0x1C06500C
 .equ Adr_NX50_trigger_activate                              , 0x1C06500C
 .equ Adr_NX50_trigger_sample_unit_trigger_impulse_length    , 0x1C065010
 .equ Adr_NX50_trigger_impulse_length                        , 0x1C065010
 .equ Adr_NX50_trigger_sample_unit_trigger_0_starttime_ns    , 0x1C065014
 .equ Adr_NX50_trigger_0_starttime_ns                        , 0x1C065014
 .equ Adr_NX50_trigger_sample_unit_trigger_1_starttime_ns    , 0x1C06501C
 .equ Adr_NX50_trigger_1_starttime_ns                        , 0x1C06501C
 .equ Adr_NX50_trigger_sample_unit_trigger_0_cyc_time        , 0x1C065024
 .equ Adr_NX50_trigger_0_cyc_time                            , 0x1C065024
 .equ Adr_NX50_trigger_sample_unit_trigger_1_cyc_time        , 0x1C065028
 .equ Adr_NX50_trigger_1_cyc_time                            , 0x1C065028
 .equ Adr_NX50_trigger_sample_unit_sample_mode               , 0x1C06502C
 .equ Adr_NX50_sample_mode                                   , 0x1C06502C
 .equ Adr_NX50_trigger_sample_unit_sample_0_pos_systime_ns   , 0x1C065030
 .equ Adr_NX50_sample_0_pos_systime_ns                       , 0x1C065030
 .equ Adr_NX50_trigger_sample_unit_sample_0_neg_systime_ns   , 0x1C065038
 .equ Adr_NX50_sample_0_neg_systime_ns                       , 0x1C065038
 .equ Adr_NX50_trigger_sample_unit_sample_1_pos_systime_ns   , 0x1C065040
 .equ Adr_NX50_sample_1_pos_systime_ns                       , 0x1C065040
 .equ Adr_NX50_trigger_sample_unit_sample_1_neg_systime_ns   , 0x1C065048
 .equ Adr_NX50_sample_1_neg_systime_ns                       , 0x1C065048
 .equ Adr_NX50_trigger_sample_unit_sercos3_timing_delay_mst_1, 0x1C065050
 .equ Adr_NX50_sercos3_timing_delay_mst_1                    , 0x1C065050
 .equ Adr_NX50_trigger_sample_unit_sercos3_timing_delay_mst_2, 0x1C065054
 .equ Adr_NX50_sercos3_timing_delay_mst_2                    , 0x1C065054
 .equ Adr_NX50_trigger_sample_unit_sercos3_ring_delay        , 0x1C065058
 .equ Adr_NX50_sercos3_ring_delay                            , 0x1C065058
 .equ Adr_NX50_trigger_sample_unit_sercos3_sync_count_1      , 0x1C06505C
 .equ Adr_NX50_sercos3_sync_count_1                          , 0x1C06505C
 .equ Adr_NX50_trigger_sample_unit_sercos3_sync_count_2      , 0x1C065060
 .equ Adr_NX50_sercos3_sync_count_2                          , 0x1C065060
 .equ Adr_NX50_trigger_sample_unit_sercos3_tcnt_0            , 0x1C065064
 .equ Adr_NX50_sercos3_tcnt_0                                , 0x1C065064
 .equ Adr_NX50_trigger_sample_unit_sercos3_tcnt_1            , 0x1C065068
 .equ Adr_NX50_sercos3_tcnt_1                                , 0x1C065068
 .equ Adr_NX50_trigger_sample_unit_sercos3_tcnt_2            , 0x1C06506C
 .equ Adr_NX50_sercos3_tcnt_2                                , 0x1C06506C
 .equ Adr_NX50_trigger_sample_unit_sercos3_tcnt_3            , 0x1C065070
 .equ Adr_NX50_sercos3_tcnt_3                                , 0x1C065070
 .equ Adr_NX50_trigger_sample_unit_sercos3_tdiv_clk          , 0x1C065074
 .equ Adr_NX50_sercos3_tdiv_clk                              , 0x1C065074
 .equ Adr_NX50_trigger_sample_unit_sercos3_dtdiv_clk         , 0x1C065078
 .equ Adr_NX50_sercos3_dtdiv_clk                             , 0x1C065078
 .equ Adr_NX50_trigger_sample_unit_sercos3_ndiv_clk          , 0x1C06507C
 .equ Adr_NX50_sercos3_ndiv_clk                              , 0x1C06507C
 .equ Adr_NX50_trigger_sample_unit_sercos3_divclk_length     , 0x1C065080
 .equ Adr_NX50_sercos3_divclk_length                         , 0x1C065080
 .equ Adr_NX50_trigger_sample_unit_sercos3_divclk_config     , 0x1C065084
 .equ Adr_NX50_sercos3_divclk_config                         , 0x1C065084
 .equ Adr_NX50_trigger_sample_unit_sercos3_tcnt_max          , 0x1C065088
 .equ Adr_NX50_sercos3_tcnt_max                              , 0x1C065088
 .equ Adr_NX50_trigger_sample_unit_sercos3_tmx               , 0x1C06508C
 .equ Adr_NX50_sercos3_tmx                                   , 0x1C06508C

@ =====================================================================
@
@ Area fmmusm
 .equ Addr_NX50_fmmusm, 0x1C065400
@
@ =====================================================================
 .equ Adr_NX50_fmmusm_fmmu0_cfg_log_startaddr , 0x1C065400
 .equ Adr_NX50_fmmu0_cfg_log_startaddr        , 0x1C065400
 .equ Adr_NX50_fmmusm_fmmu0_cfg_length        , 0x1C065404
 .equ Adr_NX50_fmmu0_cfg_length               , 0x1C065404
 .equ Adr_NX50_fmmusm_fmmu0_cfg_phys_startaddr, 0x1C065408
 .equ Adr_NX50_fmmu0_cfg_phys_startaddr       , 0x1C065408
 .equ Adr_NX50_fmmusm_fmmu0_cfg_enable        , 0x1C06540C
 .equ Adr_NX50_fmmu0_cfg_enable               , 0x1C06540C
 .equ Adr_NX50_fmmusm_fmmu1_cfg_log_startaddr , 0x1C065410
 .equ Adr_NX50_fmmu1_cfg_log_startaddr        , 0x1C065410
 .equ Adr_NX50_fmmusm_fmmu1_cfg_length        , 0x1C065414
 .equ Adr_NX50_fmmu1_cfg_length               , 0x1C065414
 .equ Adr_NX50_fmmusm_fmmu1_cfg_phys_startaddr, 0x1C065418
 .equ Adr_NX50_fmmu1_cfg_phys_startaddr       , 0x1C065418
 .equ Adr_NX50_fmmusm_fmmu1_cfg_enable        , 0x1C06541C
 .equ Adr_NX50_fmmu1_cfg_enable               , 0x1C06541C
 .equ Adr_NX50_fmmusm_fmmu2_cfg_log_startaddr , 0x1C065420
 .equ Adr_NX50_fmmu2_cfg_log_startaddr        , 0x1C065420
 .equ Adr_NX50_fmmusm_fmmu2_cfg_length        , 0x1C065424
 .equ Adr_NX50_fmmu2_cfg_length               , 0x1C065424
 .equ Adr_NX50_fmmusm_fmmu2_cfg_phys_startaddr, 0x1C065428
 .equ Adr_NX50_fmmu2_cfg_phys_startaddr       , 0x1C065428
 .equ Adr_NX50_fmmusm_fmmu2_cfg_enable        , 0x1C06542C
 .equ Adr_NX50_fmmu2_cfg_enable               , 0x1C06542C
 .equ Adr_NX50_fmmusm_fmmu3_cfg_log_startaddr , 0x1C065430
 .equ Adr_NX50_fmmu3_cfg_log_startaddr        , 0x1C065430
 .equ Adr_NX50_fmmusm_fmmu3_cfg_length        , 0x1C065434
 .equ Adr_NX50_fmmu3_cfg_length               , 0x1C065434
 .equ Adr_NX50_fmmusm_fmmu3_cfg_phys_startaddr, 0x1C065438
 .equ Adr_NX50_fmmu3_cfg_phys_startaddr       , 0x1C065438
 .equ Adr_NX50_fmmusm_fmmu3_cfg_enable        , 0x1C06543C
 .equ Adr_NX50_fmmu3_cfg_enable               , 0x1C06543C
 .equ Adr_NX50_fmmusm_fmmu4_cfg_log_startaddr , 0x1C065440
 .equ Adr_NX50_fmmu4_cfg_log_startaddr        , 0x1C065440
 .equ Adr_NX50_fmmusm_fmmu4_cfg_length        , 0x1C065444
 .equ Adr_NX50_fmmu4_cfg_length               , 0x1C065444
 .equ Adr_NX50_fmmusm_fmmu4_cfg_phys_startaddr, 0x1C065448
 .equ Adr_NX50_fmmu4_cfg_phys_startaddr       , 0x1C065448
 .equ Adr_NX50_fmmusm_fmmu4_cfg_enable        , 0x1C06544C
 .equ Adr_NX50_fmmu4_cfg_enable               , 0x1C06544C
 .equ Adr_NX50_fmmusm_fmmu5_cfg_log_startaddr , 0x1C065450
 .equ Adr_NX50_fmmu5_cfg_log_startaddr        , 0x1C065450
 .equ Adr_NX50_fmmusm_fmmu5_cfg_length        , 0x1C065454
 .equ Adr_NX50_fmmu5_cfg_length               , 0x1C065454
 .equ Adr_NX50_fmmusm_fmmu5_cfg_phys_startaddr, 0x1C065458
 .equ Adr_NX50_fmmu5_cfg_phys_startaddr       , 0x1C065458
 .equ Adr_NX50_fmmusm_fmmu5_cfg_enable        , 0x1C06545C
 .equ Adr_NX50_fmmu5_cfg_enable               , 0x1C06545C
 .equ Adr_NX50_fmmusm_fmmu6_cfg_log_startaddr , 0x1C065460
 .equ Adr_NX50_fmmu6_cfg_log_startaddr        , 0x1C065460
 .equ Adr_NX50_fmmusm_fmmu6_cfg_length        , 0x1C065464
 .equ Adr_NX50_fmmu6_cfg_length               , 0x1C065464
 .equ Adr_NX50_fmmusm_fmmu6_cfg_phys_startaddr, 0x1C065468
 .equ Adr_NX50_fmmu6_cfg_phys_startaddr       , 0x1C065468
 .equ Adr_NX50_fmmusm_fmmu6_cfg_enable        , 0x1C06546C
 .equ Adr_NX50_fmmu6_cfg_enable               , 0x1C06546C
 .equ Adr_NX50_fmmusm_fmmu7_cfg_log_startaddr , 0x1C065470
 .equ Adr_NX50_fmmu7_cfg_log_startaddr        , 0x1C065470
 .equ Adr_NX50_fmmusm_fmmu7_cfg_length        , 0x1C065474
 .equ Adr_NX50_fmmu7_cfg_length               , 0x1C065474
 .equ Adr_NX50_fmmusm_fmmu7_cfg_phys_startaddr, 0x1C065478
 .equ Adr_NX50_fmmu7_cfg_phys_startaddr       , 0x1C065478
 .equ Adr_NX50_fmmusm_fmmu7_cfg_enable        , 0x1C06547C
 .equ Adr_NX50_fmmu7_cfg_enable               , 0x1C06547C
 .equ Adr_NX50_fmmusm_sm0_cfg_adr_len         , 0x1C065480
 .equ Adr_NX50_sm0_cfg_adr_len                , 0x1C065480
 .equ Adr_NX50_fmmusm_sm0_cfg_mode            , 0x1C065484
 .equ Adr_NX50_sm0_cfg_mode                   , 0x1C065484
 .equ Adr_NX50_fmmusm_sm1_cfg_adr_len         , 0x1C065488
 .equ Adr_NX50_sm1_cfg_adr_len                , 0x1C065488
 .equ Adr_NX50_fmmusm_sm1_cfg_mode            , 0x1C06548C
 .equ Adr_NX50_sm1_cfg_mode                   , 0x1C06548C
 .equ Adr_NX50_fmmusm_sm2_cfg_adr_len         , 0x1C065490
 .equ Adr_NX50_sm2_cfg_adr_len                , 0x1C065490
 .equ Adr_NX50_fmmusm_sm2_cfg_mode            , 0x1C065494
 .equ Adr_NX50_sm2_cfg_mode                   , 0x1C065494
 .equ Adr_NX50_fmmusm_sm3_cfg_adr_len         , 0x1C065498
 .equ Adr_NX50_sm3_cfg_adr_len                , 0x1C065498
 .equ Adr_NX50_fmmusm_sm3_cfg_mode            , 0x1C06549C
 .equ Adr_NX50_sm3_cfg_mode                   , 0x1C06549C
 .equ Adr_NX50_fmmusm_sm4_cfg_adr_len         , 0x1C0654A0
 .equ Adr_NX50_sm4_cfg_adr_len                , 0x1C0654A0
 .equ Adr_NX50_fmmusm_sm4_cfg_mode            , 0x1C0654A4
 .equ Adr_NX50_sm4_cfg_mode                   , 0x1C0654A4
 .equ Adr_NX50_fmmusm_sm5_cfg_adr_len         , 0x1C0654A8
 .equ Adr_NX50_sm5_cfg_adr_len                , 0x1C0654A8
 .equ Adr_NX50_fmmusm_sm5_cfg_mode            , 0x1C0654AC
 .equ Adr_NX50_sm5_cfg_mode                   , 0x1C0654AC
 .equ Adr_NX50_fmmusm_sm6_cfg_adr_len         , 0x1C0654B0
 .equ Adr_NX50_sm6_cfg_adr_len                , 0x1C0654B0
 .equ Adr_NX50_fmmusm_sm6_cfg_mode            , 0x1C0654B4
 .equ Adr_NX50_sm6_cfg_mode                   , 0x1C0654B4
 .equ Adr_NX50_fmmusm_sm7_cfg_adr_len         , 0x1C0654B8
 .equ Adr_NX50_sm7_cfg_adr_len                , 0x1C0654B8
 .equ Adr_NX50_fmmusm_sm7_cfg_mode            , 0x1C0654BC
 .equ Adr_NX50_sm7_cfg_mode                   , 0x1C0654BC
 .equ Adr_NX50_fmmusm_phys_addr_offset        , 0x1C0654D0
 .equ Adr_NX50_phys_addr_offset               , 0x1C0654D0
 .equ Adr_NX50_fmmusm_phys_last_addr          , 0x1C0654D4
 .equ Adr_NX50_phys_last_addr                 , 0x1C0654D4
 .equ Adr_NX50_fmmusm_fmmusm_xpec_nr          , 0x1C0654D8
 .equ Adr_NX50_fmmusm_xpec_nr                 , 0x1C0654D8
 .equ Adr_NX50_fmmusm_fmmusm_read_addr_in     , 0x1C065500
 .equ Adr_NX50_fmmusm_read_addr_in            , 0x1C065500
 .equ Adr_NX50_fmmusm_fmmusm_write_addr_in    , 0x1C065504
 .equ Adr_NX50_fmmusm_write_addr_in           , 0x1C065504
 .equ Adr_NX50_fmmusm_sm_read_addr_out        , 0x1C065508
 .equ Adr_NX50_sm_read_addr_out               , 0x1C065508
 .equ Adr_NX50_fmmusm_sm_write_addr_out       , 0x1C06550C
 .equ Adr_NX50_sm_write_addr_out              , 0x1C06550C
 .equ Adr_NX50_fmmusm_fmmu_read_bit_rol_pos   , 0x1C065510
 .equ Adr_NX50_fmmu_read_bit_rol_pos          , 0x1C065510
 .equ Adr_NX50_fmmusm_fmmu_read_bit_mask      , 0x1C065514
 .equ Adr_NX50_fmmu_read_bit_mask             , 0x1C065514
 .equ Adr_NX50_fmmusm_fmmu_write_bit_rol_pos  , 0x1C065518
 .equ Adr_NX50_fmmu_write_bit_rol_pos         , 0x1C065518
 .equ Adr_NX50_fmmusm_fmmu_write_bit_mask     , 0x1C06551C
 .equ Adr_NX50_fmmu_write_bit_mask            , 0x1C06551C
 .equ Adr_NX50_fmmusm_fmmusm_len_en           , 0x1C065520
 .equ Adr_NX50_fmmusm_len_en                  , 0x1C065520
 .equ Adr_NX50_fmmusm_fmmusm_status_out       , 0x1C065524
 .equ Adr_NX50_fmmusm_status_out              , 0x1C065524
 .equ Adr_NX50_fmmusm_sm_buf_statcfg          , 0x1C065528
 .equ Adr_NX50_sm_buf_statcfg                 , 0x1C065528
 .equ Adr_NX50_fmmusm_sm_read_event           , 0x1C06552C
 .equ Adr_NX50_sm_read_event                  , 0x1C06552C
 .equ Adr_NX50_fmmusm_sm_write_event          , 0x1C065530
 .equ Adr_NX50_sm_write_event                 , 0x1C065530
 .equ Adr_NX50_fmmusm_sm_first_byte_addressed , 0x1C065534
 .equ Adr_NX50_sm_first_byte_addressed        , 0x1C065534
 .equ Adr_NX50_fmmusm_sm_last_byte_addressed  , 0x1C065538
 .equ Adr_NX50_sm_last_byte_addressed         , 0x1C065538
 .equ Adr_NX50_fmmusm_sm_served               , 0x1C06553C
 .equ Adr_NX50_sm_served                      , 0x1C06553C

@ =====================================================================
@
@ Area buf_man
 .equ Addr_NX50_buf_man, 0x1C065600
@
@ =====================================================================
 .equ Adr_NX50_buf_man_buf_man_xpec0, 0x1C065600
 .equ Adr_NX50_buf_man_xpec0        , 0x1C065600
 .equ Adr_NX50_buf_man_buf_man_xpec1, 0x1C065604
 .equ Adr_NX50_buf_man_xpec1        , 0x1C065604
 .equ Adr_NX50_buf_man_buf_man_arm  , 0x1C065608
 .equ Adr_NX50_buf_man_arm          , 0x1C065608
 .equ Adr_NX50_buf_man_buf_man_hif  , 0x1C06560C
 .equ Adr_NX50_buf_man_hif          , 0x1C06560C

@ =====================================================================
@
@ Area xc_extbus_sel
 .equ Addr_NX50_xc_extbus_sel, 0x1C080000
@
@ =====================================================================
 .equ Adr_NX50_xc_extbus_sel_xc_extbus_sel_ctrl, 0x1C080000
 .equ Adr_NX50_xc_extbus_sel_ctrl              , 0x1C080000

@ =====================================================================
@
@ Area far_bootb
 .equ Addr_NX50_far_bootb, 0x1C7FE000
@
@ =====================================================================
@ =====================================================================
@
@ Area vic
 .equ Addr_NX50_vic, 0x1C7FF000
@
@ =====================================================================
 .equ Adr_NX50_vic_vic_irq_status   , 0x1C7FF000
 .equ Adr_NX50_vic_irq_status       , 0x1C7FF000
 .equ Adr_NX50_vic_vic_fiq_status   , 0x1C7FF004
 .equ Adr_NX50_vic_fiq_status       , 0x1C7FF004
 .equ Adr_NX50_vic_vic_raw_intr     , 0x1C7FF008
 .equ Adr_NX50_vic_raw_intr         , 0x1C7FF008
 .equ Adr_NX50_vic_vic_int_select   , 0x1C7FF00C
 .equ Adr_NX50_vic_int_select       , 0x1C7FF00C
 .equ Adr_NX50_vic_vic_int_enable   , 0x1C7FF010
 .equ Adr_NX50_vic_int_enable       , 0x1C7FF010
 .equ Adr_NX50_vic_vic_int_enclear  , 0x1C7FF014
 .equ Adr_NX50_vic_int_enclear      , 0x1C7FF014
 .equ Adr_NX50_vic_vic_softint      , 0x1C7FF018
 .equ Adr_NX50_vic_softint          , 0x1C7FF018
 .equ Adr_NX50_vic_vic_softint_clear, 0x1C7FF01C
 .equ Adr_NX50_vic_softint_clear    , 0x1C7FF01C
 .equ Adr_NX50_vic_vic_protection   , 0x1C7FF020
 .equ Adr_NX50_vic_protection       , 0x1C7FF020
 .equ Adr_NX50_vic_vic_vect_addr    , 0x1C7FF030
 .equ Adr_NX50_vic_vect_addr        , 0x1C7FF030
 .equ Adr_NX50_vic_vic_def_vect_addr, 0x1C7FF034
 .equ Adr_NX50_vic_def_vect_addr    , 0x1C7FF034
 .equ Adr_NX50_vic_vic_vect_addr0   , 0x1C7FF100
 .equ Adr_NX50_vic_vect_addr0       , 0x1C7FF100
 .equ Adr_NX50_vic_vic_vect_addr1   , 0x1C7FF104
 .equ Adr_NX50_vic_vect_addr1       , 0x1C7FF104
 .equ Adr_NX50_vic_vic_vect_addr2   , 0x1C7FF108
 .equ Adr_NX50_vic_vect_addr2       , 0x1C7FF108
 .equ Adr_NX50_vic_vic_vect_addr3   , 0x1C7FF10C
 .equ Adr_NX50_vic_vect_addr3       , 0x1C7FF10C
 .equ Adr_NX50_vic_vic_vect_addr4   , 0x1C7FF110
 .equ Adr_NX50_vic_vect_addr4       , 0x1C7FF110
 .equ Adr_NX50_vic_vic_vect_addr5   , 0x1C7FF114
 .equ Adr_NX50_vic_vect_addr5       , 0x1C7FF114
 .equ Adr_NX50_vic_vic_vect_addr6   , 0x1C7FF118
 .equ Adr_NX50_vic_vect_addr6       , 0x1C7FF118
 .equ Adr_NX50_vic_vic_vect_addr7   , 0x1C7FF11C
 .equ Adr_NX50_vic_vect_addr7       , 0x1C7FF11C
 .equ Adr_NX50_vic_vic_vect_addr8   , 0x1C7FF120
 .equ Adr_NX50_vic_vect_addr8       , 0x1C7FF120
 .equ Adr_NX50_vic_vic_vect_addr9   , 0x1C7FF124
 .equ Adr_NX50_vic_vect_addr9       , 0x1C7FF124
 .equ Adr_NX50_vic_vic_vect_addr10  , 0x1C7FF128
 .equ Adr_NX50_vic_vect_addr10      , 0x1C7FF128
 .equ Adr_NX50_vic_vic_vect_addr11  , 0x1C7FF12C
 .equ Adr_NX50_vic_vect_addr11      , 0x1C7FF12C
 .equ Adr_NX50_vic_vic_vect_addr12  , 0x1C7FF130
 .equ Adr_NX50_vic_vect_addr12      , 0x1C7FF130
 .equ Adr_NX50_vic_vic_vect_addr13  , 0x1C7FF134
 .equ Adr_NX50_vic_vect_addr13      , 0x1C7FF134
 .equ Adr_NX50_vic_vic_vect_addr14  , 0x1C7FF138
 .equ Adr_NX50_vic_vect_addr14      , 0x1C7FF138
 .equ Adr_NX50_vic_vic_vect_addr15  , 0x1C7FF13C
 .equ Adr_NX50_vic_vect_addr15      , 0x1C7FF13C
 .equ Adr_NX50_vic_vic_vect_cntl0   , 0x1C7FF200
 .equ Adr_NX50_vic_vect_cntl0       , 0x1C7FF200
 .equ Adr_NX50_vic_vic_vect_cntl1   , 0x1C7FF204
 .equ Adr_NX50_vic_vect_cntl1       , 0x1C7FF204
 .equ Adr_NX50_vic_vic_vect_cntl2   , 0x1C7FF208
 .equ Adr_NX50_vic_vect_cntl2       , 0x1C7FF208
 .equ Adr_NX50_vic_vic_vect_cntl3   , 0x1C7FF20C
 .equ Adr_NX50_vic_vect_cntl3       , 0x1C7FF20C
 .equ Adr_NX50_vic_vic_vect_cntl4   , 0x1C7FF210
 .equ Adr_NX50_vic_vect_cntl4       , 0x1C7FF210
 .equ Adr_NX50_vic_vic_vect_cntl5   , 0x1C7FF214
 .equ Adr_NX50_vic_vect_cntl5       , 0x1C7FF214
 .equ Adr_NX50_vic_vic_vect_cntl6   , 0x1C7FF218
 .equ Adr_NX50_vic_vect_cntl6       , 0x1C7FF218
 .equ Adr_NX50_vic_vic_vect_cntl7   , 0x1C7FF21C
 .equ Adr_NX50_vic_vect_cntl7       , 0x1C7FF21C
 .equ Adr_NX50_vic_vic_vect_cntl8   , 0x1C7FF220
 .equ Adr_NX50_vic_vect_cntl8       , 0x1C7FF220
 .equ Adr_NX50_vic_vic_vect_cntl9   , 0x1C7FF224
 .equ Adr_NX50_vic_vect_cntl9       , 0x1C7FF224
 .equ Adr_NX50_vic_vic_vect_cntl10  , 0x1C7FF228
 .equ Adr_NX50_vic_vect_cntl10      , 0x1C7FF228
 .equ Adr_NX50_vic_vic_vect_cntl11  , 0x1C7FF22C
 .equ Adr_NX50_vic_vect_cntl11      , 0x1C7FF22C
 .equ Adr_NX50_vic_vic_vect_cntl12  , 0x1C7FF230
 .equ Adr_NX50_vic_vect_cntl12      , 0x1C7FF230
 .equ Adr_NX50_vic_vic_vect_cntl13  , 0x1C7FF234
 .equ Adr_NX50_vic_vect_cntl13      , 0x1C7FF234
 .equ Adr_NX50_vic_vic_vect_cntl14  , 0x1C7FF238
 .equ Adr_NX50_vic_vect_cntl14      , 0x1C7FF238
 .equ Adr_NX50_vic_vic_vect_cntl15  , 0x1C7FF23C
 .equ Adr_NX50_vic_vect_cntl15      , 0x1C7FF23C

@ =====================================================================
@
@ Area pci_ahbls6
 .equ Addr_NX50_pci_ahbls6, 0x20000000
@
@ =====================================================================
 .equ Adr_NX50_pci_ahbls6_pci_ahbls6_BASE, 0x20000000
 .equ Adr_NX50_pci_ahbls6_BASE           , 0x20000000

@ =====================================================================
@
@ Area extmemory_ahbls7
 .equ Addr_NX50_extmemory_ahbls7, 0x80000000
@
@ =====================================================================
@ =====================================================================
@
@ Area sdram
 .equ Addr_NX50_sdram, 0x80000000
@
@ =====================================================================
 .equ Adr_NX50_sdram_sdram_base, 0x80000000
 .equ Adr_NX50_sdram_base      , 0x80000000
 .equ Adr_NX50_sdram_sdram_end , 0xBFFFFFFC
 .equ Adr_NX50_sdram_end       , 0xBFFFFFFC

@ =====================================================================
@
@ Area extsram
 .equ Addr_NX50_extsram, 0xC0000000
@
@ =====================================================================
@ =====================================================================
@
@ Area extsram0
 .equ Addr_NX50_extsram0, 0xC0000000
@
@ =====================================================================
 .equ Adr_NX50_extsram0_extsram0_base, 0xC0000000
 .equ Adr_NX50_extsram0_base         , 0xC0000000
 .equ Adr_NX50_extsram0_extsram0_end , 0xC7FFFFFC
 .equ Adr_NX50_extsram0_end          , 0xC7FFFFFC

@ =====================================================================
@
@ Area extsram1
 .equ Addr_NX50_extsram1, 0xC8000000
@
@ =====================================================================
 .equ Adr_NX50_extsram1_extsram1_base, 0xC8000000
 .equ Adr_NX50_extsram1_base         , 0xC8000000
 .equ Adr_NX50_extsram1_extsram1_end , 0xCFFFFFFC
 .equ Adr_NX50_extsram1_end          , 0xCFFFFFFC

@ =====================================================================
@
@ Area extsram2
 .equ Addr_NX50_extsram2, 0xD0000000
@
@ =====================================================================
 .equ Adr_NX50_extsram2_extsram2_base, 0xD0000000
 .equ Adr_NX50_extsram2_base         , 0xD0000000
 .equ Adr_NX50_extsram2_extsram2_end , 0xD7FFFFFC
 .equ Adr_NX50_extsram2_end          , 0xD7FFFFFC

@ =====================================================================
@
@ Area extsram3
 .equ Addr_NX50_extsram3, 0xD8000000
@
@ =====================================================================
@ =====================================================================
@
@ Area ahbls4_mirror_hi
 .equ Addr_NX50_ahbls4_mirror_hi, 0xFF200000
@
@ =====================================================================
@ =====================================================================
@
@ Area intlogic_mirror_hi
 .equ Addr_NX50_intlogic_mirror_hi, 0xFF800000
@
@ =====================================================================
@ =====================================================================
@
@ Area boot_rom_adr0
@
@ =====================================================================


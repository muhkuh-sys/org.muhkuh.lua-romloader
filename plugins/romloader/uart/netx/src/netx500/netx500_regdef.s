@ ******************************************************************
@ from regdef.v by regdef2s auto-generated Assembler file           
@ please see the regdef.html file for detailed register description 
@ ******************************************************************

@ =====================================================================
@
@ Area sram0_ahbls0
 .equ Addr_NX500_NX500_sram0_ahbls0, 0x00000000
@
@ =====================================================================
 .equ Adr_NX500_NX500_sram0_ahbls0_intram0_base, 0x00000004
 .equ Adr_NX500_intram0_base             , 0x00000004
 .equ Adr_NX500_sram0_ahbls0_intram0_end , 0x00007FFC
 .equ Adr_NX500_intram0_end              , 0x00007FFC

@ =====================================================================
@
@ Area sram1_ahbls1
 .equ Addr_NX500_sram1_ahbls1, 0x00008000
@
@ =====================================================================
 .equ Adr_NX500_sram1_ahbls1_intram1_base, 0x00008000
 .equ Adr_NX500_intram1_base, 0x00008000
 .equ Adr_NX500_sram1_ahbls1_intram1_end, 0x0000FFFC
 .equ Adr_NX500_intram1_end, 0x0000FFFC

@ =====================================================================
@
@ Area sram2_ahbls2
 .equ Addr_NX500_sram2_ahbls2, 0x00010000
@
@ =====================================================================
 .equ Adr_NX500_sram2_ahbls2_intram2_base, 0x00010000
 .equ Adr_NX500_intram2_base, 0x00010000
 .equ Adr_NX500_sram2_ahbls2_intram2_end, 0x00017FFC
 .equ Adr_NX500_intram2_end, 0x00017FFC

@ =====================================================================
@
@ Area sram3_ahbls3
 .equ Addr_NX500_sram3_ahbls3, 0x00018000
@
@ =====================================================================
 .equ Adr_NX500_sram3_ahbls3_intram3_base, 0x00018000
 .equ Adr_NX500_intram3_base, 0x00018000
 .equ Adr_NX500_sram3_ahbls3_intram3_end, 0x0001FFFC
 .equ Adr_NX500_intram3_end, 0x0001FFFC

@ =====================================================================
@
@ Area intlogic_ahbls5
 .equ Addr_NX500_intlogic_ahbls5, 0x00100000
@
@ =====================================================================
@ =====================================================================
@
@ Area asic_ctrl
 .equ Addr_NX500_asic_ctrl, 0x00100000
@
@ =====================================================================
 .equ Adr_NX500_asic_ctrl_io_status, 0x00100000
 .equ Adr_NX500_io_status, 0x00100000
 .equ Adr_NX500_asic_ctrl_io_config, 0x00100004
 .equ Adr_NX500_io_config, 0x00100004
 .equ Adr_NX500_asic_ctrl_io_config_mask, 0x00100008
 .equ Adr_NX500_io_config_mask, 0x00100008
 .equ Adr_NX500_asic_ctrl_reset_ctrl, 0x0010000C
 .equ Adr_NX500_reset_ctrl, 0x0010000C
 .equ Adr_NX500_asic_ctrl_phy_control, 0x00100010
 .equ Adr_NX500_phy_control, 0x00100010
 .equ Adr_NX500_asic_ctrl_armclk_rate_mul_add, 0x00100014
 .equ Adr_NX500_armclk_rate_mul_add, 0x00100014
 .equ Adr_NX500_asic_ctrl_usbclk_rate_mul_add, 0x00100018
 .equ Adr_NX500_usbclk_rate_mul_add, 0x00100018
 .equ Adr_NX500_asic_ctrl_adcclk_rate_mul_add, 0x0010001C
 .equ Adr_NX500_adcclk_rate_mul_add, 0x0010001C
 .equ Adr_NX500_asic_ctrl_phyclk_rate_mul_add, 0x00100020
 .equ Adr_NX500_phyclk_rate_mul_add, 0x00100020
 .equ Adr_NX500_asic_ctrl_clock_enable, 0x00100024
 .equ Adr_NX500_clock_enable, 0x00100024
 .equ Adr_NX500_asic_ctrl_clock_enable_mask, 0x00100028
 .equ Adr_NX500_clock_enable_mask, 0x00100028
 .equ Adr_NX500_asic_ctrl_misc_asic_ctrl, 0x0010002C
 .equ Adr_NX500_misc_asic_ctrl, 0x0010002C
 .equ Adr_NX500_asic_ctrl_exmem_priority_lock, 0x00100030
 .equ Adr_NX500_exmem_priority_lock, 0x00100030
 .equ Adr_NX500_asic_ctrl_netx_version, 0x00100034
 .equ Adr_NX500_netx_version, 0x00100034
 .equ Adr_NX500_asic_ctrl_asic_ctrl_access_key, 0x00100070
 .equ Adr_NX500_asic_ctrl_access_key, 0x00100070
 .equ Adr_NX500_asic_ctrl_netx_lock_reserved00, 0x00100074
 .equ Adr_NX500_netx_lock_reserved00, 0x00100074
 .equ Adr_NX500_asic_ctrl_netx_lock_reserved01, 0x00100078
 .equ Adr_NX500_netx_lock_reserved01, 0x00100078
 .equ Adr_NX500_asic_ctrl_netx_lock_reserved_start, 0x00100080
 .equ Adr_NX500_netx_lock_reserved_start, 0x00100080
 .equ Adr_NX500_asic_ctrl_netx_lock_reserved_end, 0x001000FC
 .equ Adr_NX500_netx_lock_reserved_end, 0x001000FC

@ =====================================================================
@
@ Area extmemctrl
 .equ Addr_NX500_extmemctrl, 0x00100100
@
@ =====================================================================
@ =====================================================================
@
@ Area ext_asyncmem_ctrl
 .equ Addr_NX500_ext_asyncmem_ctrl, 0x00100100
@
@ =====================================================================
 .equ Adr_NX500_ext_asyncmem_ctrl_extsram0_ctrl, 0x00100100
 .equ Adr_NX500_extsram0_ctrl, 0x00100100
 .equ Adr_NX500_ext_asyncmem_ctrl_extsram1_ctrl, 0x00100104
 .equ Adr_NX500_extsram1_ctrl, 0x00100104
 .equ Adr_NX500_ext_asyncmem_ctrl_extsram2_ctrl, 0x00100108
 .equ Adr_NX500_extsram2_ctrl, 0x00100108
 .equ Adr_NX500_ext_asyncmem_ctrl_extsram3_ctrl, 0x0010010C
 .equ Adr_NX500_extsram3_ctrl, 0x0010010C

@ =====================================================================
@
@ Area ext_sdram_ctrl
 .equ Addr_NX500_ext_sdram_ctrl, 0x00100140
@
@ =====================================================================
 .equ Adr_NX500_ext_sdram_ctrl_sdram_general_ctrl, 0x00100140
 .equ Adr_NX500_sdram_general_ctrl, 0x00100140
 .equ Adr_NX500_ext_sdram_ctrl_sdram_timing_ctrl, 0x00100144
 .equ Adr_NX500_sdram_timing_ctrl, 0x00100144
 .equ Adr_NX500_ext_sdram_ctrl_sdram_mr, 0x00100148
 .equ Adr_NX500_sdram_mr, 0x00100148
 .equ Adr_NX500_ext_sdram_ctrl_sdram_emr2, 0x0010014C
 .equ Adr_NX500_sdram_emr2, 0x0010014C

@ =====================================================================
@
@ Area extmem_priority_ctrl
 .equ Addr_NX500_extmem_priority_ctrl, 0x00100180
@
@ =====================================================================
 .equ Adr_NX500_extmem_priority_ctrl_extmem_prio_timslot_ctrl, 0x00100180
 .equ Adr_NX500_extmem_prio_timslot_ctrl, 0x00100180
 .equ Adr_NX500_extmem_priority_ctrl_extmem_prio_accesstime_ctrl, 0x00100184
 .equ Adr_NX500_extmem_prio_accesstime_ctrl, 0x00100184

@ =====================================================================
@
@ Area watchdog
 .equ Addr_NX500_watchdog, 0x00100200
@
@ =====================================================================
 .equ Adr_NX500_watchdog_netx_sys_wdg_ctrl, 0x00100200
 .equ Adr_NX500_netx_sys_wdg_ctrl, 0x00100200
 .equ Adr_NX500_watchdog_netx_sys_wdg, 0x00100204
 .equ Adr_NX500_netx_sys_wdg, 0x00100204
 .equ Adr_NX500_watchdog_netx_sys_wdg_irq_timeout, 0x00100208
 .equ Adr_NX500_netx_sys_wdg_irq_timeout, 0x00100208
 .equ Adr_NX500_watchdog_netx_sys_wdg_res_timeout, 0x0010020C
 .equ Adr_NX500_netx_sys_wdg_res_timeout, 0x0010020C
 .equ Adr_NX500_watchdog_netx_sys_wdg_ctrl_mirror1, 0x00100210
 .equ Adr_NX500_netx_sys_wdg_ctrl_mirror1, 0x00100210
 .equ Adr_NX500_watchdog_netx_sys_wdg_mirror1, 0x00100214
 .equ Adr_NX500_netx_sys_wdg_mirror1, 0x00100214
 .equ Adr_NX500_watchdog_netx_sys_wdg_irq_timeout_mirror1, 0x00100218
 .equ Adr_NX500_netx_sys_wdg_irq_timeout_mirror1, 0x00100218
 .equ Adr_NX500_watchdog_netx_sys_wdg_res_timeout_mirror1, 0x0010021C
 .equ Adr_NX500_netx_sys_wdg_res_timeout_mirror1, 0x0010021C
 .equ Adr_NX500_watchdog_netx_sys_wdg_ctrl_mirror2, 0x00100220
 .equ Adr_NX500_netx_sys_wdg_ctrl_mirror2, 0x00100220
 .equ Adr_NX500_watchdog_netx_sys_wdg_mirror2, 0x00100224
 .equ Adr_NX500_netx_sys_wdg_mirror2, 0x00100224
 .equ Adr_NX500_watchdog_netx_sys_wdg_irq_timeout_mirror2, 0x00100228
 .equ Adr_NX500_netx_sys_wdg_irq_timeout_mirror2, 0x00100228
 .equ Adr_NX500_watchdog_netx_sys_wdg_res_timeout_mirror2, 0x0010022C
 .equ Adr_NX500_netx_sys_wdg_res_timeout_mirror2, 0x0010022C
 .equ Adr_NX500_watchdog_netx_sys_wdg_ctrl_mirror3, 0x00100230
 .equ Adr_NX500_netx_sys_wdg_ctrl_mirror3, 0x00100230
 .equ Adr_NX500_watchdog_netx_sys_wdg_mirror3, 0x00100234
 .equ Adr_NX500_netx_sys_wdg_mirror3, 0x00100234
 .equ Adr_NX500_watchdog_netx_sys_wdg_irq_timeout_mirror3, 0x00100238
 .equ Adr_NX500_netx_sys_wdg_irq_timeout_mirror3, 0x00100238
 .equ Adr_NX500_watchdog_netx_sys_wdg_res_timeout_mirror3, 0x0010023C
 .equ Adr_NX500_netx_sys_wdg_res_timeout_mirror3, 0x0010023C
 .equ Adr_NX500_watchdog_netx_sys_wdg_ctrl_mirror4, 0x00100240
 .equ Adr_NX500_netx_sys_wdg_ctrl_mirror4, 0x00100240
 .equ Adr_NX500_watchdog_netx_sys_wdg_mirror4, 0x00100244
 .equ Adr_NX500_netx_sys_wdg_mirror4, 0x00100244
 .equ Adr_NX500_watchdog_netx_sys_wdg_irq_timeout_mirror4, 0x00100248
 .equ Adr_NX500_netx_sys_wdg_irq_timeout_mirror4, 0x00100248
 .equ Adr_NX500_watchdog_netx_sys_wdg_res_timeout_mirror4, 0x0010024C
 .equ Adr_NX500_netx_sys_wdg_res_timeout_mirror4, 0x0010024C
 .equ Adr_NX500_watchdog_netx_sys_wdg_ctrl_mirror5, 0x00100250
 .equ Adr_NX500_netx_sys_wdg_ctrl_mirror5, 0x00100250
 .equ Adr_NX500_watchdog_netx_sys_wdg_mirror5, 0x00100254
 .equ Adr_NX500_netx_sys_wdg_mirror5, 0x00100254
 .equ Adr_NX500_watchdog_netx_sys_wdg_irq_timeout_mirror5, 0x00100258
 .equ Adr_NX500_netx_sys_wdg_irq_timeout_mirror5, 0x00100258
 .equ Adr_NX500_watchdog_netx_sys_wdg_res_timeout_mirror5, 0x0010025C
 .equ Adr_NX500_netx_sys_wdg_res_timeout_mirror5, 0x0010025C
 .equ Adr_NX500_watchdog_netx_sys_wdg_ctrl_mirror6, 0x00100260
 .equ Adr_NX500_netx_sys_wdg_ctrl_mirror6, 0x00100260
 .equ Adr_NX500_watchdog_netx_sys_wdg_mirror6, 0x00100264
 .equ Adr_NX500_netx_sys_wdg_mirror6, 0x00100264
 .equ Adr_NX500_watchdog_netx_sys_wdg_irq_timeout_mirror6, 0x00100268
 .equ Adr_NX500_netx_sys_wdg_irq_timeout_mirror6, 0x00100268
 .equ Adr_NX500_watchdog_netx_sys_wdg_res_timeout_mirror6, 0x0010026C
 .equ Adr_NX500_netx_sys_wdg_res_timeout_mirror6, 0x0010026C
 .equ Adr_NX500_watchdog_netx_sys_wdg_ctrl_mirror7, 0x00100270
 .equ Adr_NX500_netx_sys_wdg_ctrl_mirror7, 0x00100270
 .equ Adr_NX500_watchdog_netx_sys_wdg_mirror7, 0x00100274
 .equ Adr_NX500_netx_sys_wdg_mirror7, 0x00100274
 .equ Adr_NX500_watchdog_netx_sys_wdg_irq_timeout_mirror7, 0x00100278
 .equ Adr_NX500_netx_sys_wdg_irq_timeout_mirror7, 0x00100278
 .equ Adr_NX500_watchdog_netx_sys_wdg_res_timeout_mirror7, 0x0010027C
 .equ Adr_NX500_netx_sys_wdg_res_timeout_mirror7, 0x0010027C
 .equ Adr_NX500_watchdog_netx_sys_wdg_ctrl_mirror8, 0x00100280
 .equ Adr_NX500_netx_sys_wdg_ctrl_mirror8, 0x00100280
 .equ Adr_NX500_watchdog_netx_sys_wdg_mirror8, 0x00100284
 .equ Adr_NX500_netx_sys_wdg_mirror8, 0x00100284
 .equ Adr_NX500_watchdog_netx_sys_wdg_irq_timeout_mirror8, 0x00100288
 .equ Adr_NX500_netx_sys_wdg_irq_timeout_mirror8, 0x00100288
 .equ Adr_NX500_watchdog_netx_sys_wdg_res_timeout_mirror8, 0x0010028C
 .equ Adr_NX500_netx_sys_wdg_res_timeout_mirror8, 0x0010028C
 .equ Adr_NX500_watchdog_netx_sys_wdg_ctrl_mirror9, 0x00100290
 .equ Adr_NX500_netx_sys_wdg_ctrl_mirror9, 0x00100290
 .equ Adr_NX500_watchdog_netx_sys_wdg_mirror9, 0x00100294
 .equ Adr_NX500_netx_sys_wdg_mirror9, 0x00100294
 .equ Adr_NX500_watchdog_netx_sys_wdg_irq_timeout_mirror9, 0x00100298
 .equ Adr_NX500_netx_sys_wdg_irq_timeout_mirror9, 0x00100298
 .equ Adr_NX500_watchdog_netx_sys_wdg_res_timeout_mirror9, 0x0010029C
 .equ Adr_NX500_netx_sys_wdg_res_timeout_mirror9, 0x0010029C
 .equ Adr_NX500_watchdog_netx_sys_wdg_ctrl_mirror10, 0x001002A0
 .equ Adr_NX500_netx_sys_wdg_ctrl_mirror10, 0x001002A0
 .equ Adr_NX500_watchdog_netx_sys_wdg_mirror10, 0x001002A4
 .equ Adr_NX500_netx_sys_wdg_mirror10, 0x001002A4
 .equ Adr_NX500_watchdog_netx_sys_wdg_irq_timeout_mirror10, 0x001002A8
 .equ Adr_NX500_netx_sys_wdg_irq_timeout_mirror10, 0x001002A8
 .equ Adr_NX500_watchdog_netx_sys_wdg_res_timeout_mirror10, 0x001002AC
 .equ Adr_NX500_netx_sys_wdg_res_timeout_mirror10, 0x001002AC
 .equ Adr_NX500_watchdog_netx_sys_wdg_ctrl_mirror11, 0x001002B0
 .equ Adr_NX500_netx_sys_wdg_ctrl_mirror11, 0x001002B0
 .equ Adr_NX500_watchdog_netx_sys_wdg_mirror11, 0x001002B4
 .equ Adr_NX500_netx_sys_wdg_mirror11, 0x001002B4
 .equ Adr_NX500_watchdog_netx_sys_wdg_irq_timeout_mirror11, 0x001002B8
 .equ Adr_NX500_netx_sys_wdg_irq_timeout_mirror11, 0x001002B8
 .equ Adr_NX500_watchdog_netx_sys_wdg_res_timeout_mirror11, 0x001002BC
 .equ Adr_NX500_netx_sys_wdg_res_timeout_mirror11, 0x001002BC
 .equ Adr_NX500_watchdog_netx_sys_wdg_ctrl_mirror12, 0x001002C0
 .equ Adr_NX500_netx_sys_wdg_ctrl_mirror12, 0x001002C0
 .equ Adr_NX500_watchdog_netx_sys_wdg_mirror12, 0x001002C4
 .equ Adr_NX500_netx_sys_wdg_mirror12, 0x001002C4
 .equ Adr_NX500_watchdog_netx_sys_wdg_irq_timeout_mirror12, 0x001002C8
 .equ Adr_NX500_netx_sys_wdg_irq_timeout_mirror12, 0x001002C8
 .equ Adr_NX500_watchdog_netx_sys_wdg_res_timeout_mirror12, 0x001002CC
 .equ Adr_NX500_netx_sys_wdg_res_timeout_mirror12, 0x001002CC
 .equ Adr_NX500_watchdog_netx_sys_wdg_ctrl_mirror13, 0x001002D0
 .equ Adr_NX500_netx_sys_wdg_ctrl_mirror13, 0x001002D0
 .equ Adr_NX500_watchdog_netx_sys_wdg_mirror13, 0x001002D4
 .equ Adr_NX500_netx_sys_wdg_mirror13, 0x001002D4
 .equ Adr_NX500_watchdog_netx_sys_wdg_irq_timeout_mirror13, 0x001002D8
 .equ Adr_NX500_netx_sys_wdg_irq_timeout_mirror13, 0x001002D8
 .equ Adr_NX500_watchdog_netx_sys_wdg_res_timeout_mirror13, 0x001002DC
 .equ Adr_NX500_netx_sys_wdg_res_timeout_mirror13, 0x001002DC
 .equ Adr_NX500_watchdog_netx_sys_wdg_ctrl_mirror14, 0x001002E0
 .equ Adr_NX500_netx_sys_wdg_ctrl_mirror14, 0x001002E0
 .equ Adr_NX500_watchdog_netx_sys_wdg_mirror14, 0x001002E4
 .equ Adr_NX500_netx_sys_wdg_mirror14, 0x001002E4
 .equ Adr_NX500_watchdog_netx_sys_wdg_irq_timeout_mirror14, 0x001002E8
 .equ Adr_NX500_netx_sys_wdg_irq_timeout_mirror14, 0x001002E8
 .equ Adr_NX500_watchdog_netx_sys_wdg_res_timeout_mirror14, 0x001002EC
 .equ Adr_NX500_netx_sys_wdg_res_timeout_mirror14, 0x001002EC
 .equ Adr_NX500_watchdog_netx_sys_wdg_ctrl_mirror15, 0x001002F0
 .equ Adr_NX500_netx_sys_wdg_ctrl_mirror15, 0x001002F0
 .equ Adr_NX500_watchdog_netx_sys_wdg_mirror15, 0x001002F4
 .equ Adr_NX500_netx_sys_wdg_mirror15, 0x001002F4
 .equ Adr_NX500_watchdog_netx_sys_wdg_irq_timeout_mirror15, 0x001002F8
 .equ Adr_NX500_netx_sys_wdg_irq_timeout_mirror15, 0x001002F8
 .equ Adr_NX500_watchdog_netx_sys_wdg_res_timeout_mirror15, 0x001002FC
 .equ Adr_NX500_netx_sys_wdg_res_timeout_mirror15, 0x001002FC

@ =====================================================================
@
@ Area abort
 .equ Addr_NX500_abort, 0x00100300
@
@ =====================================================================
@ =====================================================================
@
@ Area gpio
 .equ Addr_NX500_gpio, 0x00100800
@
@ =====================================================================
 .equ Adr_NX500_gpio_gpio_cfg0, 0x00100800
 .equ Adr_NX500_gpio_cfg0, 0x00100800
 .equ Adr_NX500_gpio_gpio_cfg1, 0x00100804
 .equ Adr_NX500_gpio_cfg1, 0x00100804
 .equ Adr_NX500_gpio_gpio_cfg2, 0x00100808
 .equ Adr_NX500_gpio_cfg2, 0x00100808
 .equ Adr_NX500_gpio_gpio_cfg3, 0x0010080C
 .equ Adr_NX500_gpio_cfg3, 0x0010080C
 .equ Adr_NX500_gpio_gpio_cfg4, 0x00100810
 .equ Adr_NX500_gpio_cfg4, 0x00100810
 .equ Adr_NX500_gpio_gpio_cfg5, 0x00100814
 .equ Adr_NX500_gpio_cfg5, 0x00100814
 .equ Adr_NX500_gpio_gpio_cfg6, 0x00100818
 .equ Adr_NX500_gpio_cfg6, 0x00100818
 .equ Adr_NX500_gpio_gpio_cfg7, 0x0010081C
 .equ Adr_NX500_gpio_cfg7, 0x0010081C
 .equ Adr_NX500_gpio_gpio_cfg8, 0x00100820
 .equ Adr_NX500_gpio_cfg8, 0x00100820
 .equ Adr_NX500_gpio_gpio_cfg9, 0x00100824
 .equ Adr_NX500_gpio_cfg9, 0x00100824
 .equ Adr_NX500_gpio_gpio_cfg10, 0x00100828
 .equ Adr_NX500_gpio_cfg10, 0x00100828
 .equ Adr_NX500_gpio_gpio_cfg11, 0x0010082C
 .equ Adr_NX500_gpio_cfg11, 0x0010082C
 .equ Adr_NX500_gpio_gpio_cfg12, 0x00100830
 .equ Adr_NX500_gpio_cfg12, 0x00100830
 .equ Adr_NX500_gpio_gpio_cfg13, 0x00100834
 .equ Adr_NX500_gpio_cfg13, 0x00100834
 .equ Adr_NX500_gpio_gpio_cfg14, 0x00100838
 .equ Adr_NX500_gpio_cfg14, 0x00100838
 .equ Adr_NX500_gpio_gpio_cfg15, 0x0010083C
 .equ Adr_NX500_gpio_cfg15, 0x0010083C
 .equ Adr_NX500_gpio_gpio_tc0, 0x00100840
 .equ Adr_NX500_gpio_tc0, 0x00100840
 .equ Adr_NX500_gpio_gpio_tc1, 0x00100844
 .equ Adr_NX500_gpio_tc1, 0x00100844
 .equ Adr_NX500_gpio_gpio_tc2, 0x00100848
 .equ Adr_NX500_gpio_tc2, 0x00100848
 .equ Adr_NX500_gpio_gpio_tc3, 0x0010084C
 .equ Adr_NX500_gpio_tc3, 0x0010084C
 .equ Adr_NX500_gpio_gpio_tc4, 0x00100850
 .equ Adr_NX500_gpio_tc4, 0x00100850
 .equ Adr_NX500_gpio_gpio_tc5, 0x00100854
 .equ Adr_NX500_gpio_tc5, 0x00100854
 .equ Adr_NX500_gpio_gpio_tc6, 0x00100858
 .equ Adr_NX500_gpio_tc6, 0x00100858
 .equ Adr_NX500_gpio_gpio_tc7, 0x0010085C
 .equ Adr_NX500_gpio_tc7, 0x0010085C
 .equ Adr_NX500_gpio_gpio_tc8, 0x00100860
 .equ Adr_NX500_gpio_tc8, 0x00100860
 .equ Adr_NX500_gpio_gpio_tc9, 0x00100864
 .equ Adr_NX500_gpio_tc9, 0x00100864
 .equ Adr_NX500_gpio_gpio_tc10, 0x00100868
 .equ Adr_NX500_gpio_tc10, 0x00100868
 .equ Adr_NX500_gpio_gpio_tc11, 0x0010086C
 .equ Adr_NX500_gpio_tc11, 0x0010086C
 .equ Adr_NX500_gpio_gpio_tc12, 0x00100870
 .equ Adr_NX500_gpio_tc12, 0x00100870
 .equ Adr_NX500_gpio_gpio_tc13, 0x00100874
 .equ Adr_NX500_gpio_tc13, 0x00100874
 .equ Adr_NX500_gpio_gpio_tc14, 0x00100878
 .equ Adr_NX500_gpio_tc14, 0x00100878
 .equ Adr_NX500_gpio_gpio_tc15, 0x0010087C
 .equ Adr_NX500_gpio_tc15, 0x0010087C
 .equ Adr_NX500_gpio_gpio_counter0_ctrl, 0x00100880
 .equ Adr_NX500_gpio_counter0_ctrl, 0x00100880
 .equ Adr_NX500_gpio_gpio_counter1_ctrl, 0x00100884
 .equ Adr_NX500_gpio_counter1_ctrl, 0x00100884
 .equ Adr_NX500_gpio_gpio_counter2_ctrl, 0x00100888
 .equ Adr_NX500_gpio_counter2_ctrl, 0x00100888
 .equ Adr_NX500_gpio_gpio_counter3_ctrl, 0x0010088C
 .equ Adr_NX500_gpio_counter3_ctrl, 0x0010088C
 .equ Adr_NX500_gpio_gpio_counter4_ctrl, 0x00100890
 .equ Adr_NX500_gpio_counter4_ctrl, 0x00100890
 .equ Adr_NX500_gpio_gpio_counter0_max, 0x00100894
 .equ Adr_NX500_gpio_counter0_max, 0x00100894
 .equ Adr_NX500_gpio_gpio_counter1_max, 0x00100898
 .equ Adr_NX500_gpio_counter1_max, 0x00100898
 .equ Adr_NX500_gpio_gpio_counter2_max, 0x0010089C
 .equ Adr_NX500_gpio_counter2_max, 0x0010089C
 .equ Adr_NX500_gpio_gpio_counter3_max, 0x001008A0
 .equ Adr_NX500_gpio_counter3_max, 0x001008A0
 .equ Adr_NX500_gpio_gpio_counter4_max, 0x001008A4
 .equ Adr_NX500_gpio_counter4_max, 0x001008A4
 .equ Adr_NX500_gpio_gpio_counter0_cnt, 0x001008A8
 .equ Adr_NX500_gpio_counter0_cnt, 0x001008A8
 .equ Adr_NX500_gpio_gpio_counter1_cnt, 0x001008AC
 .equ Adr_NX500_gpio_counter1_cnt, 0x001008AC
 .equ Adr_NX500_gpio_gpio_counter2_cnt, 0x001008B0
 .equ Adr_NX500_gpio_counter2_cnt, 0x001008B0
 .equ Adr_NX500_gpio_gpio_counter3_cnt, 0x001008B4
 .equ Adr_NX500_gpio_counter3_cnt, 0x001008B4
 .equ Adr_NX500_gpio_gpio_counter4_cnt, 0x001008B8
 .equ Adr_NX500_gpio_counter4_cnt, 0x001008B8
 .equ Adr_NX500_gpio_gpio_irq_mask_set, 0x001008BC
 .equ Adr_NX500_gpio_irq_mask_set, 0x001008BC
 .equ Adr_NX500_gpio_gpio_irq_mask_rst, 0x001008C0
 .equ Adr_NX500_gpio_irq_mask_rst, 0x001008C0
 .equ Adr_NX500_gpio_gpio_systime_cmp, 0x001008C4
 .equ Adr_NX500_gpio_systime_cmp, 0x001008C4
 .equ Adr_NX500_gpio_gpio_line, 0x001008C8
 .equ Adr_NX500_gpio_line, 0x001008C8
 .equ Adr_NX500_gpio_gpio_in, 0x001008CC
 .equ Adr_NX500_gpio_in, 0x001008CC
 .equ Adr_NX500_gpio_gpio_irq, 0x001008D0
 .equ Adr_NX500_gpio_irq, 0x001008D0

@ =====================================================================
@
@ Area pio
 .equ Addr_NX500_pio, 0x00100900
@
@ =====================================================================
 .equ Adr_NX500_pio_pio_in, 0x00100900
 .equ Adr_NX500_pio_in, 0x00100900
 .equ Adr_NX500_pio_pio_out, 0x00100904
 .equ Adr_NX500_pio_out, 0x00100904
 .equ Adr_NX500_pio_pio_oe, 0x00100908
 .equ Adr_NX500_pio_oe, 0x00100908

@ =====================================================================
@
@ Area uart
 .equ Addr_NX500_uart0, 0x00100A00
 .equ Addr_NX500_uart1, 0x00100A40
 .equ Addr_NX500_uart2, 0x00100A80
@
@ =====================================================================
 .equ Adr_NX500_uart0_uartdr, 0x00100A00
 .equ Adr_NX500_uart1_uartdr, 0x00100A40
 .equ Adr_NX500_uart2_uartdr, 0x00100A80
 .equ Adr_NX500_uart0_uartrsr, 0x00100A04
 .equ Adr_NX500_uart1_uartrsr, 0x00100A44
 .equ Adr_NX500_uart2_uartrsr, 0x00100A84
 .equ Adr_NX500_uart0_uartlcr_h, 0x00100A08
 .equ Adr_NX500_uart1_uartlcr_h, 0x00100A48
 .equ Adr_NX500_uart2_uartlcr_h, 0x00100A88
 .equ Adr_NX500_uart0_uartlcr_m, 0x00100A0C
 .equ Adr_NX500_uart1_uartlcr_m, 0x00100A4C
 .equ Adr_NX500_uart2_uartlcr_m, 0x00100A8C
 .equ Adr_NX500_uart0_uartlcr_l, 0x00100A10
 .equ Adr_NX500_uart1_uartlcr_l, 0x00100A50
 .equ Adr_NX500_uart2_uartlcr_l, 0x00100A90
 .equ Adr_NX500_uart0_uartcr, 0x00100A14
 .equ Adr_NX500_uart1_uartcr, 0x00100A54
 .equ Adr_NX500_uart2_uartcr, 0x00100A94
 .equ Adr_NX500_uart0_uartfr, 0x00100A18
 .equ Adr_NX500_uart1_uartfr, 0x00100A58
 .equ Adr_NX500_uart2_uartfr, 0x00100A98
 .equ Adr_NX500_uart0_uartiir, 0x00100A1C
 .equ Adr_NX500_uart1_uartiir, 0x00100A5C
 .equ Adr_NX500_uart2_uartiir, 0x00100A9C
 .equ Adr_NX500_uart0_uartilpr, 0x00100A20
 .equ Adr_NX500_uart1_uartilpr, 0x00100A60
 .equ Adr_NX500_uart2_uartilpr, 0x00100AA0
 .equ Adr_NX500_uart0_uartrts, 0x00100A24
 .equ Adr_NX500_uart1_uartrts, 0x00100A64
 .equ Adr_NX500_uart2_uartrts, 0x00100AA4
 .equ Adr_NX500_uart0_uartforerun, 0x00100A28
 .equ Adr_NX500_uart1_uartforerun, 0x00100A68
 .equ Adr_NX500_uart2_uartforerun, 0x00100AA8
 .equ Adr_NX500_uart0_uarttrail, 0x00100A2C
 .equ Adr_NX500_uart1_uarttrail, 0x00100A6C
 .equ Adr_NX500_uart2_uarttrail, 0x00100AAC
 .equ Adr_NX500_uart0_uartdrvout, 0x00100A30
 .equ Adr_NX500_uart1_uartdrvout, 0x00100A70
 .equ Adr_NX500_uart2_uartdrvout, 0x00100AB0
 .equ Adr_NX500_uart0_uartcr_2, 0x00100A34
 .equ Adr_NX500_uart1_uartcr_2, 0x00100A74
 .equ Adr_NX500_uart2_uartcr_2, 0x00100AB4
 .equ Adr_NX500_uart0_uartrxiflsel, 0x00100A38
 .equ Adr_NX500_uart1_uartrxiflsel, 0x00100A78
 .equ Adr_NX500_uart2_uartrxiflsel, 0x00100AB8
 .equ Adr_NX500_uart0_uarttxiflsel, 0x00100A3C
 .equ Adr_NX500_uart1_uarttxiflsel, 0x00100A7C
 .equ Adr_NX500_uart2_uarttxiflsel, 0x00100ABC

@ =====================================================================
@
@ Area miimu
 .equ Addr_NX500_miimu, 0x00100B00
@
@ =====================================================================
 .equ Adr_NX500_miimu_miimu_reg, 0x00100B00
 .equ Adr_NX500_miimu_reg, 0x00100B00
 .equ Adr_NX500_miimu_miimu_sw_en, 0x00100B04
 .equ Adr_NX500_miimu_sw_en, 0x00100B04
 .equ Adr_NX500_miimu_miimu_sw_mdc, 0x00100B08
 .equ Adr_NX500_miimu_sw_mdc, 0x00100B08
 .equ Adr_NX500_miimu_miimu_sw_mdo, 0x00100B0C
 .equ Adr_NX500_miimu_sw_mdo, 0x00100B0C
 .equ Adr_NX500_miimu_miimu_sw_mdoe, 0x00100B10
 .equ Adr_NX500_miimu_sw_mdoe, 0x00100B10
 .equ Adr_NX500_miimu_miimu_sw_mdi, 0x00100B14
 .equ Adr_NX500_miimu_sw_mdi, 0x00100B14

@ =====================================================================
@
@ Area spi
 .equ Addr_NX500_spi, 0x00100C00
@
@ =====================================================================
 .equ Adr_NX500_spi_spi_data_register, 0x00100C00
 .equ Adr_NX500_spi_data_register, 0x00100C00
 .equ Adr_NX500_spi_spi_status_register, 0x00100C04
 .equ Adr_NX500_spi_status_register, 0x00100C04
 .equ Adr_NX500_spi_spi_control_register, 0x00100C08
 .equ Adr_NX500_spi_control_register, 0x00100C08
 .equ Adr_NX500_spi_spi_interrupt_control_register, 0x00100C0C
 .equ Adr_NX500_spi_interrupt_control_register, 0x00100C0C

@ =====================================================================
@
@ Area i2c
 .equ Addr_NX500_i2c, 0x00100D00
@
@ =====================================================================
 .equ Adr_NX500_i2c_i2c_ctrl, 0x00100D00
 .equ Adr_NX500_i2c_ctrl, 0x00100D00
 .equ Adr_NX500_i2c_i2c_data, 0x00100D04
 .equ Adr_NX500_i2c_data, 0x00100D04

@ =====================================================================
@
@ Area crc
 .equ Addr_NX500_crc, 0x00101000
@
@ =====================================================================
 .equ Adr_NX500_crc_crc_crc, 0x00101000
 .equ Adr_NX500_crc_crc, 0x00101000
 .equ Adr_NX500_crc_crc_data_in, 0x00101004
 .equ Adr_NX500_crc_data_in, 0x00101004
 .equ Adr_NX500_crc_crc_polynomial, 0x00101008
 .equ Adr_NX500_crc_polynomial, 0x00101008
 .equ Adr_NX500_crc_crc_config, 0x0010100C
 .equ Adr_NX500_crc_config, 0x0010100C

@ =====================================================================
@
@ Area systime
 .equ Addr_NX500_systime, 0x00101100
@
@ =====================================================================
 .equ Adr_NX500_systime_systime_ns, 0x00101100
 .equ Adr_NX500_systime_ns, 0x00101100
 .equ Adr_NX500_systime_systime_s, 0x00101104
 .equ Adr_NX500_systime_s, 0x00101104
 .equ Adr_NX500_systime_systime_border, 0x00101108
 .equ Adr_NX500_systime_border, 0x00101108
 .equ Adr_NX500_systime_systime_count_value, 0x0010110C
 .equ Adr_NX500_systime_count_value, 0x0010110C
 .equ Adr_NX500_systime_systime_s_compare_value, 0x00101110
 .equ Adr_NX500_systime_s_compare_value, 0x00101110
 .equ Adr_NX500_systime_systime_s_compare_enable, 0x00101114
 .equ Adr_NX500_systime_s_compare_enable, 0x00101114
 .equ Adr_NX500_systime_systime_s_compare_irq, 0x00101118
 .equ Adr_NX500_systime_s_compare_irq, 0x00101118

@ =====================================================================
@
@ Area rtc
 .equ Addr_NX500_rtc, 0x00101200
@
@ =====================================================================
 .equ Adr_NX500_rtc_rtc_val, 0x00101200
 .equ Adr_NX500_rtc_val, 0x00101200
 .equ Adr_NX500_rtc_rtc_32k, 0x00101204
 .equ Adr_NX500_rtc_32k, 0x00101204
 .equ Adr_NX500_rtc_rtc_32ks, 0x00101208
 .equ Adr_NX500_rtc_32ks, 0x00101208
 .equ Adr_NX500_rtc_rtc_irq2isolate_cycles, 0x0010120C
 .equ Adr_NX500_rtc_irq2isolate_cycles, 0x0010120C
 .equ Adr_NX500_rtc_rtc_irq_mask, 0x00101210
 .equ Adr_NX500_rtc_irq_mask, 0x00101210
 .equ Adr_NX500_rtc_rtc_irq_stat, 0x00101214
 .equ Adr_NX500_rtc_irq_stat, 0x00101214
 .equ Adr_NX500_rtc_rtc_isolated, 0x00101218
 .equ Adr_NX500_rtc_isolated, 0x00101218

@ =====================================================================
@
@ Area hif
 .equ Addr_NX500_hif, 0x00103000
@
@ =====================================================================
@ =====================================================================
@
@ Area host_controlled_dma_register_block
 .equ Addr_NX500_host_controlled_dma_register_block, 0x00103000
@
@ =====================================================================
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch0_host_start, 0x00103000
 .equ Adr_NX500_dpmhs_ch0_host_start, 0x00103000
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch0_netx_start, 0x00103004
 .equ Adr_NX500_dpmhs_ch0_netx_start, 0x00103004
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch0_dma_ctrl, 0x00103008
 .equ Adr_NX500_dpmhs_ch0_dma_ctrl, 0x00103008
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch0_host_mbx_base, 0x0010300C
 .equ Adr_NX500_dpmhs_ch0_host_mbx_base, 0x0010300C
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch0_buf_ctrl, 0x00103010
 .equ Adr_NX500_dpmhs_ch0_buf_ctrl, 0x00103010
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch0_host_buf3_base, 0x00103014
 .equ Adr_NX500_dpmhs_ch0_host_buf3_base, 0x00103014
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch0_host_buf2_base, 0x00103018
 .equ Adr_NX500_dpmhs_ch0_host_buf2_base, 0x00103018
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch0_host_buf1_base, 0x0010301C
 .equ Adr_NX500_dpmhs_ch0_host_buf1_base, 0x0010301C
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch1_host_start, 0x00103020
 .equ Adr_NX500_dpmhs_ch1_host_start, 0x00103020
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch1_netx_start, 0x00103024
 .equ Adr_NX500_dpmhs_ch1_netx_start, 0x00103024
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch1_dma_ctrl, 0x00103028
 .equ Adr_NX500_dpmhs_ch1_dma_ctrl, 0x00103028
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch1_host_mbx_base, 0x0010302C
 .equ Adr_NX500_dpmhs_ch1_host_mbx_base, 0x0010302C
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch1_buf_ctrl, 0x00103030
 .equ Adr_NX500_dpmhs_ch1_buf_ctrl, 0x00103030
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch1_host_buf3_base, 0x00103034
 .equ Adr_NX500_dpmhs_ch1_host_buf3_base, 0x00103034
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch1_host_buf2_base, 0x00103038
 .equ Adr_NX500_dpmhs_ch1_host_buf2_base, 0x00103038
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch1_host_buf1_base, 0x0010303C
 .equ Adr_NX500_dpmhs_ch1_host_buf1_base, 0x0010303C
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch2_host_start, 0x00103040
 .equ Adr_NX500_dpmhs_ch2_host_start, 0x00103040
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch2_netx_start, 0x00103044
 .equ Adr_NX500_dpmhs_ch2_netx_start, 0x00103044
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch2_dma_ctrl, 0x00103048
 .equ Adr_NX500_dpmhs_ch2_dma_ctrl, 0x00103048
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch2_host_mbx_base, 0x0010304C
 .equ Adr_NX500_dpmhs_ch2_host_mbx_base, 0x0010304C
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch2_buf_ctrl, 0x00103050
 .equ Adr_NX500_dpmhs_ch2_buf_ctrl, 0x00103050
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch2_host_buf3_base, 0x00103054
 .equ Adr_NX500_dpmhs_ch2_host_buf3_base, 0x00103054
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch2_host_buf2_base, 0x00103058
 .equ Adr_NX500_dpmhs_ch2_host_buf2_base, 0x00103058
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch2_host_buf1_base, 0x0010305C
 .equ Adr_NX500_dpmhs_ch2_host_buf1_base, 0x0010305C
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch3_host_start, 0x00103060
 .equ Adr_NX500_dpmhs_ch3_host_start, 0x00103060
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch3_netx_start, 0x00103064
 .equ Adr_NX500_dpmhs_ch3_netx_start, 0x00103064
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch3_dma_ctrl, 0x00103068
 .equ Adr_NX500_dpmhs_ch3_dma_ctrl, 0x00103068
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch3_host_mbx_base, 0x0010306C
 .equ Adr_NX500_dpmhs_ch3_host_mbx_base, 0x0010306C
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch3_buf_ctrl, 0x00103070
 .equ Adr_NX500_dpmhs_ch3_buf_ctrl, 0x00103070
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch3_host_buf3_base, 0x00103074
 .equ Adr_NX500_dpmhs_ch3_host_buf3_base, 0x00103074
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch3_host_buf2_base, 0x00103078
 .equ Adr_NX500_dpmhs_ch3_host_buf2_base, 0x00103078
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch3_host_buf1_base, 0x0010307C
 .equ Adr_NX500_dpmhs_ch3_host_buf1_base, 0x0010307C
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch4_host_start, 0x00103080
 .equ Adr_NX500_dpmhs_ch4_host_start, 0x00103080
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch4_netx_start, 0x00103084
 .equ Adr_NX500_dpmhs_ch4_netx_start, 0x00103084
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch4_dma_ctrl, 0x00103088
 .equ Adr_NX500_dpmhs_ch4_dma_ctrl, 0x00103088
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch4_host_mbx_base, 0x0010308C
 .equ Adr_NX500_dpmhs_ch4_host_mbx_base, 0x0010308C
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch4_buf_ctrl, 0x00103090
 .equ Adr_NX500_dpmhs_ch4_buf_ctrl, 0x00103090
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch4_host_buf3_base, 0x00103094
 .equ Adr_NX500_dpmhs_ch4_host_buf3_base, 0x00103094
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch4_host_buf2_base, 0x00103098
 .equ Adr_NX500_dpmhs_ch4_host_buf2_base, 0x00103098
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch4_host_buf1_base, 0x0010309C
 .equ Adr_NX500_dpmhs_ch4_host_buf1_base, 0x0010309C
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch5_host_start, 0x001030A0
 .equ Adr_NX500_dpmhs_ch5_host_start, 0x001030A0
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch5_netx_start, 0x001030A4
 .equ Adr_NX500_dpmhs_ch5_netx_start, 0x001030A4
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch5_dma_ctrl, 0x001030A8
 .equ Adr_NX500_dpmhs_ch5_dma_ctrl, 0x001030A8
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch5_host_mbx_base, 0x001030AC
 .equ Adr_NX500_dpmhs_ch5_host_mbx_base, 0x001030AC
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch5_buf_ctrl, 0x001030B0
 .equ Adr_NX500_dpmhs_ch5_buf_ctrl, 0x001030B0
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch5_host_buf3_base, 0x001030B4
 .equ Adr_NX500_dpmhs_ch5_host_buf3_base, 0x001030B4
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch5_host_buf2_base, 0x001030B8
 .equ Adr_NX500_dpmhs_ch5_host_buf2_base, 0x001030B8
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch5_host_buf1_base, 0x001030BC
 .equ Adr_NX500_dpmhs_ch5_host_buf1_base, 0x001030BC
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch6_host_start, 0x001030C0
 .equ Adr_NX500_dpmhs_ch6_host_start, 0x001030C0
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch6_netx_start, 0x001030C4
 .equ Adr_NX500_dpmhs_ch6_netx_start, 0x001030C4
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch6_dma_ctrl, 0x001030C8
 .equ Adr_NX500_dpmhs_ch6_dma_ctrl, 0x001030C8
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch6_host_mbx_base, 0x001030CC
 .equ Adr_NX500_dpmhs_ch6_host_mbx_base, 0x001030CC
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch6_buf_ctrl, 0x001030D0
 .equ Adr_NX500_dpmhs_ch6_buf_ctrl, 0x001030D0
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch6_host_buf3_base, 0x001030D4
 .equ Adr_NX500_dpmhs_ch6_host_buf3_base, 0x001030D4
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch6_host_buf2_base, 0x001030D8
 .equ Adr_NX500_dpmhs_ch6_host_buf2_base, 0x001030D8
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch6_host_buf1_base, 0x001030DC
 .equ Adr_NX500_dpmhs_ch6_host_buf1_base, 0x001030DC
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch7_host_start, 0x001030E0
 .equ Adr_NX500_dpmhs_ch7_host_start, 0x001030E0
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch7_netx_start, 0x001030E4
 .equ Adr_NX500_dpmhs_ch7_netx_start, 0x001030E4
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch7_dma_ctrl, 0x001030E8
 .equ Adr_NX500_dpmhs_ch7_dma_ctrl, 0x001030E8
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch7_host_mbx_base, 0x001030EC
 .equ Adr_NX500_dpmhs_ch7_host_mbx_base, 0x001030EC
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch7_buf_ctrl, 0x001030F0
 .equ Adr_NX500_dpmhs_ch7_buf_ctrl, 0x001030F0
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch7_host_buf3_base, 0x001030F4
 .equ Adr_NX500_dpmhs_ch7_host_buf3_base, 0x001030F4
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch7_host_buf2_base, 0x001030F8
 .equ Adr_NX500_dpmhs_ch7_host_buf2_base, 0x001030F8
 .equ Adr_NX500_host_controlled_dma_register_block_dpmhs_ch7_host_buf1_base, 0x001030FC
 .equ Adr_NX500_dpmhs_ch7_host_buf1_base, 0x001030FC

@ =====================================================================
@
@ Area host_controlled_global_register_block
 .equ Addr_NX500_host_controlled_global_register_block, 0x00103100
@
@ =====================================================================
 .equ Adr_NX500_host_controlled_global_register_block_pci_base, 0x001031BC
 .equ Adr_NX500_pci_base, 0x001031BC
 .equ Adr_NX500_host_controlled_global_register_block_wdg_host, 0x001031C0
 .equ Adr_NX500_wdg_host, 0x001031C0
 .equ Adr_NX500_host_controlled_global_register_block_dpmhs_wdg_host_trig, 0x001031C4
 .equ Adr_NX500_dpmhs_wdg_host_trig, 0x001031C4
 .equ Adr_NX500_host_controlled_global_register_block_dpmhs_wdg_arm_timeout, 0x001031C8
 .equ Adr_NX500_dpmhs_wdg_arm_timeout, 0x001031C8
 .equ Adr_NX500_host_controlled_global_register_block_cyclic_tmr_control, 0x001031D0
 .equ Adr_NX500_cyclic_tmr_control, 0x001031D0
 .equ Adr_NX500_host_controlled_global_register_block_cyclic_tmr_reload, 0x001031D4
 .equ Adr_NX500_cyclic_tmr_reload, 0x001031D4
 .equ Adr_NX500_host_controlled_global_register_block_sta_host, 0x001031D8
 .equ Adr_NX500_sta_host, 0x001031D8
 .equ Adr_NX500_host_controlled_global_register_block_res_reg, 0x001031DC
 .equ Adr_NX500_res_reg, 0x001031DC
 .equ Adr_NX500_host_controlled_global_register_block_irq_status_0_host, 0x001031E0
 .equ Adr_NX500_irq_status_0_host, 0x001031E0
 .equ Adr_NX500_host_controlled_global_register_block_irq_status_1_host, 0x001031E4
 .equ Adr_NX500_irq_status_1_host, 0x001031E4
 .equ Adr_NX500_host_controlled_global_register_block_irq_status_2_host, 0x001031E8
 .equ Adr_NX500_irq_status_2_host, 0x001031E8
 .equ Adr_NX500_host_controlled_global_register_block_dpmhs_int_sta3, 0x001031EC
 .equ Adr_NX500_dpmhs_int_sta3, 0x001031EC
 .equ Adr_NX500_host_controlled_global_register_block_irq_mask_0_host, 0x001031F0
 .equ Adr_NX500_irq_mask_0_host, 0x001031F0
 .equ Adr_NX500_host_controlled_global_register_block_irq_mask_1_host, 0x001031F4
 .equ Adr_NX500_irq_mask_1_host, 0x001031F4
 .equ Adr_NX500_host_controlled_global_register_block_irq_mask_2_host, 0x001031F8
 .equ Adr_NX500_irq_mask_2_host, 0x001031F8
 .equ Adr_NX500_host_controlled_global_register_block_dpmhs_int_en3, 0x001031FC
 .equ Adr_NX500_dpmhs_int_en3, 0x001031FC

@ =====================================================================
@
@ Area host_controlled_handshake_register_block
 .equ Addr_NX500_host_controlled_handshake_register_block, 0x00103200
@
@ =====================================================================
 .equ Adr_NX500_host_controlled_handshake_register_block_dpmhs_hs_data0, 0x00103200
 .equ Adr_NX500_dpmhs_hs_data0, 0x00103200
 .equ Adr_NX500_host_controlled_handshake_register_block_dpmhs_hs_data1, 0x00103204
 .equ Adr_NX500_dpmhs_hs_data1, 0x00103204
 .equ Adr_NX500_host_controlled_handshake_register_block_dpmhs_hs_data2, 0x00103208
 .equ Adr_NX500_dpmhs_hs_data2, 0x00103208
 .equ Adr_NX500_host_controlled_handshake_register_block_dpmhs_hs_data3, 0x0010320C
 .equ Adr_NX500_dpmhs_hs_data3, 0x0010320C
 .equ Adr_NX500_host_controlled_handshake_register_block_dpmhs_hs_data4, 0x00103210
 .equ Adr_NX500_dpmhs_hs_data4, 0x00103210
 .equ Adr_NX500_host_controlled_handshake_register_block_dpmhs_hs_data5, 0x00103214
 .equ Adr_NX500_dpmhs_hs_data5, 0x00103214
 .equ Adr_NX500_host_controlled_handshake_register_block_dpmhs_hs_data6, 0x00103218
 .equ Adr_NX500_dpmhs_hs_data6, 0x00103218
 .equ Adr_NX500_host_controlled_handshake_register_block_dpmhs_hs_data7, 0x0010321C
 .equ Adr_NX500_dpmhs_hs_data7, 0x0010321C
 .equ Adr_NX500_host_controlled_handshake_register_block_dpmhs_hs_data8, 0x00103220
 .equ Adr_NX500_dpmhs_hs_data8, 0x00103220
 .equ Adr_NX500_host_controlled_handshake_register_block_dpmhs_hs_data9, 0x00103224
 .equ Adr_NX500_dpmhs_hs_data9, 0x00103224
 .equ Adr_NX500_host_controlled_handshake_register_block_dpmhs_hs_data10, 0x00103228
 .equ Adr_NX500_dpmhs_hs_data10, 0x00103228
 .equ Adr_NX500_host_controlled_handshake_register_block_dpmhs_hs_data11, 0x0010322C
 .equ Adr_NX500_dpmhs_hs_data11, 0x0010322C
 .equ Adr_NX500_host_controlled_handshake_register_block_dpmhs_hs_data12, 0x00103230
 .equ Adr_NX500_dpmhs_hs_data12, 0x00103230
 .equ Adr_NX500_host_controlled_handshake_register_block_dpmhs_hs_data13, 0x00103234
 .equ Adr_NX500_dpmhs_hs_data13, 0x00103234
 .equ Adr_NX500_host_controlled_handshake_register_block_dpmhs_hs_data14, 0x00103238
 .equ Adr_NX500_dpmhs_hs_data14, 0x00103238
 .equ Adr_NX500_host_controlled_handshake_register_block_dpmhs_hs_data15, 0x0010323C
 .equ Adr_NX500_dpmhs_hs_data15, 0x0010323C

@ =====================================================================
@
@ Area netx_controlled_dma_register_block
 .equ Addr_NX500_netx_controlled_dma_register_block, 0x00103300
@
@ =====================================================================
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch0_host_start, 0x00103300
 .equ Adr_NX500_dpmas_ch0_host_start, 0x00103300
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch0_netx_start, 0x00103304
 .equ Adr_NX500_dpmas_ch0_netx_start, 0x00103304
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch0_dma_ctrl, 0x00103308
 .equ Adr_NX500_dpmas_ch0_dma_ctrl, 0x00103308
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch0_mbx_base, 0x0010330C
 .equ Adr_NX500_dpmas_ch0_mbx_base, 0x0010330C
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch0_buf_ctrl, 0x00103310
 .equ Adr_NX500_dpmas_ch0_buf_ctrl, 0x00103310
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch0_netx_buf_base3, 0x00103314
 .equ Adr_NX500_dpmas_ch0_netx_buf_base3, 0x00103314
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch0_netx_buf_base2, 0x00103318
 .equ Adr_NX500_dpmas_ch0_netx_buf_base2, 0x00103318
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch0_netx_buf_base1, 0x0010331C
 .equ Adr_NX500_dpmas_ch0_netx_buf_base1, 0x0010331C
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch1_host_start, 0x00103320
 .equ Adr_NX500_dpmas_ch1_host_start, 0x00103320
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch1_netx_start, 0x00103324
 .equ Adr_NX500_dpmas_ch1_netx_start, 0x00103324
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch1_dma_ctrl, 0x00103328
 .equ Adr_NX500_dpmas_ch1_dma_ctrl, 0x00103328
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch1_mbx_base, 0x0010332C
 .equ Adr_NX500_dpmas_ch1_mbx_base, 0x0010332C
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch1_buf_ctrl, 0x00103330
 .equ Adr_NX500_dpmas_ch1_buf_ctrl, 0x00103330
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch1_netx_buf_base3, 0x00103334
 .equ Adr_NX500_dpmas_ch1_netx_buf_base3, 0x00103334
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch1_netx_buf_base2, 0x00103338
 .equ Adr_NX500_dpmas_ch1_netx_buf_base2, 0x00103338
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch1_netx_buf_base1, 0x0010333C
 .equ Adr_NX500_dpmas_ch1_netx_buf_base1, 0x0010333C
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch2_host_start, 0x00103340
 .equ Adr_NX500_dpmas_ch2_host_start, 0x00103340
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch2_netx_start, 0x00103344
 .equ Adr_NX500_dpmas_ch2_netx_start, 0x00103344
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch2_dma_ctrl, 0x00103348
 .equ Adr_NX500_dpmas_ch2_dma_ctrl, 0x00103348
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch2_mbx_base, 0x0010334C
 .equ Adr_NX500_dpmas_ch2_mbx_base, 0x0010334C
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch2_buf_ctrl, 0x00103350
 .equ Adr_NX500_dpmas_ch2_buf_ctrl, 0x00103350
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch2_netx_buf_base3, 0x00103354
 .equ Adr_NX500_dpmas_ch2_netx_buf_base3, 0x00103354
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch2_netx_buf_base2, 0x00103358
 .equ Adr_NX500_dpmas_ch2_netx_buf_base2, 0x00103358
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch2_netx_buf_base1, 0x0010335C
 .equ Adr_NX500_dpmas_ch2_netx_buf_base1, 0x0010335C
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch3_host_start, 0x00103360
 .equ Adr_NX500_dpmas_ch3_host_start, 0x00103360
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch3_netx_start, 0x00103364
 .equ Adr_NX500_dpmas_ch3_netx_start, 0x00103364
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch3_dma_ctrl, 0x00103368
 .equ Adr_NX500_dpmas_ch3_dma_ctrl, 0x00103368
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch3_mbx_base, 0x0010336C
 .equ Adr_NX500_dpmas_ch3_mbx_base, 0x0010336C
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch3_buf_ctrl, 0x00103370
 .equ Adr_NX500_dpmas_ch3_buf_ctrl, 0x00103370
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch3_netx_buf_base3, 0x00103374
 .equ Adr_NX500_dpmas_ch3_netx_buf_base3, 0x00103374
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch3_netx_buf_base2, 0x00103378
 .equ Adr_NX500_dpmas_ch3_netx_buf_base2, 0x00103378
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch3_netx_buf_base1, 0x0010337C
 .equ Adr_NX500_dpmas_ch3_netx_buf_base1, 0x0010337C
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch4_host_start, 0x00103380
 .equ Adr_NX500_dpmas_ch4_host_start, 0x00103380
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch4_netx_start, 0x00103384
 .equ Adr_NX500_dpmas_ch4_netx_start, 0x00103384
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch4_dma_ctrl, 0x00103388
 .equ Adr_NX500_dpmas_ch4_dma_ctrl, 0x00103388
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch4_mbx_base, 0x0010338C
 .equ Adr_NX500_dpmas_ch4_mbx_base, 0x0010338C
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch4_buf_ctrl, 0x00103390
 .equ Adr_NX500_dpmas_ch4_buf_ctrl, 0x00103390
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch4_netx_buf_base3, 0x00103394
 .equ Adr_NX500_dpmas_ch4_netx_buf_base3, 0x00103394
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch4_netx_buf_base2, 0x00103398
 .equ Adr_NX500_dpmas_ch4_netx_buf_base2, 0x00103398
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch4_netx_buf_base1, 0x0010339C
 .equ Adr_NX500_dpmas_ch4_netx_buf_base1, 0x0010339C
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch5_host_start, 0x001033A0
 .equ Adr_NX500_dpmas_ch5_host_start, 0x001033A0
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch5_netx_start, 0x001033A4
 .equ Adr_NX500_dpmas_ch5_netx_start, 0x001033A4
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch5_dma_ctrl, 0x001033A8
 .equ Adr_NX500_dpmas_ch5_dma_ctrl, 0x001033A8
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch5_mbx_base, 0x001033AC
 .equ Adr_NX500_dpmas_ch5_mbx_base, 0x001033AC
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch5_buf_ctrl, 0x001033B0
 .equ Adr_NX500_dpmas_ch5_buf_ctrl, 0x001033B0
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch5_netx_buf_base3, 0x001033B4
 .equ Adr_NX500_dpmas_ch5_netx_buf_base3, 0x001033B4
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch5_netx_buf_base2, 0x001033B8
 .equ Adr_NX500_dpmas_ch5_netx_buf_base2, 0x001033B8
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch5_netx_buf_base1, 0x001033BC
 .equ Adr_NX500_dpmas_ch5_netx_buf_base1, 0x001033BC
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch6_host_start, 0x001033C0
 .equ Adr_NX500_dpmas_ch6_host_start, 0x001033C0
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch6_netx_start, 0x001033C4
 .equ Adr_NX500_dpmas_ch6_netx_start, 0x001033C4
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch6_dma_ctrl, 0x001033C8
 .equ Adr_NX500_dpmas_ch6_dma_ctrl, 0x001033C8
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch6_mbx_base, 0x001033CC
 .equ Adr_NX500_dpmas_ch6_mbx_base, 0x001033CC
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch6_buf_ctrl, 0x001033D0
 .equ Adr_NX500_dpmas_ch6_buf_ctrl, 0x001033D0
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch6_netx_buf_base3, 0x001033D4
 .equ Adr_NX500_dpmas_ch6_netx_buf_base3, 0x001033D4
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch6_netx_buf_base2, 0x001033D8
 .equ Adr_NX500_dpmas_ch6_netx_buf_base2, 0x001033D8
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch6_netx_buf_base1, 0x001033DC
 .equ Adr_NX500_dpmas_ch6_netx_buf_base1, 0x001033DC
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch7_host_start, 0x001033E0
 .equ Adr_NX500_dpmas_ch7_host_start, 0x001033E0
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch7_netx_start, 0x001033E4
 .equ Adr_NX500_dpmas_ch7_netx_start, 0x001033E4
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch7_dma_ctrl, 0x001033E8
 .equ Adr_NX500_dpmas_ch7_dma_ctrl, 0x001033E8
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch7_mbx_base, 0x001033EC
 .equ Adr_NX500_dpmas_ch7_mbx_base, 0x001033EC
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch7_buf_ctrl, 0x001033F0
 .equ Adr_NX500_dpmas_ch7_buf_ctrl, 0x001033F0
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch7_netx_buf_base3, 0x001033F4
 .equ Adr_NX500_dpmas_ch7_netx_buf_base3, 0x001033F4
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch7_netx_buf_base2, 0x001033F8
 .equ Adr_NX500_dpmas_ch7_netx_buf_base2, 0x001033F8
 .equ Adr_NX500_netx_controlled_dma_register_block_dpmas_ch7_netx_buf_base1, 0x001033FC
 .equ Adr_NX500_dpmas_ch7_netx_buf_base1, 0x001033FC

@ =====================================================================
@
@ Area netx_controlled_global_register_block_1
 .equ Addr_NX500_netx_controlled_global_register_block_1, 0x00103400
@
@ =====================================================================
 .equ Adr_NX500_netx_controlled_global_register_block_1_pci_window_low0, 0x00103480
 .equ Adr_NX500_pci_window_low0, 0x00103480
 .equ Adr_NX500_netx_controlled_global_register_block_1_pci_window_high0, 0x00103484
 .equ Adr_NX500_pci_window_high0, 0x00103484
 .equ Adr_NX500_netx_controlled_global_register_block_1_pci_window_low1, 0x00103488
 .equ Adr_NX500_pci_window_low1, 0x00103488
 .equ Adr_NX500_netx_controlled_global_register_block_1_pci_window_high1, 0x0010348C
 .equ Adr_NX500_pci_window_high1, 0x0010348C
 .equ Adr_NX500_netx_controlled_global_register_block_1_pci_window_low2, 0x00103490
 .equ Adr_NX500_pci_window_low2, 0x00103490
 .equ Adr_NX500_netx_controlled_global_register_block_1_pci_window_high2, 0x00103494
 .equ Adr_NX500_pci_window_high2, 0x00103494
 .equ Adr_NX500_netx_controlled_global_register_block_1_pci_window_low3, 0x00103498
 .equ Adr_NX500_pci_window_low3, 0x00103498
 .equ Adr_NX500_netx_controlled_global_register_block_1_pci_window_high3, 0x0010349C
 .equ Adr_NX500_pci_window_high3, 0x0010349C
 .equ Adr_NX500_netx_controlled_global_register_block_1_pci_io_base, 0x001034A0
 .equ Adr_NX500_pci_io_base, 0x001034A0
 .equ Adr_NX500_netx_controlled_global_register_block_1_pci_rom_base, 0x001034A4
 .equ Adr_NX500_pci_rom_base, 0x001034A4
 .equ Adr_NX500_netx_controlled_global_register_block_1_arb_ctrl, 0x001034A8
 .equ Adr_NX500_arb_ctrl, 0x001034A8
 .equ Adr_NX500_netx_controlled_global_register_block_1_pci_config, 0x001034AC
 .equ Adr_NX500_pci_config, 0x001034AC
 .equ Adr_NX500_netx_controlled_global_register_block_1_cis_base, 0x001034BC
 .equ Adr_NX500_cis_base, 0x001034BC
 .equ Adr_NX500_netx_controlled_global_register_block_1_wdg_netx, 0x001034C0
 .equ Adr_NX500_wdg_netx, 0x001034C0
 .equ Adr_NX500_netx_controlled_global_register_block_1_dpmas_wdg_arm_timeout, 0x001034C8
 .equ Adr_NX500_dpmas_wdg_arm_timeout, 0x001034C8
 .equ Adr_NX500_netx_controlled_global_register_block_1_dpmas_wdg_arm_trig, 0x001034CC
 .equ Adr_NX500_dpmas_wdg_arm_trig, 0x001034CC
 .equ Adr_NX500_netx_controlled_global_register_block_1_sta_netx, 0x001034D8
 .equ Adr_NX500_sta_netx, 0x001034D8
 .equ Adr_NX500_netx_controlled_global_register_block_1_irq_status_0_netx, 0x001034E0
 .equ Adr_NX500_irq_status_0_netx, 0x001034E0
 .equ Adr_NX500_netx_controlled_global_register_block_1_irq_status_1_netx, 0x001034E4
 .equ Adr_NX500_irq_status_1_netx, 0x001034E4
 .equ Adr_NX500_netx_controlled_global_register_block_1_irq_status_2_netx, 0x001034E8
 .equ Adr_NX500_irq_status_2_netx, 0x001034E8
 .equ Adr_NX500_netx_controlled_global_register_block_1_irq_status_3_netx, 0x001034EC
 .equ Adr_NX500_irq_status_3_netx, 0x001034EC
 .equ Adr_NX500_netx_controlled_global_register_block_1_irq_mask_0_netx, 0x001034F0
 .equ Adr_NX500_irq_mask_0_netx, 0x001034F0
 .equ Adr_NX500_netx_controlled_global_register_block_1_irq_mask_1_netx, 0x001034F4
 .equ Adr_NX500_irq_mask_1_netx, 0x001034F4
 .equ Adr_NX500_netx_controlled_global_register_block_1_irq_mask_2_netx, 0x001034F8
 .equ Adr_NX500_irq_mask_2_netx, 0x001034F8
 .equ Adr_NX500_netx_controlled_global_register_block_1_irq_mask_3_netx, 0x001034FC
 .equ Adr_NX500_irq_mask_3_netx, 0x001034FC

@ =====================================================================
@
@ Area netx_controlled_handshake_register_block
 .equ Addr_NX500_netx_controlled_handshake_register_block, 0x00103500
@
@ =====================================================================
 .equ Adr_NX500_netx_controlled_handshake_register_block_handshake_ch0, 0x00103500
 .equ Adr_NX500_handshake_ch0, 0x00103500
 .equ Adr_NX500_netx_controlled_handshake_register_block_handshake_ch1, 0x00103504
 .equ Adr_NX500_handshake_ch1, 0x00103504
 .equ Adr_NX500_netx_controlled_handshake_register_block_handshake_ch2, 0x00103508
 .equ Adr_NX500_handshake_ch2, 0x00103508
 .equ Adr_NX500_netx_controlled_handshake_register_block_handshake_ch3, 0x0010350C
 .equ Adr_NX500_handshake_ch3, 0x0010350C
 .equ Adr_NX500_netx_controlled_handshake_register_block_handshake_ch4, 0x00103510
 .equ Adr_NX500_handshake_ch4, 0x00103510
 .equ Adr_NX500_netx_controlled_handshake_register_block_handshake_ch5, 0x00103514
 .equ Adr_NX500_handshake_ch5, 0x00103514
 .equ Adr_NX500_netx_controlled_handshake_register_block_handshake_ch6, 0x00103518
 .equ Adr_NX500_handshake_ch6, 0x00103518
 .equ Adr_NX500_netx_controlled_handshake_register_block_handshake_ch7, 0x0010351C
 .equ Adr_NX500_handshake_ch7, 0x0010351C
 .equ Adr_NX500_netx_controlled_handshake_register_block_handshake_ch8, 0x00103520
 .equ Adr_NX500_handshake_ch8, 0x00103520
 .equ Adr_NX500_netx_controlled_handshake_register_block_handshake_ch9, 0x00103524
 .equ Adr_NX500_handshake_ch9, 0x00103524
 .equ Adr_NX500_netx_controlled_handshake_register_block_handshake_ch10, 0x00103528
 .equ Adr_NX500_handshake_ch10, 0x00103528
 .equ Adr_NX500_netx_controlled_handshake_register_block_handshake_ch11, 0x0010352C
 .equ Adr_NX500_handshake_ch11, 0x0010352C
 .equ Adr_NX500_netx_controlled_handshake_register_block_handshake_ch12, 0x00103530
 .equ Adr_NX500_handshake_ch12, 0x00103530
 .equ Adr_NX500_netx_controlled_handshake_register_block_handshake_ch13, 0x00103534
 .equ Adr_NX500_handshake_ch13, 0x00103534
 .equ Adr_NX500_netx_controlled_handshake_register_block_handshake_ch14, 0x00103538
 .equ Adr_NX500_handshake_ch14, 0x00103538
 .equ Adr_NX500_netx_controlled_handshake_register_block_handshake_ch15, 0x0010353C
 .equ Adr_NX500_handshake_ch15, 0x0010353C

@ =====================================================================
@
@ Area netx_controlled_global_register_block_2
 .equ Addr_NX500_netx_controlled_global_register_block_2, 0x00103600
@
@ =====================================================================
 .equ Adr_NX500_netx_controlled_global_register_block_2_clk_reg, 0x00103604
 .equ Adr_NX500_clk_reg, 0x00103604
 .equ Adr_NX500_netx_controlled_global_register_block_2_if_conf1, 0x00103608
 .equ Adr_NX500_if_conf1, 0x00103608
 .equ Adr_NX500_netx_controlled_global_register_block_2_if_conf2, 0x0010360C
 .equ Adr_NX500_if_conf2, 0x0010360C
 .equ Adr_NX500_netx_controlled_global_register_block_2_exp_bus_reg, 0x00103610
 .equ Adr_NX500_exp_bus_reg, 0x00103610
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_ext_config1, 0x00103614
 .equ Adr_NX500_dpmas_ext_config1, 0x00103614
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_ext_config2, 0x00103618
 .equ Adr_NX500_dpmas_ext_config2, 0x00103618
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_ext_config3, 0x0010361C
 .equ Adr_NX500_dpmas_ext_config3, 0x0010361C
 .equ Adr_NX500_netx_controlled_global_register_block_2_io_reg_mode0, 0x00103620
 .equ Adr_NX500_io_reg_mode0, 0x00103620
 .equ Adr_NX500_netx_controlled_global_register_block_2_io_reg_drv_en0, 0x00103624
 .equ Adr_NX500_io_reg_drv_en0, 0x00103624
 .equ Adr_NX500_netx_controlled_global_register_block_2_io_reg_data0, 0x00103628
 .equ Adr_NX500_io_reg_data0, 0x00103628
 .equ Adr_NX500_netx_controlled_global_register_block_2_io_reg_reserved0, 0x0010362C
 .equ Adr_NX500_io_reg_reserved0, 0x0010362C
 .equ Adr_NX500_netx_controlled_global_register_block_2_io_reg_mode1, 0x00103630
 .equ Adr_NX500_io_reg_mode1, 0x00103630
 .equ Adr_NX500_netx_controlled_global_register_block_2_io_reg_drv_en1, 0x00103634
 .equ Adr_NX500_io_reg_drv_en1, 0x00103634
 .equ Adr_NX500_netx_controlled_global_register_block_2_io_reg_data1, 0x00103638
 .equ Adr_NX500_io_reg_data1, 0x00103638
 .equ Adr_NX500_netx_controlled_global_register_block_2_io_reg_reserved1, 0x0010363C
 .equ Adr_NX500_io_reg_reserved1, 0x0010363C
 .equ Adr_NX500_netx_controlled_global_register_block_2_mb_ctrl, 0x00103640
 .equ Adr_NX500_mb_ctrl, 0x00103640
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_db_mapping0, 0x00103644
 .equ Adr_NX500_dpmas_db_mapping0, 0x00103644
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_db_end1, 0x00103648
 .equ Adr_NX500_dpmas_db_end1, 0x00103648
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_db_mapping1, 0x0010364C
 .equ Adr_NX500_dpmas_db_mapping1, 0x0010364C
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_db_end2, 0x00103650
 .equ Adr_NX500_dpmas_db_end2, 0x00103650
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_db_mapping2, 0x00103654
 .equ Adr_NX500_dpmas_db_mapping2, 0x00103654
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_db_end3, 0x00103658
 .equ Adr_NX500_dpmas_db_end3, 0x00103658
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_db_mapping3, 0x0010365C
 .equ Adr_NX500_dpmas_db_mapping3, 0x0010365C
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_db_end4, 0x00103660
 .equ Adr_NX500_dpmas_db_end4, 0x00103660
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_db_mapping4, 0x00103664
 .equ Adr_NX500_dpmas_db_mapping4, 0x00103664
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_db_end5, 0x00103668
 .equ Adr_NX500_dpmas_db_end5, 0x00103668
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_db_mapping5, 0x0010366C
 .equ Adr_NX500_dpmas_db_mapping5, 0x0010366C
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_db_end6, 0x00103670
 .equ Adr_NX500_dpmas_db_end6, 0x00103670
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_db_mapping6, 0x00103674
 .equ Adr_NX500_dpmas_db_mapping6, 0x00103674
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_db_end7, 0x00103678
 .equ Adr_NX500_dpmas_db_end7, 0x00103678
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_db_mapping7, 0x0010367C
 .equ Adr_NX500_dpmas_db_mapping7, 0x0010367C
 .equ Adr_NX500_netx_controlled_global_register_block_2_hs_ctrl, 0x00103680
 .equ Adr_NX500_hs_ctrl, 0x00103680
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_hscr1, 0x00103684
 .equ Adr_NX500_dpmas_hscr1, 0x00103684
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_hscr2, 0x00103688
 .equ Adr_NX500_dpmas_hscr2, 0x00103688
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_hscr3, 0x0010368C
 .equ Adr_NX500_dpmas_hscr3, 0x0010368C
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_hscr4, 0x00103690
 .equ Adr_NX500_dpmas_hscr4, 0x00103690
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_hscr5, 0x00103694
 .equ Adr_NX500_dpmas_hscr5, 0x00103694
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_hscr6, 0x00103698
 .equ Adr_NX500_dpmas_hscr6, 0x00103698
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_hscr7, 0x0010369C
 .equ Adr_NX500_dpmas_hscr7, 0x0010369C
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_hscr8, 0x001036A0
 .equ Adr_NX500_dpmas_hscr8, 0x001036A0
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_hscr9, 0x001036A4
 .equ Adr_NX500_dpmas_hscr9, 0x001036A4
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_hscr10, 0x001036A8
 .equ Adr_NX500_dpmas_hscr10, 0x001036A8
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_hscr11, 0x001036AC
 .equ Adr_NX500_dpmas_hscr11, 0x001036AC
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_hscr12, 0x001036B0
 .equ Adr_NX500_dpmas_hscr12, 0x001036B0
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_hscr13, 0x001036B4
 .equ Adr_NX500_dpmas_hscr13, 0x001036B4
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_hscr14, 0x001036B8
 .equ Adr_NX500_dpmas_hscr14, 0x001036B8
 .equ Adr_NX500_netx_controlled_global_register_block_2_dpmas_hscr15, 0x001036BC
 .equ Adr_NX500_dpmas_hscr15, 0x001036BC

@ =====================================================================
@
@ Area netx_controlled_pci_configuration_shadow_register_block
 .equ Addr_NX500_netx_controlled_pci_configuration_shadow_register_block, 0x00103700
@
@ =====================================================================
 .equ Adr_NX500_netx_controlled_pci_configuration_shadow_register_block_pci_regs_addr, 0x00103700
 .equ Adr_NX500_pci_regs_addr, 0x00103700
 .equ Adr_NX500_netx_controlled_pci_configuration_shadow_register_block_dpmas_pci_conf_rd_data, 0x00103704
 .equ Adr_NX500_dpmas_pci_conf_rd_data, 0x00103704
 .equ Adr_NX500_netx_controlled_pci_configuration_shadow_register_block_dpmas_pci_conf_wr_ctrl, 0x00103708
 .equ Adr_NX500_dpmas_pci_conf_wr_ctrl, 0x00103708
 .equ Adr_NX500_netx_controlled_pci_configuration_shadow_register_block_dpmas_pci_conf_wr_data, 0x0010370C
 .equ Adr_NX500_dpmas_pci_conf_wr_data, 0x0010370C

@ =====================================================================
@
@ Area lcd_ctrl
 .equ Addr_NX500_lcd_ctrl, 0x00104000
@
@ =====================================================================
 .equ Adr_NX500_lcd_ctrl_LCDTiming0, 0x00104000
 .equ Adr_NX500_LCDTiming0, 0x00104000
 .equ Adr_NX500_lcd_ctrl_LCDTiming1, 0x00104004
 .equ Adr_NX500_LCDTiming1, 0x00104004
 .equ Adr_NX500_lcd_ctrl_LCDTiming2, 0x00104008
 .equ Adr_NX500_LCDTiming2, 0x00104008
 .equ Adr_NX500_lcd_ctrl_LCDTiming3, 0x0010400C
 .equ Adr_NX500_LCDTiming3, 0x0010400C
 .equ Adr_NX500_lcd_ctrl_LCDUPBASE, 0x00104010
 .equ Adr_NX500_LCDUPBASE, 0x00104010
 .equ Adr_NX500_lcd_ctrl_LCDLPBASE, 0x00104014
 .equ Adr_NX500_LCDLPBASE, 0x00104014
 .equ Adr_NX500_lcd_ctrl_LCDIMSC, 0x00104018
 .equ Adr_NX500_LCDIMSC, 0x00104018
 .equ Adr_NX500_lcd_ctrl_LCDControl, 0x0010401C
 .equ Adr_NX500_LCDControl, 0x0010401C
 .equ Adr_NX500_lcd_ctrl_LCDRIS, 0x00104020
 .equ Adr_NX500_LCDRIS, 0x00104020
 .equ Adr_NX500_lcd_ctrl_LCDMIS, 0x00104024
 .equ Adr_NX500_LCDMIS, 0x00104024
 .equ Adr_NX500_lcd_ctrl_LCDICR, 0x00104028
 .equ Adr_NX500_LCDICR, 0x00104028
 .equ Adr_NX500_lcd_ctrl_LCDUPCURR, 0x0010402C
 .equ Adr_NX500_LCDUPCURR, 0x0010402C
 .equ Adr_NX500_lcd_ctrl_LCDLPCURR, 0x00104030
 .equ Adr_NX500_LCDLPCURR, 0x00104030
 .equ Adr_NX500_lcd_ctrl_LCDPaletteStart, 0x00104200
 .equ Adr_NX500_LCDPaletteStart, 0x00104200
 .equ Adr_NX500_lcd_ctrl_LCDPaletteEnd, 0x001043FC
 .equ Adr_NX500_LCDPaletteEnd, 0x001043FC
 .equ Adr_NX500_lcd_ctrl_DMAFIFOStart, 0x00104400
 .equ Adr_NX500_DMAFIFOStart, 0x00104400
 .equ Adr_NX500_lcd_ctrl_DMAFIFOEnd, 0x001047FC
 .equ Adr_NX500_DMAFIFOEnd, 0x001047FC
 .equ Adr_NX500_lcd_ctrl_CLCDPERIPHID0, 0x00104FE0
 .equ Adr_NX500_CLCDPERIPHID0, 0x00104FE0
 .equ Adr_NX500_lcd_ctrl_CLCDPERIPHID1, 0x00104FE4
 .equ Adr_NX500_CLCDPERIPHID1, 0x00104FE4
 .equ Adr_NX500_lcd_ctrl_CLCDPERIPHID2, 0x00104FE8
 .equ Adr_NX500_CLCDPERIPHID2, 0x00104FE8
 .equ Adr_NX500_lcd_ctrl_CLCDPERIPHID3, 0x00104FEC
 .equ Adr_NX500_CLCDPERIPHID3, 0x00104FEC
 .equ Adr_NX500_lcd_ctrl_CLCDPCELLID0, 0x00104FF0
 .equ Adr_NX500_CLCDPCELLID0, 0x00104FF0
 .equ Adr_NX500_lcd_ctrl_CLCDPCELLID1, 0x00104FF4
 .equ Adr_NX500_CLCDPCELLID1, 0x00104FF4
 .equ Adr_NX500_lcd_ctrl_CLCDPCELLID2, 0x00104FF8
 .equ Adr_NX500_CLCDPCELLID2, 0x00104FF8
 .equ Adr_NX500_lcd_ctrl_CLCDPCELLID3, 0x00104FFC
 .equ Adr_NX500_CLCDPCELLID3, 0x00104FFC

@ =====================================================================
@
@ Area usb_core
 .equ Addr_NX500_usb_core, 0x00120000
@
@ =====================================================================
 .equ Adr_NX500_usb_core_ID, 0x00120000
 .equ Adr_NX500_ID, 0x00120000
 .equ Adr_NX500_usb_core_USB_CTRL, 0x00120004
 .equ Adr_NX500_USB_CTRL, 0x00120004
 .equ Adr_NX500_usb_core_FRM_TIMER, 0x00120008
 .equ Adr_NX500_FRM_TIMER, 0x00120008
 .equ Adr_NX500_usb_core_MAIN_EV, 0x0012000C
 .equ Adr_NX500_MAIN_EV, 0x0012000C
 .equ Adr_NX500_usb_core_MAIN_EM, 0x00120010
 .equ Adr_NX500_MAIN_EM, 0x00120010
 .equ Adr_NX500_usb_core_PIPE_EV, 0x00120014
 .equ Adr_NX500_PIPE_EV, 0x00120014
 .equ Adr_NX500_usb_core_PIPE_EM, 0x00120018
 .equ Adr_NX500_PIPE_EM, 0x00120018
 .equ Adr_NX500_usb_core_PIPE_SEL, 0x00120024
 .equ Adr_NX500_PIPE_SEL, 0x00120024
 .equ Adr_NX500_usb_core_PORT_STAT, 0x0012002C
 .equ Adr_NX500_PORT_STAT, 0x0012002C
 .equ Adr_NX500_usb_core_PORT_CTRL, 0x00120030
 .equ Adr_NX500_PORT_CTRL, 0x00120030
 .equ Adr_NX500_usb_core_PSC_EV, 0x00120034
 .equ Adr_NX500_PSC_EV, 0x00120034
 .equ Adr_NX500_usb_core_PSC_EM, 0x00120038
 .equ Adr_NX500_PSC_EM, 0x00120038
 .equ Adr_NX500_usb_core_PIPE_CTRL, 0x00120040
 .equ Adr_NX500_PIPE_CTRL, 0x00120040
 .equ Adr_NX500_usb_core_PIPE_CFG, 0x00120044
 .equ Adr_NX500_PIPE_CFG, 0x00120044
 .equ Adr_NX500_usb_core_PIPE_ADDR, 0x00120048
 .equ Adr_NX500_PIPE_ADDR, 0x00120048
 .equ Adr_NX500_usb_core_PIPE_STAT, 0x0012004C
 .equ Adr_NX500_PIPE_STAT, 0x0012004C
 .equ Adr_NX500_usb_core_PIPE_DATA_PTR, 0x00120050
 .equ Adr_NX500_PIPE_DATA_PTR, 0x00120050
 .equ Adr_NX500_usb_core_PIPE_DATA_TBYTES, 0x00120054
 .equ Adr_NX500_PIPE_DATA_TBYTES, 0x00120054
 .equ Adr_NX500_usb_core_PIPE_ADATA_PTR, 0x00120058
 .equ Adr_NX500_PIPE_ADATA_PTR, 0x00120058
 .equ Adr_NX500_usb_core_PIPE_ADATA_TBYTES, 0x0012005C
 .equ Adr_NX500_PIPE_ADATA_TBYTES, 0x0012005C
 .equ Adr_NX500_usb_core_DEBUG_CTRL, 0x00120060
 .equ Adr_NX500_DEBUG_CTRL, 0x00120060
 .equ Adr_NX500_usb_core_DEBUG_PID, 0x00120064
 .equ Adr_NX500_DEBUG_PID, 0x00120064
 .equ Adr_NX500_usb_core_DEBUG_STAT, 0x00120068
 .equ Adr_NX500_DEBUG_STAT, 0x00120068
 .equ Adr_NX500_usb_core_TEST, 0x0012006C
 .equ Adr_NX500_TEST, 0x0012006C
 .equ Adr_NX500_usb_core_MAIN_CFG, 0x00120080
 .equ Adr_NX500_MAIN_CFG, 0x00120080
 .equ Adr_NX500_usb_core_MODE_CFG, 0x00120084
 .equ Adr_NX500_MODE_CFG, 0x00120084
 .equ Adr_NX500_usb_core_usb_core_ctrl, 0x00120088
 .equ Adr_NX500_usb_core_ctrl, 0x00120088

@ =====================================================================
@
@ Area usb_fifo
 .equ Addr_NX500_usb_fifo, 0x00130000
@
@ =====================================================================
 .equ Adr_NX500_usb_fifo_USB_FIFO_BASE, 0x00130000
 .equ Adr_NX500_USB_FIFO_BASE, 0x00130000

@ =====================================================================
@
@ Area xc
 .equ Addr_NX500_xc, 0x00140000
@
@ =====================================================================
 .equ Adr_NX500_xc_xc_base, 0x00140000
 .equ Adr_NX500_xc_base, 0x00140000

@ =====================================================================
@
@ Area xmac
 .equ Addr_NX500_xmac0, 0x00160000
 .equ Addr_NX500_xmac1, 0x00161000
 .equ Addr_NX500_xmac3, 0x00163000
 .equ Addr_NX500_xmac2, 0x00162000
@
@ =====================================================================
 .equ Adr_NX500_xmac0_xmac_rpu_program_start, 0x00160000
 .equ Adr_NX500_xmac1_xmac_rpu_program_start, 0x00161000
 .equ Adr_NX500_xmac3_xmac_rpu_program_start, 0x00163000
 .equ Adr_NX500_xmac2_xmac_rpu_program_start, 0x00162000
 .equ Adr_NX500_xmac0_xmac_rpu_program_end, 0x001603FF
 .equ Adr_NX500_xmac1_xmac_rpu_program_end, 0x001613FF
 .equ Adr_NX500_xmac3_xmac_rpu_program_end, 0x001633FF
 .equ Adr_NX500_xmac2_xmac_rpu_program_end, 0x001623FF
 .equ Adr_NX500_xmac0_xmac_tpu_program_start, 0x00160400
 .equ Adr_NX500_xmac1_xmac_tpu_program_start, 0x00161400
 .equ Adr_NX500_xmac3_xmac_tpu_program_start, 0x00163400
 .equ Adr_NX500_xmac2_xmac_tpu_program_start, 0x00162400
 .equ Adr_NX500_xmac0_xmac_tpu_program_end, 0x001607FF
 .equ Adr_NX500_xmac1_xmac_tpu_program_end, 0x001617FF
 .equ Adr_NX500_xmac3_xmac_tpu_program_end, 0x001637FF
 .equ Adr_NX500_xmac2_xmac_tpu_program_end, 0x001627FF
 .equ Adr_NX500_xmac0_xmac_sr0, 0x00160800
 .equ Adr_NX500_xmac1_xmac_sr0, 0x00161800
 .equ Adr_NX500_xmac3_xmac_sr0, 0x00163800
 .equ Adr_NX500_xmac2_xmac_sr0, 0x00162800
 .equ Adr_NX500_xmac0_xmac_sr1, 0x00160804
 .equ Adr_NX500_xmac1_xmac_sr1, 0x00161804
 .equ Adr_NX500_xmac3_xmac_sr1, 0x00163804
 .equ Adr_NX500_xmac2_xmac_sr1, 0x00162804
 .equ Adr_NX500_xmac0_xmac_sr2, 0x00160808
 .equ Adr_NX500_xmac1_xmac_sr2, 0x00161808
 .equ Adr_NX500_xmac3_xmac_sr2, 0x00163808
 .equ Adr_NX500_xmac2_xmac_sr2, 0x00162808
 .equ Adr_NX500_xmac0_xmac_sr3, 0x0016080C
 .equ Adr_NX500_xmac1_xmac_sr3, 0x0016180C
 .equ Adr_NX500_xmac3_xmac_sr3, 0x0016380C
 .equ Adr_NX500_xmac2_xmac_sr3, 0x0016280C
 .equ Adr_NX500_xmac0_xmac_sr4, 0x00160810
 .equ Adr_NX500_xmac1_xmac_sr4, 0x00161810
 .equ Adr_NX500_xmac3_xmac_sr4, 0x00163810
 .equ Adr_NX500_xmac2_xmac_sr4, 0x00162810
 .equ Adr_NX500_xmac0_xmac_sr5, 0x00160814
 .equ Adr_NX500_xmac1_xmac_sr5, 0x00161814
 .equ Adr_NX500_xmac3_xmac_sr5, 0x00163814
 .equ Adr_NX500_xmac2_xmac_sr5, 0x00162814
 .equ Adr_NX500_xmac0_xmac_sr6, 0x00160818
 .equ Adr_NX500_xmac1_xmac_sr6, 0x00161818
 .equ Adr_NX500_xmac3_xmac_sr6, 0x00163818
 .equ Adr_NX500_xmac2_xmac_sr6, 0x00162818
 .equ Adr_NX500_xmac0_xmac_sr7, 0x0016081C
 .equ Adr_NX500_xmac1_xmac_sr7, 0x0016181C
 .equ Adr_NX500_xmac3_xmac_sr7, 0x0016381C
 .equ Adr_NX500_xmac2_xmac_sr7, 0x0016281C
 .equ Adr_NX500_xmac0_xmac_sr8, 0x00160820
 .equ Adr_NX500_xmac1_xmac_sr8, 0x00161820
 .equ Adr_NX500_xmac3_xmac_sr8, 0x00163820
 .equ Adr_NX500_xmac2_xmac_sr8, 0x00162820
 .equ Adr_NX500_xmac0_xmac_sr9, 0x00160824
 .equ Adr_NX500_xmac1_xmac_sr9, 0x00161824
 .equ Adr_NX500_xmac3_xmac_sr9, 0x00163824
 .equ Adr_NX500_xmac2_xmac_sr9, 0x00162824
 .equ Adr_NX500_xmac0_xmac_sr10, 0x00160828
 .equ Adr_NX500_xmac1_xmac_sr10, 0x00161828
 .equ Adr_NX500_xmac3_xmac_sr10, 0x00163828
 .equ Adr_NX500_xmac2_xmac_sr10, 0x00162828
 .equ Adr_NX500_xmac0_xmac_sr11, 0x0016082C
 .equ Adr_NX500_xmac1_xmac_sr11, 0x0016182C
 .equ Adr_NX500_xmac3_xmac_sr11, 0x0016382C
 .equ Adr_NX500_xmac2_xmac_sr11, 0x0016282C
 .equ Adr_NX500_xmac0_xmac_sr12, 0x00160830
 .equ Adr_NX500_xmac1_xmac_sr12, 0x00161830
 .equ Adr_NX500_xmac3_xmac_sr12, 0x00163830
 .equ Adr_NX500_xmac2_xmac_sr12, 0x00162830
 .equ Adr_NX500_xmac0_xmac_sr13, 0x00160834
 .equ Adr_NX500_xmac1_xmac_sr13, 0x00161834
 .equ Adr_NX500_xmac3_xmac_sr13, 0x00163834
 .equ Adr_NX500_xmac2_xmac_sr13, 0x00162834
 .equ Adr_NX500_xmac0_xmac_sr14, 0x00160838
 .equ Adr_NX500_xmac1_xmac_sr14, 0x00161838
 .equ Adr_NX500_xmac3_xmac_sr14, 0x00163838
 .equ Adr_NX500_xmac2_xmac_sr14, 0x00162838
 .equ Adr_NX500_xmac0_xmac_sr15, 0x0016083C
 .equ Adr_NX500_xmac1_xmac_sr15, 0x0016183C
 .equ Adr_NX500_xmac3_xmac_sr15, 0x0016383C
 .equ Adr_NX500_xmac2_xmac_sr15, 0x0016283C
 .equ Adr_NX500_xmac0_xmac_status_shared0, 0x00160840
 .equ Adr_NX500_xmac1_xmac_status_shared0, 0x00161840
 .equ Adr_NX500_xmac3_xmac_status_shared0, 0x00163840
 .equ Adr_NX500_xmac2_xmac_status_shared0, 0x00162840
 .equ Adr_NX500_xmac0_xmac_config_shared0, 0x00160844
 .equ Adr_NX500_xmac1_xmac_config_shared0, 0x00161844
 .equ Adr_NX500_xmac3_xmac_config_shared0, 0x00163844
 .equ Adr_NX500_xmac2_xmac_config_shared0, 0x00162844
 .equ Adr_NX500_xmac0_xmac_status_shared1, 0x00160848
 .equ Adr_NX500_xmac1_xmac_status_shared1, 0x00161848
 .equ Adr_NX500_xmac3_xmac_status_shared1, 0x00163848
 .equ Adr_NX500_xmac2_xmac_status_shared1, 0x00162848
 .equ Adr_NX500_xmac0_xmac_config_shared1, 0x0016084C
 .equ Adr_NX500_xmac1_xmac_config_shared1, 0x0016184C
 .equ Adr_NX500_xmac3_xmac_config_shared1, 0x0016384C
 .equ Adr_NX500_xmac2_xmac_config_shared1, 0x0016284C
 .equ Adr_NX500_xmac0_xmac_status_shared2, 0x00160850
 .equ Adr_NX500_xmac1_xmac_status_shared2, 0x00161850
 .equ Adr_NX500_xmac3_xmac_status_shared2, 0x00163850
 .equ Adr_NX500_xmac2_xmac_status_shared2, 0x00162850
 .equ Adr_NX500_xmac0_xmac_config_shared2, 0x00160854
 .equ Adr_NX500_xmac1_xmac_config_shared2, 0x00161854
 .equ Adr_NX500_xmac3_xmac_config_shared2, 0x00163854
 .equ Adr_NX500_xmac2_xmac_config_shared2, 0x00162854
 .equ Adr_NX500_xmac0_xmac_status_shared3, 0x00160858
 .equ Adr_NX500_xmac1_xmac_status_shared3, 0x00161858
 .equ Adr_NX500_xmac3_xmac_status_shared3, 0x00163858
 .equ Adr_NX500_xmac2_xmac_status_shared3, 0x00162858
 .equ Adr_NX500_xmac0_xmac_config_shared3, 0x0016085C
 .equ Adr_NX500_xmac1_xmac_config_shared3, 0x0016185C
 .equ Adr_NX500_xmac3_xmac_config_shared3, 0x0016385C
 .equ Adr_NX500_xmac2_xmac_config_shared3, 0x0016285C
 .equ Adr_NX500_xmac0_xmac_urx_utx0, 0x00160860
 .equ Adr_NX500_xmac1_xmac_urx_utx0, 0x00161860
 .equ Adr_NX500_xmac3_xmac_urx_utx0, 0x00163860
 .equ Adr_NX500_xmac2_xmac_urx_utx0, 0x00162860
 .equ Adr_NX500_xmac0_xmac_urx_utx1, 0x00160864
 .equ Adr_NX500_xmac1_xmac_urx_utx1, 0x00161864
 .equ Adr_NX500_xmac3_xmac_urx_utx1, 0x00163864
 .equ Adr_NX500_xmac2_xmac_urx_utx1, 0x00162864
 .equ Adr_NX500_xmac0_xmac_urx_utx2, 0x00160868
 .equ Adr_NX500_xmac1_xmac_urx_utx2, 0x00161868
 .equ Adr_NX500_xmac3_xmac_urx_utx2, 0x00163868
 .equ Adr_NX500_xmac2_xmac_urx_utx2, 0x00162868
 .equ Adr_NX500_xmac0_xmac_urx_utx3, 0x0016086C
 .equ Adr_NX500_xmac1_xmac_urx_utx3, 0x0016186C
 .equ Adr_NX500_xmac3_xmac_urx_utx3, 0x0016386C
 .equ Adr_NX500_xmac2_xmac_urx_utx3, 0x0016286C
 .equ Adr_NX500_xmac0_xmac_urx, 0x00160870
 .equ Adr_NX500_xmac1_xmac_urx, 0x00161870
 .equ Adr_NX500_xmac3_xmac_urx, 0x00163870
 .equ Adr_NX500_xmac2_xmac_urx, 0x00162870
 .equ Adr_NX500_xmac0_xmac_utx, 0x00160874
 .equ Adr_NX500_xmac1_xmac_utx, 0x00161874
 .equ Adr_NX500_xmac3_xmac_utx, 0x00163874
 .equ Adr_NX500_xmac2_xmac_utx, 0x00162874
 .equ Adr_NX500_xmac0_xmac_rx, 0x00160878
 .equ Adr_NX500_xmac1_xmac_rx, 0x00161878
 .equ Adr_NX500_xmac3_xmac_rx, 0x00163878
 .equ Adr_NX500_xmac2_xmac_rx, 0x00162878
 .equ Adr_NX500_xmac0_xmac_rx_hw, 0x0016087C
 .equ Adr_NX500_xmac1_xmac_rx_hw, 0x0016187C
 .equ Adr_NX500_xmac3_xmac_rx_hw, 0x0016387C
 .equ Adr_NX500_xmac2_xmac_rx_hw, 0x0016287C
 .equ Adr_NX500_xmac0_xmac_rx_hw_count, 0x00160880
 .equ Adr_NX500_xmac1_xmac_rx_hw_count, 0x00161880
 .equ Adr_NX500_xmac3_xmac_rx_hw_count, 0x00163880
 .equ Adr_NX500_xmac2_xmac_rx_hw_count, 0x00162880
 .equ Adr_NX500_xmac0_xmac_tx, 0x00160884
 .equ Adr_NX500_xmac1_xmac_tx, 0x00161884
 .equ Adr_NX500_xmac3_xmac_tx, 0x00163884
 .equ Adr_NX500_xmac2_xmac_tx, 0x00162884
 .equ Adr_NX500_xmac0_xmac_tx_hw, 0x00160888
 .equ Adr_NX500_xmac1_xmac_tx_hw, 0x00161888
 .equ Adr_NX500_xmac3_xmac_tx_hw, 0x00163888
 .equ Adr_NX500_xmac2_xmac_tx_hw, 0x00162888
 .equ Adr_NX500_xmac0_xmac_tx_hw_count, 0x0016088C
 .equ Adr_NX500_xmac1_xmac_tx_hw_count, 0x0016188C
 .equ Adr_NX500_xmac3_xmac_tx_hw_count, 0x0016388C
 .equ Adr_NX500_xmac2_xmac_tx_hw_count, 0x0016288C
 .equ Adr_NX500_xmac0_xmac_tx_sent, 0x00160890
 .equ Adr_NX500_xmac1_xmac_tx_sent, 0x00161890
 .equ Adr_NX500_xmac3_xmac_tx_sent, 0x00163890
 .equ Adr_NX500_xmac2_xmac_tx_sent, 0x00162890
 .equ Adr_NX500_xmac0_xmac_rpu_pc, 0x00160894
 .equ Adr_NX500_xmac1_xmac_rpu_pc, 0x00161894
 .equ Adr_NX500_xmac3_xmac_rpu_pc, 0x00163894
 .equ Adr_NX500_xmac2_xmac_rpu_pc, 0x00162894
 .equ Adr_NX500_xmac0_xmac_tpu_pc, 0x00160898
 .equ Adr_NX500_xmac1_xmac_tpu_pc, 0x00161898
 .equ Adr_NX500_xmac3_xmac_tpu_pc, 0x00163898
 .equ Adr_NX500_xmac2_xmac_tpu_pc, 0x00162898
 .equ Adr_NX500_xmac0_xmac_wr0, 0x0016089C
 .equ Adr_NX500_xmac1_xmac_wr0, 0x0016189C
 .equ Adr_NX500_xmac3_xmac_wr0, 0x0016389C
 .equ Adr_NX500_xmac2_xmac_wr0, 0x0016289C
 .equ Adr_NX500_xmac0_xmac_wr1, 0x001608A0
 .equ Adr_NX500_xmac1_xmac_wr1, 0x001618A0
 .equ Adr_NX500_xmac3_xmac_wr1, 0x001638A0
 .equ Adr_NX500_xmac2_xmac_wr1, 0x001628A0
 .equ Adr_NX500_xmac0_xmac_wr2, 0x001608A4
 .equ Adr_NX500_xmac1_xmac_wr2, 0x001618A4
 .equ Adr_NX500_xmac3_xmac_wr2, 0x001638A4
 .equ Adr_NX500_xmac2_xmac_wr2, 0x001628A4
 .equ Adr_NX500_xmac0_xmac_wr3, 0x001608A8
 .equ Adr_NX500_xmac1_xmac_wr3, 0x001618A8
 .equ Adr_NX500_xmac3_xmac_wr3, 0x001638A8
 .equ Adr_NX500_xmac2_xmac_wr3, 0x001628A8
 .equ Adr_NX500_xmac0_xmac_wr4, 0x001608AC
 .equ Adr_NX500_xmac1_xmac_wr4, 0x001618AC
 .equ Adr_NX500_xmac3_xmac_wr4, 0x001638AC
 .equ Adr_NX500_xmac2_xmac_wr4, 0x001628AC
 .equ Adr_NX500_xmac0_xmac_wr5, 0x001608B0
 .equ Adr_NX500_xmac1_xmac_wr5, 0x001618B0
 .equ Adr_NX500_xmac3_xmac_wr5, 0x001638B0
 .equ Adr_NX500_xmac2_xmac_wr5, 0x001628B0
 .equ Adr_NX500_xmac0_xmac_wr6, 0x001608B4
 .equ Adr_NX500_xmac1_xmac_wr6, 0x001618B4
 .equ Adr_NX500_xmac3_xmac_wr6, 0x001638B4
 .equ Adr_NX500_xmac2_xmac_wr6, 0x001628B4
 .equ Adr_NX500_xmac0_xmac_wr7, 0x001608B8
 .equ Adr_NX500_xmac1_xmac_wr7, 0x001618B8
 .equ Adr_NX500_xmac3_xmac_wr7, 0x001638B8
 .equ Adr_NX500_xmac2_xmac_wr7, 0x001628B8
 .equ Adr_NX500_xmac0_xmac_wr8, 0x001608BC
 .equ Adr_NX500_xmac1_xmac_wr8, 0x001618BC
 .equ Adr_NX500_xmac3_xmac_wr8, 0x001638BC
 .equ Adr_NX500_xmac2_xmac_wr8, 0x001628BC
 .equ Adr_NX500_xmac0_xmac_wr9, 0x001608C0
 .equ Adr_NX500_xmac1_xmac_wr9, 0x001618C0
 .equ Adr_NX500_xmac3_xmac_wr9, 0x001638C0
 .equ Adr_NX500_xmac2_xmac_wr9, 0x001628C0
 .equ Adr_NX500_xmac0_xmac_sys_time, 0x001608C4
 .equ Adr_NX500_xmac1_xmac_sys_time, 0x001618C4
 .equ Adr_NX500_xmac3_xmac_sys_time, 0x001638C4
 .equ Adr_NX500_xmac2_xmac_sys_time, 0x001628C4
 .equ Adr_NX500_xmac0_xmac_cmp0_status, 0x001608C8
 .equ Adr_NX500_xmac1_xmac_cmp0_status, 0x001618C8
 .equ Adr_NX500_xmac3_xmac_cmp0_status, 0x001638C8
 .equ Adr_NX500_xmac2_xmac_cmp0_status, 0x001628C8
 .equ Adr_NX500_xmac0_xmac_cmp1_status, 0x001608CC
 .equ Adr_NX500_xmac1_xmac_cmp1_status, 0x001618CC
 .equ Adr_NX500_xmac3_xmac_cmp1_status, 0x001638CC
 .equ Adr_NX500_xmac2_xmac_cmp1_status, 0x001628CC
 .equ Adr_NX500_xmac0_xmac_cmp2_status, 0x001608D0
 .equ Adr_NX500_xmac1_xmac_cmp2_status, 0x001618D0
 .equ Adr_NX500_xmac3_xmac_cmp2_status, 0x001638D0
 .equ Adr_NX500_xmac2_xmac_cmp2_status, 0x001628D0
 .equ Adr_NX500_xmac0_xmac_cmp3_status, 0x001608D4
 .equ Adr_NX500_xmac1_xmac_cmp3_status, 0x001618D4
 .equ Adr_NX500_xmac3_xmac_cmp3_status, 0x001638D4
 .equ Adr_NX500_xmac2_xmac_cmp3_status, 0x001628D4
 .equ Adr_NX500_xmac0_xmac_alu_flags, 0x001608D8
 .equ Adr_NX500_xmac1_xmac_alu_flags, 0x001618D8
 .equ Adr_NX500_xmac3_xmac_alu_flags, 0x001638D8
 .equ Adr_NX500_xmac2_xmac_alu_flags, 0x001628D8
 .equ Adr_NX500_xmac0_xmac_status_int, 0x001608DC
 .equ Adr_NX500_xmac1_xmac_status_int, 0x001618DC
 .equ Adr_NX500_xmac3_xmac_status_int, 0x001638DC
 .equ Adr_NX500_xmac2_xmac_status_int, 0x001628DC
 .equ Adr_NX500_xmac0_xmac_stat_bits, 0x001608E0
 .equ Adr_NX500_xmac1_xmac_stat_bits, 0x001618E0
 .equ Adr_NX500_xmac3_xmac_stat_bits, 0x001638E0
 .equ Adr_NX500_xmac2_xmac_stat_bits, 0x001628E0
 .equ Adr_NX500_xmac0_xmac_status_mii, 0x001608E4
 .equ Adr_NX500_xmac1_xmac_status_mii, 0x001618E4
 .equ Adr_NX500_xmac3_xmac_status_mii, 0x001638E4
 .equ Adr_NX500_xmac2_xmac_status_mii, 0x001628E4
 .equ Adr_NX500_xmac0_xmac_status_mii2, 0x001608E8
 .equ Adr_NX500_xmac1_xmac_status_mii2, 0x001618E8
 .equ Adr_NX500_xmac3_xmac_status_mii2, 0x001638E8
 .equ Adr_NX500_xmac2_xmac_status_mii2, 0x001628E8
 .equ Adr_NX500_xmac0_xmac_config_mii, 0x001608EC
 .equ Adr_NX500_xmac1_xmac_config_mii, 0x001618EC
 .equ Adr_NX500_xmac3_xmac_config_mii, 0x001638EC
 .equ Adr_NX500_xmac2_xmac_config_mii, 0x001628EC
 .equ Adr_NX500_xmac0_xmac_config_nibble_fifo, 0x001608F0
 .equ Adr_NX500_xmac1_xmac_config_nibble_fifo, 0x001618F0
 .equ Adr_NX500_xmac3_xmac_config_nibble_fifo, 0x001638F0
 .equ Adr_NX500_xmac2_xmac_config_nibble_fifo, 0x001628F0
 .equ Adr_NX500_xmac0_xmac_config_sbu, 0x001608F4
 .equ Adr_NX500_xmac1_xmac_config_sbu, 0x001618F4
 .equ Adr_NX500_xmac3_xmac_config_sbu, 0x001638F4
 .equ Adr_NX500_xmac2_xmac_config_sbu, 0x001628F4
 .equ Adr_NX500_xmac0_xmac_sbu_rate_mul_add, 0x001608F8
 .equ Adr_NX500_xmac1_xmac_sbu_rate_mul_add, 0x001618F8
 .equ Adr_NX500_xmac3_xmac_sbu_rate_mul_add, 0x001638F8
 .equ Adr_NX500_xmac2_xmac_sbu_rate_mul_add, 0x001628F8
 .equ Adr_NX500_xmac0_xmac_sbu_rate_mul_start, 0x001608FC
 .equ Adr_NX500_xmac1_xmac_sbu_rate_mul_start, 0x001618FC
 .equ Adr_NX500_xmac3_xmac_sbu_rate_mul_start, 0x001638FC
 .equ Adr_NX500_xmac2_xmac_sbu_rate_mul_start, 0x001628FC
 .equ Adr_NX500_xmac0_xmac_sbu_rate_mul, 0x00160900
 .equ Adr_NX500_xmac1_xmac_sbu_rate_mul, 0x00161900
 .equ Adr_NX500_xmac3_xmac_sbu_rate_mul, 0x00163900
 .equ Adr_NX500_xmac2_xmac_sbu_rate_mul, 0x00162900
 .equ Adr_NX500_xmac0_xmac_start_sample_pos, 0x00160904
 .equ Adr_NX500_xmac1_xmac_start_sample_pos, 0x00161904
 .equ Adr_NX500_xmac3_xmac_start_sample_pos, 0x00163904
 .equ Adr_NX500_xmac2_xmac_start_sample_pos, 0x00162904
 .equ Adr_NX500_xmac0_xmac_stop_sample_pos, 0x00160908
 .equ Adr_NX500_xmac1_xmac_stop_sample_pos, 0x00161908
 .equ Adr_NX500_xmac3_xmac_stop_sample_pos, 0x00163908
 .equ Adr_NX500_xmac2_xmac_stop_sample_pos, 0x00162908
 .equ Adr_NX500_xmac0_xmac_config_obu, 0x0016090C
 .equ Adr_NX500_xmac1_xmac_config_obu, 0x0016190C
 .equ Adr_NX500_xmac3_xmac_config_obu, 0x0016390C
 .equ Adr_NX500_xmac2_xmac_config_obu, 0x0016290C
 .equ Adr_NX500_xmac0_xmac_obu_rate_mul_add, 0x00160910
 .equ Adr_NX500_xmac1_xmac_obu_rate_mul_add, 0x00161910
 .equ Adr_NX500_xmac3_xmac_obu_rate_mul_add, 0x00163910
 .equ Adr_NX500_xmac2_xmac_obu_rate_mul_add, 0x00162910
 .equ Adr_NX500_xmac0_xmac_obu_rate_mul_start, 0x00160914
 .equ Adr_NX500_xmac1_xmac_obu_rate_mul_start, 0x00161914
 .equ Adr_NX500_xmac3_xmac_obu_rate_mul_start, 0x00163914
 .equ Adr_NX500_xmac2_xmac_obu_rate_mul_start, 0x00162914
 .equ Adr_NX500_xmac0_xmac_obu_rate_mul, 0x00160918
 .equ Adr_NX500_xmac1_xmac_obu_rate_mul, 0x00161918
 .equ Adr_NX500_xmac3_xmac_obu_rate_mul, 0x00163918
 .equ Adr_NX500_xmac2_xmac_obu_rate_mul, 0x00162918
 .equ Adr_NX500_xmac0_xmac_start_trans_pos, 0x0016091C
 .equ Adr_NX500_xmac1_xmac_start_trans_pos, 0x0016191C
 .equ Adr_NX500_xmac3_xmac_start_trans_pos, 0x0016391C
 .equ Adr_NX500_xmac2_xmac_start_trans_pos, 0x0016291C
 .equ Adr_NX500_xmac0_xmac_stop_trans_pos, 0x00160920
 .equ Adr_NX500_xmac1_xmac_stop_trans_pos, 0x00161920
 .equ Adr_NX500_xmac3_xmac_stop_trans_pos, 0x00163920
 .equ Adr_NX500_xmac2_xmac_stop_trans_pos, 0x00162920
 .equ Adr_NX500_xmac0_xmac_rpu_count1, 0x00160924
 .equ Adr_NX500_xmac1_xmac_rpu_count1, 0x00161924
 .equ Adr_NX500_xmac3_xmac_rpu_count1, 0x00163924
 .equ Adr_NX500_xmac2_xmac_rpu_count1, 0x00162924
 .equ Adr_NX500_xmac0_xmac_rpu_count2, 0x00160928
 .equ Adr_NX500_xmac1_xmac_rpu_count2, 0x00161928
 .equ Adr_NX500_xmac3_xmac_rpu_count2, 0x00163928
 .equ Adr_NX500_xmac2_xmac_rpu_count2, 0x00162928
 .equ Adr_NX500_xmac0_xmac_tpu_count1, 0x0016092C
 .equ Adr_NX500_xmac1_xmac_tpu_count1, 0x0016192C
 .equ Adr_NX500_xmac3_xmac_tpu_count1, 0x0016392C
 .equ Adr_NX500_xmac2_xmac_tpu_count1, 0x0016292C
 .equ Adr_NX500_xmac0_xmac_tpu_count2, 0x00160930
 .equ Adr_NX500_xmac1_xmac_tpu_count2, 0x00161930
 .equ Adr_NX500_xmac3_xmac_tpu_count2, 0x00163930
 .equ Adr_NX500_xmac2_xmac_tpu_count2, 0x00162930
 .equ Adr_NX500_xmac0_xmac_rx_count, 0x00160934
 .equ Adr_NX500_xmac1_xmac_rx_count, 0x00161934
 .equ Adr_NX500_xmac3_xmac_rx_count, 0x00163934
 .equ Adr_NX500_xmac2_xmac_rx_count, 0x00162934
 .equ Adr_NX500_xmac0_xmac_tx_count, 0x00160938
 .equ Adr_NX500_xmac1_xmac_tx_count, 0x00161938
 .equ Adr_NX500_xmac3_xmac_tx_count, 0x00163938
 .equ Adr_NX500_xmac2_xmac_tx_count, 0x00162938
 .equ Adr_NX500_xmac0_xmac_rpm_mask0, 0x0016093C
 .equ Adr_NX500_xmac1_xmac_rpm_mask0, 0x0016193C
 .equ Adr_NX500_xmac3_xmac_rpm_mask0, 0x0016393C
 .equ Adr_NX500_xmac2_xmac_rpm_mask0, 0x0016293C
 .equ Adr_NX500_xmac0_xmac_rpm_val0, 0x00160940
 .equ Adr_NX500_xmac1_xmac_rpm_val0, 0x00161940
 .equ Adr_NX500_xmac3_xmac_rpm_val0, 0x00163940
 .equ Adr_NX500_xmac2_xmac_rpm_val0, 0x00162940
 .equ Adr_NX500_xmac0_xmac_rpm_mask1, 0x00160944
 .equ Adr_NX500_xmac1_xmac_rpm_mask1, 0x00161944
 .equ Adr_NX500_xmac3_xmac_rpm_mask1, 0x00163944
 .equ Adr_NX500_xmac2_xmac_rpm_mask1, 0x00162944
 .equ Adr_NX500_xmac0_xmac_rpm_val1, 0x00160948
 .equ Adr_NX500_xmac1_xmac_rpm_val1, 0x00161948
 .equ Adr_NX500_xmac3_xmac_rpm_val1, 0x00163948
 .equ Adr_NX500_xmac2_xmac_rpm_val1, 0x00162948
 .equ Adr_NX500_xmac0_xmac_tpm_mask0, 0x0016094C
 .equ Adr_NX500_xmac1_xmac_tpm_mask0, 0x0016194C
 .equ Adr_NX500_xmac3_xmac_tpm_mask0, 0x0016394C
 .equ Adr_NX500_xmac2_xmac_tpm_mask0, 0x0016294C
 .equ Adr_NX500_xmac0_xmac_tpm_val0, 0x00160950
 .equ Adr_NX500_xmac1_xmac_tpm_val0, 0x00161950
 .equ Adr_NX500_xmac3_xmac_tpm_val0, 0x00163950
 .equ Adr_NX500_xmac2_xmac_tpm_val0, 0x00162950
 .equ Adr_NX500_xmac0_xmac_tpm_mask1, 0x00160954
 .equ Adr_NX500_xmac1_xmac_tpm_mask1, 0x00161954
 .equ Adr_NX500_xmac3_xmac_tpm_mask1, 0x00163954
 .equ Adr_NX500_xmac2_xmac_tpm_mask1, 0x00162954
 .equ Adr_NX500_xmac0_xmac_tpm_val1, 0x00160958
 .equ Adr_NX500_xmac1_xmac_tpm_val1, 0x00161958
 .equ Adr_NX500_xmac3_xmac_tpm_val1, 0x00163958
 .equ Adr_NX500_xmac2_xmac_tpm_val1, 0x00162958
 .equ Adr_NX500_xmac0_xmac_pwm_config, 0x0016095C
 .equ Adr_NX500_xmac1_xmac_pwm_config, 0x0016195C
 .equ Adr_NX500_xmac3_xmac_pwm_config, 0x0016395C
 .equ Adr_NX500_xmac2_xmac_pwm_config, 0x0016295C
 .equ Adr_NX500_xmac0_xmac_pwm_status, 0x00160960
 .equ Adr_NX500_xmac1_xmac_pwm_status, 0x00161960
 .equ Adr_NX500_xmac3_xmac_pwm_status, 0x00163960
 .equ Adr_NX500_xmac2_xmac_pwm_status, 0x00162960
 .equ Adr_NX500_xmac0_xmac_pwm_tp, 0x00160964
 .equ Adr_NX500_xmac1_xmac_pwm_tp, 0x00161964
 .equ Adr_NX500_xmac3_xmac_pwm_tp, 0x00163964
 .equ Adr_NX500_xmac2_xmac_pwm_tp, 0x00162964
 .equ Adr_NX500_xmac0_xmac_pwm_tu, 0x00160968
 .equ Adr_NX500_xmac1_xmac_pwm_tu, 0x00161968
 .equ Adr_NX500_xmac3_xmac_pwm_tu, 0x00163968
 .equ Adr_NX500_xmac2_xmac_pwm_tu, 0x00162968
 .equ Adr_NX500_xmac0_xmac_pwm_tv, 0x0016096C
 .equ Adr_NX500_xmac1_xmac_pwm_tv, 0x0016196C
 .equ Adr_NX500_xmac3_xmac_pwm_tv, 0x0016396C
 .equ Adr_NX500_xmac2_xmac_pwm_tv, 0x0016296C
 .equ Adr_NX500_xmac0_xmac_pwm_tw, 0x00160970
 .equ Adr_NX500_xmac1_xmac_pwm_tw, 0x00161970
 .equ Adr_NX500_xmac3_xmac_pwm_tw, 0x00163970
 .equ Adr_NX500_xmac2_xmac_pwm_tw, 0x00162970
 .equ Adr_NX500_xmac0_xmac_pwm_td, 0x00160974
 .equ Adr_NX500_xmac1_xmac_pwm_td, 0x00161974
 .equ Adr_NX500_xmac3_xmac_pwm_td, 0x00163974
 .equ Adr_NX500_xmac2_xmac_pwm_td, 0x00162974
 .equ Adr_NX500_xmac0_xmac_rpwm_tp, 0x00160978
 .equ Adr_NX500_xmac1_xmac_rpwm_tp, 0x00161978
 .equ Adr_NX500_xmac3_xmac_rpwm_tp, 0x00163978
 .equ Adr_NX500_xmac2_xmac_rpwm_tp, 0x00162978
 .equ Adr_NX500_xmac0_xmac_rpwm_tr, 0x0016097C
 .equ Adr_NX500_xmac1_xmac_rpwm_tr, 0x0016197C
 .equ Adr_NX500_xmac3_xmac_rpwm_tr, 0x0016397C
 .equ Adr_NX500_xmac2_xmac_rpwm_tr, 0x0016297C
 .equ Adr_NX500_xmac0_xmac_pwm_cnt, 0x00160980
 .equ Adr_NX500_xmac1_xmac_pwm_cnt, 0x00161980
 .equ Adr_NX500_xmac3_xmac_pwm_cnt, 0x00163980
 .equ Adr_NX500_xmac2_xmac_pwm_cnt, 0x00162980
 .equ Adr_NX500_xmac0_xmac_rpwm_cnt, 0x00160984
 .equ Adr_NX500_xmac1_xmac_rpwm_cnt, 0x00161984
 .equ Adr_NX500_xmac3_xmac_rpwm_cnt, 0x00163984
 .equ Adr_NX500_xmac2_xmac_rpwm_cnt, 0x00162984
 .equ Adr_NX500_xmac0_xmac_pwm_strtime, 0x00160988
 .equ Adr_NX500_xmac1_xmac_pwm_strtime, 0x00161988
 .equ Adr_NX500_xmac3_xmac_pwm_strtime, 0x00163988
 .equ Adr_NX500_xmac2_xmac_pwm_strtime, 0x00162988
 .equ Adr_NX500_xmac0_xmac_rpwm_strtime, 0x0016098C
 .equ Adr_NX500_xmac1_xmac_rpwm_strtime, 0x0016198C
 .equ Adr_NX500_xmac3_xmac_rpwm_strtime, 0x0016398C
 .equ Adr_NX500_xmac2_xmac_rpwm_strtime, 0x0016298C
 .equ Adr_NX500_xmac0_xmac_pos_config_encoder, 0x00160990
 .equ Adr_NX500_xmac1_xmac_pos_config_encoder, 0x00161990
 .equ Adr_NX500_xmac3_xmac_pos_config_encoder, 0x00163990
 .equ Adr_NX500_xmac2_xmac_pos_config_encoder, 0x00162990
 .equ Adr_NX500_xmac0_xmac_pos_config_capture, 0x00160994
 .equ Adr_NX500_xmac1_xmac_pos_config_capture, 0x00161994
 .equ Adr_NX500_xmac3_xmac_pos_config_capture, 0x00163994
 .equ Adr_NX500_xmac2_xmac_pos_config_capture, 0x00162994
 .equ Adr_NX500_xmac0_xmac_pos_command, 0x00160998
 .equ Adr_NX500_xmac1_xmac_pos_command, 0x00161998
 .equ Adr_NX500_xmac3_xmac_pos_command, 0x00163998
 .equ Adr_NX500_xmac2_xmac_pos_command, 0x00162998
 .equ Adr_NX500_xmac0_xmac_pos_status, 0x0016099C
 .equ Adr_NX500_xmac1_xmac_pos_status, 0x0016199C
 .equ Adr_NX500_xmac3_xmac_pos_status, 0x0016399C
 .equ Adr_NX500_xmac2_xmac_pos_status, 0x0016299C
 .equ Adr_NX500_xmac0_xmac_pos_enc0_position, 0x001609A0
 .equ Adr_NX500_xmac1_xmac_pos_enc0_position, 0x001619A0
 .equ Adr_NX500_xmac3_xmac_pos_enc0_position, 0x001639A0
 .equ Adr_NX500_xmac2_xmac_pos_enc0_position, 0x001629A0
 .equ Adr_NX500_xmac0_xmac_pos_enc0_nullposition, 0x001609A4
 .equ Adr_NX500_xmac1_xmac_pos_enc0_nullposition, 0x001619A4
 .equ Adr_NX500_xmac3_xmac_pos_enc0_nullposition, 0x001639A4
 .equ Adr_NX500_xmac2_xmac_pos_enc0_nullposition, 0x001629A4
 .equ Adr_NX500_xmac0_xmac_pos_enc1_position, 0x001609A8
 .equ Adr_NX500_xmac1_xmac_pos_enc1_position, 0x001619A8
 .equ Adr_NX500_xmac3_xmac_pos_enc1_position, 0x001639A8
 .equ Adr_NX500_xmac2_xmac_pos_enc1_position, 0x001629A8
 .equ Adr_NX500_xmac0_xmac_pos_enc1_nullposition, 0x001609AC
 .equ Adr_NX500_xmac1_xmac_pos_enc1_nullposition, 0x001619AC
 .equ Adr_NX500_xmac3_xmac_pos_enc1_nullposition, 0x001639AC
 .equ Adr_NX500_xmac2_xmac_pos_enc1_nullposition, 0x001629AC
 .equ Adr_NX500_xmac0_xmac_pos_enc0_edgetime, 0x001609B0
 .equ Adr_NX500_xmac1_xmac_pos_enc0_edgetime, 0x001619B0
 .equ Adr_NX500_xmac3_xmac_pos_enc0_edgetime, 0x001639B0
 .equ Adr_NX500_xmac2_xmac_pos_enc0_edgetime, 0x001629B0
 .equ Adr_NX500_xmac0_xmac_pos_enc1_edgetime, 0x001609B4
 .equ Adr_NX500_xmac1_xmac_pos_enc1_edgetime, 0x001619B4
 .equ Adr_NX500_xmac3_xmac_pos_enc1_edgetime, 0x001639B4
 .equ Adr_NX500_xmac2_xmac_pos_enc1_edgetime, 0x001629B4
 .equ Adr_NX500_xmac0_xmac_pos_capture0, 0x001609B8
 .equ Adr_NX500_xmac1_xmac_pos_capture0, 0x001619B8
 .equ Adr_NX500_xmac3_xmac_pos_capture0, 0x001639B8
 .equ Adr_NX500_xmac2_xmac_pos_capture0, 0x001629B8
 .equ Adr_NX500_xmac0_xmac_pos_capture1, 0x001609BC
 .equ Adr_NX500_xmac1_xmac_pos_capture1, 0x001619BC
 .equ Adr_NX500_xmac3_xmac_pos_capture1, 0x001639BC
 .equ Adr_NX500_xmac2_xmac_pos_capture1, 0x001629BC
 .equ Adr_NX500_xmac0_xmac_pos_capture2, 0x001609C0
 .equ Adr_NX500_xmac1_xmac_pos_capture2, 0x001619C0
 .equ Adr_NX500_xmac3_xmac_pos_capture2, 0x001639C0
 .equ Adr_NX500_xmac2_xmac_pos_capture2, 0x001629C0
 .equ Adr_NX500_xmac0_xmac_pos_capture3, 0x001609C4
 .equ Adr_NX500_xmac1_xmac_pos_capture3, 0x001619C4
 .equ Adr_NX500_xmac3_xmac_pos_capture3, 0x001639C4
 .equ Adr_NX500_xmac2_xmac_pos_capture3, 0x001629C4
 .equ Adr_NX500_xmac0_xmac_rx_crc_polynomial_l, 0x001609D8
 .equ Adr_NX500_xmac1_xmac_rx_crc_polynomial_l, 0x001619D8
 .equ Adr_NX500_xmac3_xmac_rx_crc_polynomial_l, 0x001639D8
 .equ Adr_NX500_xmac2_xmac_rx_crc_polynomial_l, 0x001629D8
 .equ Adr_NX500_xmac0_xmac_rx_crc_polynomial_h, 0x001609DC
 .equ Adr_NX500_xmac1_xmac_rx_crc_polynomial_h, 0x001619DC
 .equ Adr_NX500_xmac3_xmac_rx_crc_polynomial_h, 0x001639DC
 .equ Adr_NX500_xmac2_xmac_rx_crc_polynomial_h, 0x001629DC
 .equ Adr_NX500_xmac0_xmac_rx_crc_l, 0x001609E0
 .equ Adr_NX500_xmac1_xmac_rx_crc_l, 0x001619E0
 .equ Adr_NX500_xmac3_xmac_rx_crc_l, 0x001639E0
 .equ Adr_NX500_xmac2_xmac_rx_crc_l, 0x001629E0
 .equ Adr_NX500_xmac0_xmac_rx_crc_h, 0x001609E4
 .equ Adr_NX500_xmac1_xmac_rx_crc_h, 0x001619E4
 .equ Adr_NX500_xmac3_xmac_rx_crc_h, 0x001639E4
 .equ Adr_NX500_xmac2_xmac_rx_crc_h, 0x001629E4
 .equ Adr_NX500_xmac0_xmac_rx_crc_cfg, 0x001609E8
 .equ Adr_NX500_xmac1_xmac_rx_crc_cfg, 0x001619E8
 .equ Adr_NX500_xmac3_xmac_rx_crc_cfg, 0x001639E8
 .equ Adr_NX500_xmac2_xmac_rx_crc_cfg, 0x001629E8
 .equ Adr_NX500_xmac0_xmac_tx_crc_polynomial_l, 0x001609EC
 .equ Adr_NX500_xmac1_xmac_tx_crc_polynomial_l, 0x001619EC
 .equ Adr_NX500_xmac3_xmac_tx_crc_polynomial_l, 0x001639EC
 .equ Adr_NX500_xmac2_xmac_tx_crc_polynomial_l, 0x001629EC
 .equ Adr_NX500_xmac0_xmac_tx_crc_polynomial_h, 0x001609F0
 .equ Adr_NX500_xmac1_xmac_tx_crc_polynomial_h, 0x001619F0
 .equ Adr_NX500_xmac3_xmac_tx_crc_polynomial_h, 0x001639F0
 .equ Adr_NX500_xmac2_xmac_tx_crc_polynomial_h, 0x001629F0
 .equ Adr_NX500_xmac0_xmac_tx_crc_l, 0x001609F4
 .equ Adr_NX500_xmac1_xmac_tx_crc_l, 0x001619F4
 .equ Adr_NX500_xmac3_xmac_tx_crc_l, 0x001639F4
 .equ Adr_NX500_xmac2_xmac_tx_crc_l, 0x001629F4
 .equ Adr_NX500_xmac0_xmac_tx_crc_h, 0x001609F8
 .equ Adr_NX500_xmac1_xmac_tx_crc_h, 0x001619F8
 .equ Adr_NX500_xmac3_xmac_tx_crc_h, 0x001639F8
 .equ Adr_NX500_xmac2_xmac_tx_crc_h, 0x001629F8
 .equ Adr_NX500_xmac0_xmac_tx_crc_cfg, 0x001609FC
 .equ Adr_NX500_xmac1_xmac_tx_crc_cfg, 0x001619FC
 .equ Adr_NX500_xmac3_xmac_tx_crc_cfg, 0x001639FC
 .equ Adr_NX500_xmac2_xmac_tx_crc_cfg, 0x001629FC
 .equ Adr_NX500_xmac0_xmac_rpu_hold_pc, 0x00160A00
 .equ Adr_NX500_xmac1_xmac_rpu_hold_pc, 0x00161A00
 .equ Adr_NX500_xmac3_xmac_rpu_hold_pc, 0x00163A00
 .equ Adr_NX500_xmac2_xmac_rpu_hold_pc, 0x00162A00
 .equ Adr_NX500_xmac0_xmac_tpu_hold_pc, 0x00160A04
 .equ Adr_NX500_xmac1_xmac_tpu_hold_pc, 0x00161A04
 .equ Adr_NX500_xmac3_xmac_tpu_hold_pc, 0x00163A04
 .equ Adr_NX500_xmac2_xmac_tpu_hold_pc, 0x00162A04

@ =====================================================================
@
@ Area pointer_fifo
 .equ Addr_NX500_pointer_fifo, 0x00164000
@
@ =====================================================================
 .equ Adr_NX500_pointer_fifo_pfifo_base, 0x00164000
 .equ Adr_NX500_pfifo_base, 0x00164000
 .equ Adr_NX500_pointer_fifo_pfifo_border_base, 0x00164080
 .equ Adr_NX500_pfifo_border_base, 0x00164080
 .equ Adr_NX500_pointer_fifo_pfifo_reset, 0x00164100
 .equ Adr_NX500_pfifo_reset, 0x00164100
 .equ Adr_NX500_pointer_fifo_pfifo_full, 0x00164104
 .equ Adr_NX500_pfifo_full, 0x00164104
 .equ Adr_NX500_pointer_fifo_pfifo_empty, 0x00164108
 .equ Adr_NX500_pfifo_empty, 0x00164108
 .equ Adr_NX500_pointer_fifo_pfifo_overflow, 0x0016410C
 .equ Adr_NX500_pfifo_overflow, 0x0016410C
 .equ Adr_NX500_pointer_fifo_pfifo_underrun, 0x00164110
 .equ Adr_NX500_pfifo_underrun, 0x00164110
 .equ Adr_NX500_pointer_fifo_pfifo_fill_level_base, 0x00164180
 .equ Adr_NX500_pfifo_fill_level_base, 0x00164180

@ =====================================================================
@
@ Area xpec_irq_registers
 .equ Addr_NX500_xpec_irq_registers, 0x00164400
@
@ =====================================================================
 .equ Adr_NX500_xpec_irq_registers_irq_xpec0, 0x00164400
 .equ Adr_NX500_irq_xpec0, 0x00164400
 .equ Adr_NX500_xpec_irq_registers_irq_xpec1, 0x00164404
 .equ Adr_NX500_irq_xpec1, 0x00164404
 .equ Adr_NX500_xpec_irq_registers_irq_xpec2, 0x00164408
 .equ Adr_NX500_irq_xpec2, 0x00164408
 .equ Adr_NX500_xpec_irq_registers_irq_xpec3, 0x0016440C
 .equ Adr_NX500_irq_xpec3, 0x0016440C

@ =====================================================================
@
@ Area xc_mem_prot
 .equ Addr_NX500_xc_mem_prot, 0x00164800
@
@ =====================================================================
 .equ Adr_NX500_xc_mem_prot_xpec0_ram_low, 0x00164800
 .equ Adr_NX500_xpec0_ram_low, 0x00164800
 .equ Adr_NX500_xc_mem_prot_xpec1_ram_low, 0x00164804
 .equ Adr_NX500_xpec1_ram_low, 0x00164804
 .equ Adr_NX500_xc_mem_prot_xpec2_ram_low, 0x00164808
 .equ Adr_NX500_xpec2_ram_low, 0x00164808
 .equ Adr_NX500_xc_mem_prot_xpec3_ram_low, 0x0016480C
 .equ Adr_NX500_xpec3_ram_low, 0x0016480C
 .equ Adr_NX500_xc_mem_prot_xpec0_ram_high, 0x00164810
 .equ Adr_NX500_xpec0_ram_high, 0x00164810
 .equ Adr_NX500_xc_mem_prot_xpec1_ram_high, 0x00164814
 .equ Adr_NX500_xpec1_ram_high, 0x00164814
 .equ Adr_NX500_xc_mem_prot_xpec2_ram_high, 0x00164818
 .equ Adr_NX500_xpec2_ram_high, 0x00164818
 .equ Adr_NX500_xc_mem_prot_xpec3_ram_high, 0x0016481C
 .equ Adr_NX500_xpec3_ram_high, 0x0016481C
 .equ Adr_NX500_xc_mem_prot_prot_key_enable, 0x00164900
 .equ Adr_NX500_prot_key_enable, 0x00164900
 .equ Adr_NX500_xc_mem_prot_prot_key_disable, 0x00164904
 .equ Adr_NX500_prot_key_disable, 0x00164904
 .equ Adr_NX500_xc_mem_prot_prot_status, 0x00164908
 .equ Adr_NX500_prot_status, 0x00164908

@ =====================================================================
@
@ Area xc_debug
 .equ Addr_NX500_xc_debug, 0x00164C00
@
@ =====================================================================
 .equ Adr_NX500_xc_debug_xc_debug_config, 0x00164C00
 .equ Adr_NX500_xc_debug_config, 0x00164C00

@ =====================================================================
@
@ Area xpec
 .equ Addr_NX500_xpec0, 0x00170000
 .equ Addr_NX500_xpec1, 0x00174000
 .equ Addr_NX500_xpec2, 0x00178000
 .equ Addr_NX500_xpec3, 0x0017C000
@
@ =====================================================================
 .equ Adr_NX500_xpec0_xpec_r0, 0x00170000
 .equ Adr_NX500_xpec1_xpec_r0, 0x00174000
 .equ Adr_NX500_xpec2_xpec_r0, 0x00178000
 .equ Adr_NX500_xpec3_xpec_r0, 0x0017C000
 .equ Adr_NX500_xpec0_xpec_r1, 0x00170004
 .equ Adr_NX500_xpec1_xpec_r1, 0x00174004
 .equ Adr_NX500_xpec2_xpec_r1, 0x00178004
 .equ Adr_NX500_xpec3_xpec_r1, 0x0017C004
 .equ Adr_NX500_xpec0_xpec_r2, 0x00170008
 .equ Adr_NX500_xpec1_xpec_r2, 0x00174008
 .equ Adr_NX500_xpec2_xpec_r2, 0x00178008
 .equ Adr_NX500_xpec3_xpec_r2, 0x0017C008
 .equ Adr_NX500_xpec0_xpec_r3, 0x0017000C
 .equ Adr_NX500_xpec1_xpec_r3, 0x0017400C
 .equ Adr_NX500_xpec2_xpec_r3, 0x0017800C
 .equ Adr_NX500_xpec3_xpec_r3, 0x0017C00C
 .equ Adr_NX500_xpec0_xpec_r4, 0x00170010
 .equ Adr_NX500_xpec1_xpec_r4, 0x00174010
 .equ Adr_NX500_xpec2_xpec_r4, 0x00178010
 .equ Adr_NX500_xpec3_xpec_r4, 0x0017C010
 .equ Adr_NX500_xpec0_xpec_r5, 0x00170014
 .equ Adr_NX500_xpec1_xpec_r5, 0x00174014
 .equ Adr_NX500_xpec2_xpec_r5, 0x00178014
 .equ Adr_NX500_xpec3_xpec_r5, 0x0017C014
 .equ Adr_NX500_xpec0_xpec_r6, 0x00170018
 .equ Adr_NX500_xpec1_xpec_r6, 0x00174018
 .equ Adr_NX500_xpec2_xpec_r6, 0x00178018
 .equ Adr_NX500_xpec3_xpec_r6, 0x0017C018
 .equ Adr_NX500_xpec0_xpec_r7, 0x0017001C
 .equ Adr_NX500_xpec1_xpec_r7, 0x0017401C
 .equ Adr_NX500_xpec2_xpec_r7, 0x0017801C
 .equ Adr_NX500_xpec3_xpec_r7, 0x0017C01C
 .equ Adr_NX500_xpec0_range01, 0x00170020
 .equ Adr_NX500_xpec1_range01, 0x00174020
 .equ Adr_NX500_xpec2_range01, 0x00178020
 .equ Adr_NX500_xpec3_range01, 0x0017C020
 .equ Adr_NX500_xpec0_range23, 0x00170024
 .equ Adr_NX500_xpec1_range23, 0x00174024
 .equ Adr_NX500_xpec2_range23, 0x00178024
 .equ Adr_NX500_xpec3_range23, 0x0017C024
 .equ Adr_NX500_xpec0_range45, 0x00170028
 .equ Adr_NX500_xpec1_range45, 0x00174028
 .equ Adr_NX500_xpec2_range45, 0x00178028
 .equ Adr_NX500_xpec3_range45, 0x0017C028
 .equ Adr_NX500_xpec0_range67, 0x0017002C
 .equ Adr_NX500_xpec1_range67, 0x0017402C
 .equ Adr_NX500_xpec2_range67, 0x0017802C
 .equ Adr_NX500_xpec3_range67, 0x0017C02C
 .equ Adr_NX500_xpec0_timer0, 0x00170030
 .equ Adr_NX500_xpec1_timer0, 0x00174030
 .equ Adr_NX500_xpec2_timer0, 0x00178030
 .equ Adr_NX500_xpec3_timer0, 0x0017C030
 .equ Adr_NX500_xpec0_timer1, 0x00170034
 .equ Adr_NX500_xpec1_timer1, 0x00174034
 .equ Adr_NX500_xpec2_timer1, 0x00178034
 .equ Adr_NX500_xpec3_timer1, 0x0017C034
 .equ Adr_NX500_xpec0_timer2, 0x00170038
 .equ Adr_NX500_xpec1_timer2, 0x00174038
 .equ Adr_NX500_xpec2_timer2, 0x00178038
 .equ Adr_NX500_xpec3_timer2, 0x0017C038
 .equ Adr_NX500_xpec0_timer3, 0x0017003C
 .equ Adr_NX500_xpec1_timer3, 0x0017403C
 .equ Adr_NX500_xpec2_timer3, 0x0017803C
 .equ Adr_NX500_xpec3_timer3, 0x0017C03C
 .equ Adr_NX500_xpec0_urx_count, 0x00170040
 .equ Adr_NX500_xpec1_urx_count, 0x00174040
 .equ Adr_NX500_xpec2_urx_count, 0x00178040
 .equ Adr_NX500_xpec3_urx_count, 0x0017C040
 .equ Adr_NX500_xpec0_utx_count, 0x00170044
 .equ Adr_NX500_xpec1_utx_count, 0x00174044
 .equ Adr_NX500_xpec2_utx_count, 0x00178044
 .equ Adr_NX500_xpec3_utx_count, 0x0017C044
 .equ Adr_NX500_xpec0_xpec_pc, 0x00170048
 .equ Adr_NX500_xpec1_xpec_pc, 0x00174048
 .equ Adr_NX500_xpec2_xpec_pc, 0x00178048
 .equ Adr_NX500_xpec3_xpec_pc, 0x0017C048
 .equ Adr_NX500_xpec0_zero, 0x0017004C
 .equ Adr_NX500_xpec1_zero, 0x0017404C
 .equ Adr_NX500_xpec2_zero, 0x0017804C
 .equ Adr_NX500_xpec3_zero, 0x0017C04C
 .equ Adr_NX500_xpec0_xpec_statcfg, 0x00170050
 .equ Adr_NX500_xpec1_xpec_statcfg, 0x00174050
 .equ Adr_NX500_xpec2_xpec_statcfg, 0x00178050
 .equ Adr_NX500_xpec3_xpec_statcfg, 0x0017C050
 .equ Adr_NX500_xpec0_ec_maska, 0x00170054
 .equ Adr_NX500_xpec1_ec_maska, 0x00174054
 .equ Adr_NX500_xpec2_ec_maska, 0x00178054
 .equ Adr_NX500_xpec3_ec_maska, 0x0017C054
 .equ Adr_NX500_xpec0_ec_maskb, 0x00170058
 .equ Adr_NX500_xpec1_ec_maskb, 0x00174058
 .equ Adr_NX500_xpec2_ec_maskb, 0x00178058
 .equ Adr_NX500_xpec3_ec_maskb, 0x0017C058
 .equ Adr_NX500_xpec0_ec_mask0, 0x0017005C
 .equ Adr_NX500_xpec1_ec_mask0, 0x0017405C
 .equ Adr_NX500_xpec2_ec_mask0, 0x0017805C
 .equ Adr_NX500_xpec3_ec_mask0, 0x0017C05C
 .equ Adr_NX500_xpec0_ec_mask1, 0x00170060
 .equ Adr_NX500_xpec1_ec_mask1, 0x00174060
 .equ Adr_NX500_xpec2_ec_mask1, 0x00178060
 .equ Adr_NX500_xpec3_ec_mask1, 0x0017C060
 .equ Adr_NX500_xpec0_ec_mask2, 0x00170064
 .equ Adr_NX500_xpec1_ec_mask2, 0x00174064
 .equ Adr_NX500_xpec2_ec_mask2, 0x00178064
 .equ Adr_NX500_xpec3_ec_mask2, 0x0017C064
 .equ Adr_NX500_xpec0_ec_mask3, 0x00170068
 .equ Adr_NX500_xpec1_ec_mask3, 0x00174068
 .equ Adr_NX500_xpec2_ec_mask3, 0x00178068
 .equ Adr_NX500_xpec3_ec_mask3, 0x0017C068
 .equ Adr_NX500_xpec0_ec_mask4, 0x0017006C
 .equ Adr_NX500_xpec1_ec_mask4, 0x0017406C
 .equ Adr_NX500_xpec2_ec_mask4, 0x0017806C
 .equ Adr_NX500_xpec3_ec_mask4, 0x0017C06C
 .equ Adr_NX500_xpec0_ec_mask5, 0x00170070
 .equ Adr_NX500_xpec1_ec_mask5, 0x00174070
 .equ Adr_NX500_xpec2_ec_mask5, 0x00178070
 .equ Adr_NX500_xpec3_ec_mask5, 0x0017C070
 .equ Adr_NX500_xpec0_ec_mask6, 0x00170074
 .equ Adr_NX500_xpec1_ec_mask6, 0x00174074
 .equ Adr_NX500_xpec2_ec_mask6, 0x00178074
 .equ Adr_NX500_xpec3_ec_mask6, 0x0017C074
 .equ Adr_NX500_xpec0_ec_mask7, 0x00170078
 .equ Adr_NX500_xpec1_ec_mask7, 0x00174078
 .equ Adr_NX500_xpec2_ec_mask7, 0x00178078
 .equ Adr_NX500_xpec3_ec_mask7, 0x0017C078
 .equ Adr_NX500_xpec0_ec_mask8, 0x0017007C
 .equ Adr_NX500_xpec1_ec_mask8, 0x0017407C
 .equ Adr_NX500_xpec2_ec_mask8, 0x0017807C
 .equ Adr_NX500_xpec3_ec_mask8, 0x0017C07C
 .equ Adr_NX500_xpec0_ec_mask9, 0x00170080
 .equ Adr_NX500_xpec1_ec_mask9, 0x00174080
 .equ Adr_NX500_xpec2_ec_mask9, 0x00178080
 .equ Adr_NX500_xpec3_ec_mask9, 0x0017C080
 .equ Adr_NX500_xpec0_timer4, 0x00170084
 .equ Adr_NX500_xpec1_timer4, 0x00174084
 .equ Adr_NX500_xpec2_timer4, 0x00178084
 .equ Adr_NX500_xpec3_timer4, 0x0017C084
 .equ Adr_NX500_xpec0_timer5, 0x00170088
 .equ Adr_NX500_xpec1_timer5, 0x00174088
 .equ Adr_NX500_xpec2_timer5, 0x00178088
 .equ Adr_NX500_xpec3_timer5, 0x0017C088
 .equ Adr_NX500_xpec0_irq, 0x0017008C
 .equ Adr_NX500_xpec1_irq, 0x0017408C
 .equ Adr_NX500_xpec2_irq, 0x0017808C
 .equ Adr_NX500_xpec3_irq, 0x0017C08C
 .equ Adr_NX500_xpec0_xpec_systime_ns, 0x00170090
 .equ Adr_NX500_xpec1_xpec_systime_ns, 0x00174090
 .equ Adr_NX500_xpec2_xpec_systime_ns, 0x00178090
 .equ Adr_NX500_xpec3_xpec_systime_ns, 0x0017C090
 .equ Adr_NX500_xpec0_fifo_data, 0x00170094
 .equ Adr_NX500_xpec1_fifo_data, 0x00174094
 .equ Adr_NX500_xpec2_fifo_data, 0x00178094
 .equ Adr_NX500_xpec3_fifo_data, 0x0017C094
 .equ Adr_NX500_xpec0_xpec_systime_s, 0x00170098
 .equ Adr_NX500_xpec1_xpec_systime_s, 0x00174098
 .equ Adr_NX500_xpec2_xpec_systime_s, 0x00178098
 .equ Adr_NX500_xpec3_xpec_systime_s, 0x0017C098
 .equ Adr_NX500_xpec0_xpec_adc, 0x0017009C
 .equ Adr_NX500_xpec1_xpec_adc, 0x0017409C
 .equ Adr_NX500_xpec2_xpec_adc, 0x0017809C
 .equ Adr_NX500_xpec3_xpec_adc, 0x0017C09C
 .equ Adr_NX500_xpec0_xpec_sr0, 0x001700A0
 .equ Adr_NX500_xpec1_xpec_sr0, 0x001740A0
 .equ Adr_NX500_xpec2_xpec_sr0, 0x001780A0
 .equ Adr_NX500_xpec3_xpec_sr0, 0x0017C0A0
 .equ Adr_NX500_xpec0_xpec_sr1, 0x001700A4
 .equ Adr_NX500_xpec1_xpec_sr1, 0x001740A4
 .equ Adr_NX500_xpec2_xpec_sr1, 0x001780A4
 .equ Adr_NX500_xpec3_xpec_sr1, 0x0017C0A4
 .equ Adr_NX500_xpec0_xpec_sr2, 0x001700A8
 .equ Adr_NX500_xpec1_xpec_sr2, 0x001740A8
 .equ Adr_NX500_xpec2_xpec_sr2, 0x001780A8
 .equ Adr_NX500_xpec3_xpec_sr2, 0x0017C0A8
 .equ Adr_NX500_xpec0_xpec_sr3, 0x001700AC
 .equ Adr_NX500_xpec1_xpec_sr3, 0x001740AC
 .equ Adr_NX500_xpec2_xpec_sr3, 0x001780AC
 .equ Adr_NX500_xpec3_xpec_sr3, 0x0017C0AC
 .equ Adr_NX500_xpec0_xpec_sr4, 0x001700B0
 .equ Adr_NX500_xpec1_xpec_sr4, 0x001740B0
 .equ Adr_NX500_xpec2_xpec_sr4, 0x001780B0
 .equ Adr_NX500_xpec3_xpec_sr4, 0x0017C0B0
 .equ Adr_NX500_xpec0_xpec_sr5, 0x001700B4
 .equ Adr_NX500_xpec1_xpec_sr5, 0x001740B4
 .equ Adr_NX500_xpec2_xpec_sr5, 0x001780B4
 .equ Adr_NX500_xpec3_xpec_sr5, 0x0017C0B4
 .equ Adr_NX500_xpec0_xpec_sr6, 0x001700B8
 .equ Adr_NX500_xpec1_xpec_sr6, 0x001740B8
 .equ Adr_NX500_xpec2_xpec_sr6, 0x001780B8
 .equ Adr_NX500_xpec3_xpec_sr6, 0x0017C0B8
 .equ Adr_NX500_xpec0_xpec_sr7, 0x001700BC
 .equ Adr_NX500_xpec1_xpec_sr7, 0x001740BC
 .equ Adr_NX500_xpec2_xpec_sr7, 0x001780BC
 .equ Adr_NX500_xpec3_xpec_sr7, 0x0017C0BC
 .equ Adr_NX500_xpec0_xpec_sr8, 0x001700C0
 .equ Adr_NX500_xpec1_xpec_sr8, 0x001740C0
 .equ Adr_NX500_xpec2_xpec_sr8, 0x001780C0
 .equ Adr_NX500_xpec3_xpec_sr8, 0x0017C0C0
 .equ Adr_NX500_xpec0_xpec_sr9, 0x001700C4
 .equ Adr_NX500_xpec1_xpec_sr9, 0x001740C4
 .equ Adr_NX500_xpec2_xpec_sr9, 0x001780C4
 .equ Adr_NX500_xpec3_xpec_sr9, 0x0017C0C4
 .equ Adr_NX500_xpec0_xpec_sr10, 0x001700C8
 .equ Adr_NX500_xpec1_xpec_sr10, 0x001740C8
 .equ Adr_NX500_xpec2_xpec_sr10, 0x001780C8
 .equ Adr_NX500_xpec3_xpec_sr10, 0x0017C0C8
 .equ Adr_NX500_xpec0_xpec_sr11, 0x001700CC
 .equ Adr_NX500_xpec1_xpec_sr11, 0x001740CC
 .equ Adr_NX500_xpec2_xpec_sr11, 0x001780CC
 .equ Adr_NX500_xpec3_xpec_sr11, 0x0017C0CC
 .equ Adr_NX500_xpec0_xpec_sr12, 0x001700D0
 .equ Adr_NX500_xpec1_xpec_sr12, 0x001740D0
 .equ Adr_NX500_xpec2_xpec_sr12, 0x001780D0
 .equ Adr_NX500_xpec3_xpec_sr12, 0x0017C0D0
 .equ Adr_NX500_xpec0_xpec_sr13, 0x001700D4
 .equ Adr_NX500_xpec1_xpec_sr13, 0x001740D4
 .equ Adr_NX500_xpec2_xpec_sr13, 0x001780D4
 .equ Adr_NX500_xpec3_xpec_sr13, 0x0017C0D4
 .equ Adr_NX500_xpec0_xpec_sr14, 0x001700D8
 .equ Adr_NX500_xpec1_xpec_sr14, 0x001740D8
 .equ Adr_NX500_xpec2_xpec_sr14, 0x001780D8
 .equ Adr_NX500_xpec3_xpec_sr14, 0x0017C0D8
 .equ Adr_NX500_xpec0_xpec_sr15, 0x001700DC
 .equ Adr_NX500_xpec1_xpec_sr15, 0x001740DC
 .equ Adr_NX500_xpec2_xpec_sr15, 0x001780DC
 .equ Adr_NX500_xpec3_xpec_sr15, 0x0017C0DC
 .equ Adr_NX500_xpec0_statcfg0, 0x001700E0
 .equ Adr_NX500_xpec1_statcfg0, 0x001740E0
 .equ Adr_NX500_xpec2_statcfg0, 0x001780E0
 .equ Adr_NX500_xpec3_statcfg0, 0x0017C0E0
 .equ Adr_NX500_xpec0_statcfg1, 0x001700E4
 .equ Adr_NX500_xpec1_statcfg1, 0x001740E4
 .equ Adr_NX500_xpec2_statcfg1, 0x001780E4
 .equ Adr_NX500_xpec3_statcfg1, 0x0017C0E4
 .equ Adr_NX500_xpec0_statcfg2, 0x001700E8
 .equ Adr_NX500_xpec1_statcfg2, 0x001740E8
 .equ Adr_NX500_xpec2_statcfg2, 0x001780E8
 .equ Adr_NX500_xpec3_statcfg2, 0x0017C0E8
 .equ Adr_NX500_xpec0_statcfg3, 0x001700EC
 .equ Adr_NX500_xpec1_statcfg3, 0x001740EC
 .equ Adr_NX500_xpec2_statcfg3, 0x001780EC
 .equ Adr_NX500_xpec3_statcfg3, 0x0017C0EC
 .equ Adr_NX500_xpec0_urtx0, 0x001700F0
 .equ Adr_NX500_xpec1_urtx0, 0x001740F0
 .equ Adr_NX500_xpec2_urtx0, 0x001780F0
 .equ Adr_NX500_xpec3_urtx0, 0x0017C0F0
 .equ Adr_NX500_xpec0_urtx1, 0x001700F4
 .equ Adr_NX500_xpec1_urtx1, 0x001740F4
 .equ Adr_NX500_xpec2_urtx1, 0x001780F4
 .equ Adr_NX500_xpec3_urtx1, 0x0017C0F4
 .equ Adr_NX500_xpec0_urtx2, 0x001700F8
 .equ Adr_NX500_xpec1_urtx2, 0x001740F8
 .equ Adr_NX500_xpec2_urtx2, 0x001780F8
 .equ Adr_NX500_xpec3_urtx2, 0x0017C0F8
 .equ Adr_NX500_xpec0_urtx3, 0x001700FC
 .equ Adr_NX500_xpec1_urtx3, 0x001740FC
 .equ Adr_NX500_xpec2_urtx3, 0x001780FC
 .equ Adr_NX500_xpec3_urtx3, 0x0017C0FC
 .equ Adr_NX500_xpec0_xpu_hold_pc, 0x00170100
 .equ Adr_NX500_xpec1_xpu_hold_pc, 0x00174100
 .equ Adr_NX500_xpec2_xpu_hold_pc, 0x00178100
 .equ Adr_NX500_xpec3_xpu_hold_pc, 0x0017C100
 .equ Adr_NX500_xpec0_ram_start, 0x00172000
 .equ Adr_NX500_xpec1_ram_start, 0x00176000
 .equ Adr_NX500_xpec2_ram_start, 0x0017A000
 .equ Adr_NX500_xpec3_ram_start, 0x0017E000

@ =====================================================================
@
@ Area xc_extbus_sel
 .equ Addr_NX500_xc_extbus_sel, 0x00180000
@
@ =====================================================================
 .equ Adr_NX500_xc_extbus_sel_xc_extbus_sel_ctrl, 0x00180000
 .equ Adr_NX500_xc_extbus_sel_ctrl, 0x00180000

@ =====================================================================
@
@ Area vic
 .equ Addr_NX500_vic, 0x001FF000
@
@ =====================================================================
 .equ Adr_NX500_vic_vic_irq_status, 0x001FF000
 .equ Adr_NX500_vic_irq_status, 0x001FF000
 .equ Adr_NX500_vic_vic_fiq_status, 0x001FF004
 .equ Adr_NX500_vic_fiq_status, 0x001FF004
 .equ Adr_NX500_vic_vic_raw_intr, 0x001FF008
 .equ Adr_NX500_vic_raw_intr, 0x001FF008
 .equ Adr_NX500_vic_vic_int_select, 0x001FF00C
 .equ Adr_NX500_vic_int_select, 0x001FF00C
 .equ Adr_NX500_vic_vic_int_enable, 0x001FF010
 .equ Adr_NX500_vic_int_enable, 0x001FF010
 .equ Adr_NX500_vic_vic_int_enclear, 0x001FF014
 .equ Adr_NX500_vic_int_enclear, 0x001FF014
 .equ Adr_NX500_vic_vic_softint, 0x001FF018
 .equ Adr_NX500_vic_softint, 0x001FF018
 .equ Adr_NX500_vic_vic_softint_clear, 0x001FF01C
 .equ Adr_NX500_vic_softint_clear, 0x001FF01C
 .equ Adr_NX500_vic_vic_protection, 0x001FF020
 .equ Adr_NX500_vic_protection, 0x001FF020
 .equ Adr_NX500_vic_vic_vect_addr, 0x001FF030
 .equ Adr_NX500_vic_vect_addr, 0x001FF030
 .equ Adr_NX500_vic_vic_def_vect_addr, 0x001FF034
 .equ Adr_NX500_vic_def_vect_addr, 0x001FF034
 .equ Adr_NX500_vic_vic_vect_addr0, 0x001FF100
 .equ Adr_NX500_vic_vect_addr0, 0x001FF100
 .equ Adr_NX500_vic_vic_vect_addr1, 0x001FF104
 .equ Adr_NX500_vic_vect_addr1, 0x001FF104
 .equ Adr_NX500_vic_vic_vect_addr2, 0x001FF108
 .equ Adr_NX500_vic_vect_addr2, 0x001FF108
 .equ Adr_NX500_vic_vic_vect_addr3, 0x001FF10C
 .equ Adr_NX500_vic_vect_addr3, 0x001FF10C
 .equ Adr_NX500_vic_vic_vect_addr4, 0x001FF110
 .equ Adr_NX500_vic_vect_addr4, 0x001FF110
 .equ Adr_NX500_vic_vic_vect_addr5, 0x001FF114
 .equ Adr_NX500_vic_vect_addr5, 0x001FF114
 .equ Adr_NX500_vic_vic_vect_addr6, 0x001FF118
 .equ Adr_NX500_vic_vect_addr6, 0x001FF118
 .equ Adr_NX500_vic_vic_vect_addr7, 0x001FF11C
 .equ Adr_NX500_vic_vect_addr7, 0x001FF11C
 .equ Adr_NX500_vic_vic_vect_addr8, 0x001FF120
 .equ Adr_NX500_vic_vect_addr8, 0x001FF120
 .equ Adr_NX500_vic_vic_vect_addr9, 0x001FF124
 .equ Adr_NX500_vic_vect_addr9, 0x001FF124
 .equ Adr_NX500_vic_vic_vect_addr10, 0x001FF128
 .equ Adr_NX500_vic_vect_addr10, 0x001FF128
 .equ Adr_NX500_vic_vic_vect_addr11, 0x001FF12C
 .equ Adr_NX500_vic_vect_addr11, 0x001FF12C
 .equ Adr_NX500_vic_vic_vect_addr12, 0x001FF130
 .equ Adr_NX500_vic_vect_addr12, 0x001FF130
 .equ Adr_NX500_vic_vic_vect_addr13, 0x001FF134
 .equ Adr_NX500_vic_vect_addr13, 0x001FF134
 .equ Adr_NX500_vic_vic_vect_addr14, 0x001FF138
 .equ Adr_NX500_vic_vect_addr14, 0x001FF138
 .equ Adr_NX500_vic_vic_vect_addr15, 0x001FF13C
 .equ Adr_NX500_vic_vect_addr15, 0x001FF13C
 .equ Adr_NX500_vic_vic_vect_cntl0, 0x001FF200
 .equ Adr_NX500_vic_vect_cntl0, 0x001FF200
 .equ Adr_NX500_vic_vic_vect_cntl1, 0x001FF204
 .equ Adr_NX500_vic_vect_cntl1, 0x001FF204
 .equ Adr_NX500_vic_vic_vect_cntl2, 0x001FF208
 .equ Adr_NX500_vic_vect_cntl2, 0x001FF208
 .equ Adr_NX500_vic_vic_vect_cntl3, 0x001FF20C
 .equ Adr_NX500_vic_vect_cntl3, 0x001FF20C
 .equ Adr_NX500_vic_vic_vect_cntl4, 0x001FF210
 .equ Adr_NX500_vic_vect_cntl4, 0x001FF210
 .equ Adr_NX500_vic_vic_vect_cntl5, 0x001FF214
 .equ Adr_NX500_vic_vect_cntl5, 0x001FF214
 .equ Adr_NX500_vic_vic_vect_cntl6, 0x001FF218
 .equ Adr_NX500_vic_vect_cntl6, 0x001FF218
 .equ Adr_NX500_vic_vic_vect_cntl7, 0x001FF21C
 .equ Adr_NX500_vic_vect_cntl7, 0x001FF21C
 .equ Adr_NX500_vic_vic_vect_cntl8, 0x001FF220
 .equ Adr_NX500_vic_vect_cntl8, 0x001FF220
 .equ Adr_NX500_vic_vic_vect_cntl9, 0x001FF224
 .equ Adr_NX500_vic_vect_cntl9, 0x001FF224
 .equ Adr_NX500_vic_vic_vect_cntl10, 0x001FF228
 .equ Adr_NX500_vic_vect_cntl10, 0x001FF228
 .equ Adr_NX500_vic_vic_vect_cntl11, 0x001FF22C
 .equ Adr_NX500_vic_vect_cntl11, 0x001FF22C
 .equ Adr_NX500_vic_vic_vect_cntl12, 0x001FF230
 .equ Adr_NX500_vic_vect_cntl12, 0x001FF230
 .equ Adr_NX500_vic_vic_vect_cntl13, 0x001FF234
 .equ Adr_NX500_vic_vect_cntl13, 0x001FF234
 .equ Adr_NX500_vic_vic_vect_cntl14, 0x001FF238
 .equ Adr_NX500_vic_vect_cntl14, 0x001FF238
 .equ Adr_NX500_vic_vic_vect_cntl15, 0x001FF23C
 .equ Adr_NX500_vic_vect_cntl15, 0x001FF23C

@ =====================================================================
@
@ Area ahbls4
 .equ Addr_NX500_ahbls4, 0x00200000
@
@ =====================================================================
@ =====================================================================
@
@ Area boot_rom
 .equ Addr_NX500_boot_rom, 0x00200000
@
@ =====================================================================
 .equ Adr_NX500_boot_rom_boot_rom_base, 0x00200000
 .equ Adr_NX500_boot_rom_base, 0x00200000
 .equ Adr_NX500_boot_rom_boot_rom_end, 0x00207FFC
 .equ Adr_NX500_boot_rom_end, 0x00207FFC

@ =====================================================================
@
@ Area backup_ram
 .equ Addr_NX500_backup_ram, 0x00300000
@
@ =====================================================================
 .equ Adr_NX500_backup_ram_backup_ram_base, 0x00300000
 .equ Adr_NX500_backup_ram_base, 0x00300000
 .equ Adr_NX500_backup_ram_backup_ram_end, 0x00303FFC
 .equ Adr_NX500_backup_ram_end, 0x00303FFC

@ =====================================================================
@
@ Area dTCM
 .equ Addr_NX500_dTCM, 0x10000000
@
@ =====================================================================
 .equ Adr_NX500_dTCM_dTCM_base, 0x10000000
 .equ Adr_NX500_dTCM_base, 0x10000000
 .equ Adr_NX500_dTCM_dTCM_end, 0x10001FFC
 .equ Adr_NX500_dTCM_end, 0x10001FFC

@ =====================================================================
@
@ Area pci_ahbls6
 .equ Addr_NX500_pci_ahbls6, 0x20000000
@
@ =====================================================================
 .equ Adr_NX500_pci_ahbls6_pci_ahbls6_BASE, 0x20000000
 .equ Adr_NX500_pci_ahbls6_BASE, 0x20000000

@ =====================================================================
@
@ Area extmemory_ahbls7
 .equ Addr_NX500_extmemory_ahbls7, 0x80000000
@
@ =====================================================================
@ =====================================================================
@
@ Area sdram
 .equ Addr_NX500_sdram, 0x80000000
@
@ =====================================================================
 .equ Adr_NX500_sdram_sdram_base, 0x80000000
 .equ Adr_NX500_sdram_base, 0x80000000
 .equ Adr_NX500_sdram_sdram_end, 0xBFFFFFFC
 .equ Adr_NX500_sdram_end, 0xBFFFFFFC

@ =====================================================================
@
@ Area extsram
 .equ Addr_NX500_extsram, 0xC0000000
@
@ =====================================================================
@ =====================================================================
@
@ Area extsram0
 .equ Addr_NX500_extsram0, 0xC0000000
@
@ =====================================================================
 .equ Adr_NX500_extsram0_extsram0_base, 0xC0000000
 .equ Adr_NX500_extsram0_base, 0xC0000000
 .equ Adr_NX500_extsram0_extsram0_end, 0xC7FFFFFC
 .equ Adr_NX500_extsram0_end, 0xC7FFFFFC

@ =====================================================================
@
@ Area extsram1
 .equ Addr_NX500_extsram1, 0xC8000000
@
@ =====================================================================
 .equ Adr_NX500_extsram1_extsram1_base, 0xC8000000
 .equ Adr_NX500_extsram1_base         , 0xC8000000
 .equ Adr_NX500_extsram1_extsram1_end , 0xCFFFFFFC
 .equ Adr_NX500_extsram1_end          , 0xCFFFFFFC

@ =====================================================================
@
@ Area extsram2
 .equ Addr_NX500_extsram2, 0xD0000000
@
@ =====================================================================
 .equ Adr_NX500_extsram2_extsram2_base, 0xD0000000
 .equ Adr_NX500_extsram2_base         , 0xD0000000
 .equ Adr_NX500_extsram2_extsram2_end , 0xD7FFFFFC
 .equ Adr_NX500_extsram2_end          , 0xD7FFFFFC

@ =====================================================================
@
@ Area extsram3
 .equ Addr_NX500_extsram3, 0xD8000000
@
@ =====================================================================
@ =====================================================================
@
@ Area intlogic_mirror
 .equ Addr_NX500_intlogic_mirror, 0xFFF00000
@
@ =====================================================================


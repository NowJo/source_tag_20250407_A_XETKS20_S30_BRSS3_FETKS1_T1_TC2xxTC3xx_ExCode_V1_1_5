/*
################################################################################
#                                                                              #
#    TC29xED / ETKS20                            |   ETAS GmbH                 #
#    ETK INCA Integration Example                |   Stuttgart Feuerbach       #
#    Project specific linker script                                            #
#    For Demonstration Purpose Only              |   All rights reserved       #
#                                                |   Alle Rechte vorbehalten   #
#                                                                              #
################################################################################
*/

/******************************************************************************/
/* Copyright and Legal Disclaimer:                                            */
/* This Code example was provided by ETAS GmbH, Stuttgart                     */
/*                                                                            */
/* ETAS will not be held reliable for any usage of this code,                 */
/* this code is provided as example code only, and not tested and released    */
/* for production code.                                                       */
/* ETAS will not guarantee any functional part of this code in any            */
/* environment together with any ETAS tools. ETAS will not guarantee that     */
/* this code works together with any future versions of ETAS tools.           */
/*                                                                            */
/* ETAS will not guarantee that this code is free of rights of third parties. */
/*                                                                            */
/* You are hereby granted the right to use his code as a example for your own */
/* ECU implementation together with ETAS Tools. No licenses are granted by    */
/* implication or otherwise under any patents or trademarks of ETAS GmbH.     */
/* This software is provided on an "AS IS" basis and without warranty.        */
/*                                                                            */
/* You are not allowed to give these code to third parties without            */
/* the written permission of ETAS GmbH.                                       */
/*                                                                            */
/* To the maximum extent permitted by applicable law,                         */
/* ETAS GmbH DISCLAIMS ALL WARRANTIES WHETHER EXPRESS OR IMPLIED,             */
/* INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A           */
/* PARTICULAR PURPOSE AND ANY WARRANTY AGAINST INFRINGEMENT WITH REGARD       */
/* TO THE SOFTWARE (INCLUDING ANY MODIFIED VERSIONS THEREOF)                  */
/* AND ANY ACCOMPANYING WRITTEN MATERIALS.                                    */
/*                                                                            */
/*                                                                            */
/* To the maximum extent permitted by applicable law,                         */
/* IN NO EVENT SHALL ETAS BE LIABLE FOR ANY                                   */
/* DAMAGES WHATSOEVER (INCLUDING WITHOUT LIMITATION, DAMAGES FOR LOSS OF      */
/* BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION,     */
/* OR OTHER PECUNIARY LOSS)                                                   */
/* ARISING OF THE USE OR INABILITY TO USE THE SOFTWARE.                       */
/*                                                                            */
/*                                                                            */
/* ETAS GmbH assumes no responsibility for the maintenance                    */
/* and support of this software                                               */
/*                                                                            */
/*                                                                            */
/*  COPYRIGHT (c) ETAS GmbH 2012-2018                                         */
/*  All Rights Reserved                                                       */
/******************************************************************************/



// Basic placements
#define RESET             0xA0000020
#define A1_START_ADDRESS  0xA0001000
#define LIBRARIES         0xA0002000
#define INTTAB            0xA0006000
#define TRAPTAB           0xA0008000
#define A0_START_ADDRESS  0xD8000000
#define CSA_START_TC0     0xD001B000
#define CSA_TC0             64
#define CSA_START_TC1     0xD0020000
#define CSA_TC1             0
#define CSA_START_TC2     0xD0020000
#define CSA_TC2             0
#define XVWBUF          256

#define ISTACK_TC0          1K      /* interrupt stack size tc0 */
#define USTACK_TC0          16K     /* user stack size tc0 */
#define ISTACK_TC1          1K      /* interrupt stack size tc1 */
#define USTACK_TC1          1K      /* user stack size tc1 */
#define ISTACK_TC2          1K      /* interrupt stack size tc2 */
#define USTACK_TC2          1K      /* user stack size tc2 */

// force the stack to be quad word aligned
#define __TC112_COR16__

/* Including a modified lsl file where the Program Flash area 0x80000000 is declared as reserved instead of the 0xA0000000 area. */
#include "tc29x_NON_cached.lsl"


//*** emulation RAM: BF00 0000h - BF1F FFFFh (2048kB) ***
//*** EMURAM section ***

memory EMURAM_CODECHECK_PATTERN
{
   mau = 8;
   size = 64k;
   type = reserved ram;
   map (dest=bus:mpe:sri, dest_offset=0xBF1F0000, size=64k);
}


/* With the :vtc: core name we get absolute addressing mode. */
section_layout mpe:vtc:linear
{
  /* 0xA0000000--0xA000001F is reserved for the Boot Header Mode Structure which allows to specifiy a special starting point for the program. */
  /* !! Without at least one valid BMHD the TC2xx variants after the TC2D5 won't boot !! */

  // Boot Mode Header 0
  group BMHD0_DATA (ordered, run_addr=0xA0000000)
  {
    // We want to put a BMHD in the 0xA0000000 specific area.
    // This defines : Internal start from flash at the address 0xA0000020 - Mode selection by HWCFG pins is enabled

    select "*.BMHD0";

  }

  // BMHD2
  // The Boot Mode Header 2 is added as a safety.
  // If this BMHD is reached it will enter an infinite while loop as a protection.
  // This BMHD will only be used if none of the first two BMHD are set up correctly.
  group BMHD2_DATA (ordered, run_addr=0xA000FFE0)
  {
    select "*.BMHD2";
  }

  // BMHD2 specific startup code
  // Defines an infinite while loop that will be entered if BMHD2 is used during the startup sequence.
  // This BMHD will only be used if none of the first two BMHD are set up correctly.
  group BMHD2_CODE (ordered, run_addr=0xA0010000)
  {
    select "*.special_Startup_BMHD2";
  }

  // Data Freeze validate/invalidate patterns saved in the code in order to compare it with the real pattern
  // Should be kept in a memory segment defined as CODE in the A2L
  group DF_ValidatePattern_SaveinCode (ordered, run_addr=0xA000B000)
  {
    select "*.DataFreeze_Validate_Pattern_Save_in_Code";
    select "*.DataFreeze_Invalidate_Pattern_Save_in_Code";
  }

  // FLASH KEY Pattern
  group flash_key (ordered, run_addr=0xA00FEFFC)
  {
    select "*.FLASH_KEY";
  }

  // Data Freeze validate pattern - defined also in the A2L file in a reserved memory segment
  // This value is overwritten after a successfull Data Freeze.
  group DF_ValidatePattern (ordered, run_addr=0xA00FFF00)
  {
    select "*.DataFreeze_Validate_Pattern";
  }


  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // The Program flash sector S22 starts at 0xA00E0000 for the TC27x.
  // It is STRONGLY recommended to not define CODE and DATA segments at the same time in one logical sector,
  // as Data Freeze will also erase the Code part of the flash sector during the flashing of the data segment.
  // If Data Freeze has an error and the flashing was not successfull the Code part may be missing.

  // Cal Params
  group CAL_PARAM (ordered, run_addr=0xA0164000)
  {
    select ".rodata.CAL_PARAM";
    select "*.CAL_PARAM";
    select "*.measure_parameters";
    select "*.ETK_Code_checkMemClass";
    select ".rodata.*.CAL_PARAM";
  }

  // internal RAM
  group TRACE_TRIGGER (ordered, run_addr = mem:mpe:dspr0[0x20])
  {
    select "*.ETK_Trace_TriggerByValue";  // This contains the Trace trigger and should be 4 byte aligned.
  }

  group IRAM (ordered, run_addr = mem:mpe:dspr0[0x30])
  {
    select "*.IRAM";
    select "*.ERAM";       //no external RAM available, so section is located in internal RAM
    select "*.ETK_ColdStart_MemClass";
    select "*.ETK_RAM";     //no external ETK RAM available, so section is located in internal RAM
    select "*.WriteToRAM";
    select "*.ETK_TRIGGER_ID";

    select "*.code_checkMemClassRam";
    select "*.CalWakeUp_MemClassRam";
    select "*.NVRAM";
    select "*csa_01";
    select "*dT_measurement";
    select "*measure_variables";
    select "*measure_dummies";
    select "*.VERSION_XYZ";
    select "*.IRAM_CPU0toDSPR0_measure_variables_performance_bss_8BAlign";
    select "*.IRAM_CPU0toDSPR0_measure_variables_performance_bss";
    select "*.IRAM_CPU0_measure_variables_performance_bss_1BAlign";
    select "*.measureCalibParam";
  }

  group distabTables (ordered, run_addr = mem:mpe:dspr0[0xC000])
  {
    select "*.ETK_DisTab_MemClass";
    select "*.ETK_DAQChnl_MemClass";
    select "*.ETK_BYPASS_RET_MemClass";
    select "*.ETK_DisTab17_EventList";
    select "*.ETK_DisTab17_EventConfigArea";
    select "*.ETK_DisTab17_EventOutputArea";
  }

  // adaptive Parameters
  group ADAP_PARAM (ordered, run_addr = mem:mpe:dspr0[0xE000])
  {
    select "*.ADAP_PARAM";
  }

  // bypass Offsets
  group BYP_OFFSETS (ordered, run_addr = mem:mpe:dspr0[0xF000])
  {
    select "*.BYP_OFFSETS_SER";
  }

  group DSPR1_ram (ordered, run_addr = mem:mpe:dspr1)
  {
    select "*.ERAM_CPU0toDSPR1_measure_variables_performance_bss_8BAlign";
    select "*.ERAM_CPU0toDSPR1_measure_variables_performance_bss";
  }

  group LMU_ram (ordered, run_addr = mem:mpe:lmuram[0x4000])
  {
    // first 0x4000 bytes are reserved for the Distab config and output areas
    select "*.ERAM_CPU0toLMU_measure_variables_performance_bss_8BAlign";
    select "*.ERAM_CPU0toLMU_measure_variables_performance_bss";
  }

  // EMURAM CodeCheck Pattern
  group EmuRAM_CodeCheck_Pattern(ordered, run_addr=mem:EMURAM_CODECHECK_PATTERN)
  {
    select "*.ETK_DataFreeze_Mailbox";
    select "*.code_checkMemClassEMURam";
    select "*.ETK_PageSwitch_MemClass";
    select "*.ETK_OMDTable_MemClass";

  }

  group CP_1 (ordered, run_addr=0xA0100800)
  {
    select "*.calibParam_01";
  }
  group CP_2 (ordered, run_addr=0xA0110000)
  {
    select "*.calibParam_02";
  }
  group CP_3 (ordered, run_addr=0xA0120000)
  {
    select "*.calibParam_03";
  }
  group CP_4 (ordered, run_addr=0xA0130000)
  {
    select "*.calibParam_04";
  }
  group CP_5 (ordered, run_addr=0xA0140000)
  {
    select "*.calibParam_05";
  }
  group CP_6 (ordered, run_addr=0xA0150000)
  {
    select "*.calibParam_06";
  }
  group CP_7 (ordered, run_addr=0xA0160000)
  {
    select "*.calibParam_07";
  }
  group CP_8 (ordered, run_addr=0xA0170000)
  {
    select "*.calibParam_08";
  }
  group CP_9 (ordered, run_addr=0xA0180000)
  {
    select "*.calibParam_09";
  }
  group CP_10 (ordered, run_addr=0xA0190000)
  {
    select "*.calibParam_10";
  }
  group CP_11 (ordered, run_addr=0xA01A0000)
  {
    select "*.calibParam_11";
  }
  group CP_12 (ordered, run_addr=0xA01B0000)
  {
    select "*.calibParam_12";
  }
  group CP_13 (ordered, run_addr=0xA01C0000)
  {
    select "*.calibParam_13";
  }
  group CP_14 (ordered, run_addr=0xA01D0000)
  {
    select "*.calibParam_14";
  }
  group CP_15 (ordered, run_addr=0xA01E0000)
  {
    select "*.calibParam_15";
  }
  group CP_16 (ordered, run_addr=0xA01F0000)
  {
    select "*.calibParam_16";
  }
  group CP_17 (ordered, run_addr=0xA0200000)
  {
    select "*.calibParam_17";
  }
  group CP_18 (ordered, run_addr=0xA0210000)
  {
    select "*.calibParam_18";
  }
  group CP_19 (ordered, run_addr=0xA0220000)
  {
    select "*.calibParam_19";
  }
  group CP_20 (ordered, run_addr=0xA0230000)
  {
    select "*.calibParam_20";
  }
  group CP_21 (ordered, run_addr=0xA0240000)
  {
    select "*.calibParam_21";
  }
  group CP_22 (ordered, run_addr=0xA0250000)
  {
    select "*.calibParam_22";
  }
  group CP_23 (ordered, run_addr=0xA0260000)
  {
    select "*.calibParam_23";
  }
  group CP_24 (ordered, run_addr=0xA0270000)
  {
    select "*.calibParam_24";
  }
  group CP_25 (ordered, run_addr=0xA0280000)
  {
    select "*.calibParam_25";
  }
  group CP_26 (ordered, run_addr=0xA0290000)
  {
    select "*.calibParam_26";
  }
  group CP_27 (ordered, run_addr=0xA02A0000)
  {
    select "*.calibParam_27";
  }
  group CP_28 (ordered, run_addr=0xA02B0000)
  {
    select "*.calibParam_28";
  }
  group CP_29 (ordered, run_addr=0xA02C0000)
  {
    select "*.calibParam_29";
  }
  group CP_30 (ordered, run_addr=0xA02D0000)
  {
    select "*.calibParam_30";
  }
  group CP_31 (ordered, run_addr=0xA02E0000)
  {
    select "*.calibParam_31";
  }
  group CP_32 (ordered, run_addr=0xA02F0000)
  {
    select "*.calibParam_32";
  }


}

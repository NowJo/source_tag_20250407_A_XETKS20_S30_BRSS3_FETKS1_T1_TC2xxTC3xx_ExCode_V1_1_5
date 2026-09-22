/*
################################################################################
#                                                                              #
#    ETK driver example                          |   ETAS GmbH                 #
#                                                |   Stuttgart Feuerbach       #
#    For Demonstration Purpose Only              |   All rights reserved       #
#    sample driver implementation                |   Alle Rechte vorbehalten   #
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
/*  COPYRIGHT (c) ETAS GmbH 2012-2015                                    */
/*  All Rights Reserved                                                       */
/******************************************************************************/

#include "ETK_Integration_Cfg.h"          /* for the configuration and register addresses */
// #include "../Base.h"                      /* for the register addresses */
#include "endinit.h"                      /* for the endinit protection */

/* Function used to allow access to ENDINIT protected registers. There are 4 different ENDINIT protection. 's' for the safety ENDINIT protection, the other are for the CPU ENDINIT protection. */
void unlock_wdtcon(char type){
  uint32 wcon0;              /* copies of WtachDogTimer register values */

  switch(type)
  {

    case 's': /* Safety WDT */
          wcon0 = SCU_WDTSCON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF1)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:0] must be written to 11110001 */
          SCU_WDTSCON0.U = wcon0; /* Write the password to WDT control to enable modifying */

          wcon0 = SCU_WDTSCON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF2)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:1] must be written to 1111001; [0] is user definable for the ENDINIT setting -> 0 access enabled and 1 to disable access to Endinit protected register. */
          SCU_WDTSCON0.U = wcon0; /* Write the modified settings to WDT control */

          /* read back ENDINIT and wait until it has been cleared */
          while (SCU_WDTSCON0.B.ENDINIT == 1) { ; }

          break;

    case '0': /* CPU 0 WDT */
          wcon0 = SCU_WDTCPU0CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF1)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:0] must be written to 11110001 */
          SCU_WDTCPU0CON0.U = wcon0; /* Write the password to WDT control to enable modifying */

          wcon0 = SCU_WDTCPU0CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF2)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:1] must be written to 1111001; [0] is user definable for the ENDINIT setting -> 0 access enabled and 1 to disable access to Endinit protected register. */
          //wcon0 = wcon0 | 0x00000000 ; /* Apply to desired mode change -> Access to Endinit-protected registers permitted */
          SCU_WDTCPU0CON0.U = wcon0; /* Write the modified settings to WDT control */

          /* read back ENDINIT and wait until it has been cleared */
          while (SCU_WDTCPU0CON0.B.ENDINIT == 1) { ; }

          break;

    case '1': /* CPU 1 WDT */
          wcon0 = SCU_WDTCPU1CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF1)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:0] must be written to 11110001 */
          SCU_WDTCPU1CON0.U = wcon0; /* Write the password to WDT control to enable modifying */

          wcon0 = SCU_WDTCPU1CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF2)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:1] must be written to 1111001; [0] is user definable for the ENDINIT setting -> 0 access enabled and 1 to disable access to Endinit protected register. */
          SCU_WDTCPU1CON0.U = wcon0; /* Write the modified settings to WDT control */

          /* read back ENDINIT and wait until it has been cleared */
          while (SCU_WDTCPU1CON0.B.ENDINIT == 1) { ; }

          break;

    case '2': /* CPU 2 WDT */
          wcon0 = SCU_WDTCPU2CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF1)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:0] must be written to 11110001 */
          SCU_WDTCPU2CON0.U = wcon0; /* Write the password to WDT control to enable modifying */

          wcon0 = SCU_WDTCPU2CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF2)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:1] must be written to 1111001; [0] is user definable for the ENDINIT setting -> 0 access enabled and 1 to disable access to Endinit protected register. */
          SCU_WDTCPU2CON0.U = wcon0; /* Write the modified settings to WDT control */

          /* read back ENDINIT and wait until it has been cleared */
          while (SCU_WDTCPU2CON0.B.ENDINIT == 1) { ; }

          break;

   #ifdef CPUTYPE_TC39X
    case '3': /* CPU 3 WDT */
          wcon0 = SCU_WDTCPU3CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF1)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:0] must be written to 11110001 */
          SCU_WDTCPU3CON0.U = wcon0; /* Write the password to WDT control to enable modifying */

          wcon0 = SCU_WDTCPU3CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF2)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:1] must be written to 1111001; [0] is user definable for the ENDINIT setting -> 0 access enabled and 1 to disable access to Endinit protected register. */
          SCU_WDTCPU3CON0.U = wcon0; /* Write the modified settings to WDT control */

          /* read back ENDINIT and wait until it has been cleared */
          while (SCU_WDTCPU3CON0.B.ENDINIT == 1) { ; }

          break;

    case '4': /* CPU 4 WDT */
          wcon0 = SCU_WDTCPU4CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF1)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:0] must be written to 11110001 */
          SCU_WDTCPU4CON0.U = wcon0; /* Write the password to WDT control to enable modifying */

          wcon0 = SCU_WDTCPU4CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF2)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:1] must be written to 1111001; [0] is user definable for the ENDINIT setting -> 0 access enabled and 1 to disable access to Endinit protected register. */
          SCU_WDTCPU4CON0.U = wcon0; /* Write the modified settings to WDT control */

          /* read back ENDINIT and wait until it has been cleared */
          while (SCU_WDTCPU4CON0.B.ENDINIT == 1) { ; }

          break;

    case '5': /* CPU 5 WDT */
          wcon0 = SCU_WDTCPU5CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF1)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:0] must be written to 11110001 */
          SCU_WDTCPU5CON0.U = wcon0; /* Write the password to WDT control to enable modifying */

          wcon0 = SCU_WDTCPU5CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF2)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:1] must be written to 1111001; [0] is user definable for the ENDINIT setting -> 0 access enabled and 1 to disable access to Endinit protected register. */
          SCU_WDTCPU5CON0.U = wcon0; /* Write the modified settings to WDT control */

          /* read back ENDINIT and wait until it has been cleared */
          while (SCU_WDTCPU5CON0.B.ENDINIT == 1) { ; }

          break;
    #endif // #ifdef CPUTYPE_TC39X

    default:
          break;
  }
}

/* Function used to reset access to ENDINIT protected registers, thus blocking it. There are 4 different ENDINIT protection. 's' for the safety ENDINIT protection, the other are for the CPU ENDINIT protection. */
void lock_wdtcon(char type){
  uint32 wcon0;              /* copies of WtachDogTimer register values */

  switch(type)
  {

    case 's': /* Safety WDT */
          wcon0 = SCU_WDTSCON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF1)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:0] must be written to 11110001 */
          SCU_WDTSCON0.U = wcon0; /* Write the password to WDT control to enable modifying */

          wcon0 = SCU_WDTSCON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF2)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:1] must be written to 1111001; [0] is user definable for the ENDINIT setting -> 0 access enabled and 1 to disable access to Endinit protected register. */
          wcon0 = wcon0 | 0x00000001 ; /* Apply to desired mode change -> Access to Endinit-protected registers not permitted */
          SCU_WDTSCON0.U = wcon0; /* Write the modified settings to WDT control */
          break;

    case '0': /* CPU 0 WDT */
          wcon0 = SCU_WDTCPU0CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF1)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:0] must be written to 11110001 */
          SCU_WDTCPU0CON0.U = wcon0; /* Write the password to WDT control to enable modifying */

          wcon0 = SCU_WDTCPU0CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF2)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:1] must be written to 1111001; [0] is user definable for the ENDINIT setting -> 0 access enabled and 1 to disable access to Endinit protected register. */
          wcon0 = wcon0 | 0x00000001 ; /* Apply to desired mode change -> Access to Endinit-protected registers not permitted */
          SCU_WDTCPU0CON0.U = wcon0; /* Write the modified settings to WDT control */
          break;

    case '1': /* CPU 1 WDT */
          wcon0 = SCU_WDTCPU1CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF1)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:0] must be written to 11110001 */
          SCU_WDTCPU1CON0.U = wcon0; /* Write the password to WDT control to enable modifying */

          wcon0 = SCU_WDTCPU1CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF2)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:1] must be written to 1111001; [0] is user definable for the ENDINIT setting -> 0 access enabled and 1 to disable access to Endinit protected register. */
          wcon0 = wcon0 | 0x00000001 ; /* Apply to desired mode change -> Access to Endinit-protected registers not permitted */
          SCU_WDTCPU1CON0.U = wcon0; /* Write the modified settings to WDT control */
          break;

    case '2': /* CPU 2 WDT */
          wcon0 = SCU_WDTCPU2CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF1)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:0] must be written to 11110001 */
          SCU_WDTCPU2CON0.U = wcon0; /* Write the password to WDT control to enable modifying */

          wcon0 = SCU_WDTCPU2CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF2)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:1] must be written to 1111001; [0] is user definable for the ENDINIT setting -> 0 access enabled and 1 to disable access to Endinit protected register. */
          wcon0 = wcon0 | 0x00000001 ; /* Apply to desired mode change -> Access to Endinit-protected registers not permitted */
          SCU_WDTCPU2CON0.U = wcon0; /* Write the modified settings to WDT control */
          break;

   #ifdef CPUTYPE_TC39X
    case '3': /* CPU 3 WDT */
          wcon0 = SCU_WDTCPU3CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF1)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:0] must be written to 11110001 */
          SCU_WDTCPU3CON0.U = wcon0; /* Write the password to WDT control to enable modifying */

          wcon0 = SCU_WDTCPU3CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF2)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:1] must be written to 1111001; [0] is user definable for the ENDINIT setting -> 0 access enabled and 1 to disable access to Endinit protected register. */
          wcon0 = wcon0 | 0x00000001 ; /* Apply to desired mode change -> Access to Endinit-protected registers not permitted */
          SCU_WDTCPU3CON0.U = wcon0; /* Write the modified settings to WDT control */
          break;

    case '4': /* CPU 4 WDT */
          wcon0 = SCU_WDTCPU4CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF1)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:0] must be written to 11110001 */
          SCU_WDTCPU4CON0.U = wcon0; /* Write the password to WDT control to enable modifying */

          wcon0 = SCU_WDTCPU4CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF2)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:1] must be written to 1111001; [0] is user definable for the ENDINIT setting -> 0 access enabled and 1 to disable access to Endinit protected register. */
          wcon0 = wcon0 | 0x00000001 ; /* Apply to desired mode change -> Access to Endinit-protected registers not permitted */
          SCU_WDTCPU4CON0.U = wcon0; /* Write the modified settings to WDT control */
          break;

    case '5': /* CPU 5 WDT */
          wcon0 = SCU_WDTCPU5CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF1)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:0] must be written to 11110001 */
          SCU_WDTCPU5CON0.U = wcon0; /* Write the password to WDT control to enable modifying */

          wcon0 = SCU_WDTCPU5CON0.U; /* read WDT Control 0 */
          wcon0 = ((wcon0 & 0xFFFFFF00) | (0xF2)); /* Calculate the password : [8:31]= current value of the WDTxCON0.PW and .REL fields; [7:1] must be written to 1111001; [0] is user definable for the ENDINIT setting -> 0 access enabled and 1 to disable access to Endinit protected register. */
          wcon0 = wcon0 | 0x00000001 ; /* Apply to desired mode change -> Access to Endinit-protected registers not permitted */
          SCU_WDTCPU5CON0.U = wcon0; /* Write the modified settings to WDT control */
          break;
   #endif // #ifdef CPUTYPE_TC39X

    default:
          break;
  }

}

/** This function can be called by the Tasking compiler during the startup if the following define is set in cstart.h :
#define _CALL_ENDINIT           1
It will be called when the endinit protection are disabled to allow specific register accesses. */
void _endinit(void)
{
  #ifdef PD_DEVICE

    /** The PD device does not posses EMEM RAM, thus we don't try to access it.
       The DSPR RAM (DSPR0 0x70004000-0x70006000 and DSPR1 0x60004000-0x60006000) is used for the calibration functionality instead. */

  #else // ED device
    /* We will initialize here the EMEM module to allow the later modifications of the EMEM_TILECONFIG register. */
    volatile unsigned long readback;

    /* 1st step - EMEM Module enable requested. */
    //unlock_wdtcon('0');  // not needed because this is called when the protections are disabled - disables the CPU 0 endinit protection
    EMEM_CLC.U = 0x0; // TC2D5ED Address 0xF90E6000
    readback = EMEM_CLC.U;
    //lock_wdtcon('0'); // not needed because this is called when the protections are disabled - enables the CPU 0 endinit protection - prevents the timeout

    /* 2nd step - Unlocking the EMEM module */
    /* We need to write the following three values (0b001, 0b011, and 0b111 in order in the STBULK field of the EMEM_SBRCTR to unlock the module. */
    EMEM_SBRCTR.U = 0x2; // TC27xED Address 0xF90E6034
    EMEM_SBRCTR.U = 0x6;
    EMEM_SBRCTR.U = 0xE;
    readback = EMEM_SBRCTR.U;
  #endif // #ifdef PD_DEVICE
}

/** This function can be called by the Tasking compiler during the startup if the following define is set in cstart.h :
#define __CALL_INIT           1
It will be called just before the main function is entered.
We are using it to disable the watchdog protection if the ENDINIT_PROTECTION_DISABLED define is set in ETK_Integration_Cfg.h,
as the Tasking startup code enables it during the startup.
 Even if the __WATCHDOG_DISABLE and __SAFETY_WATCHDOG_DISABLE defines are set the watchdog will only be disabled in the initial part of the startup code and then enabled again.
 As a workaround we disable the watchdog protections here in the last function being called at the end of the startup code.
 Starting from the B-step, it is also required to enable the OCDS mode and to suspend the watchdog timers in order to not produce an SMU alarm when the timeout is reached.
 See also the SMU_TC.001 Erratum in the TC27x_AB_Errata_Sheet_v1_1.pdf */
void _call_init(void)
{
  #ifdef ENDINIT_PROTECTION_DISABLED                                         // If the ENDINIT_PROTECTION_DISABLED define is set in ETK_Integration_Cfg.h
    /* Disables the endinit protection completely for the safety and CPU 0 watchdogs. */
    /** It is possible to disable the watchdog timers when OCDS mode is enabled. */
    if (CBS_OSTATE.B.OEN == 0) // If OCDS is currently disabled
    {
      CBS_OEC.U = 0xA1;
      CBS_OEC.U = 0x5E;
      CBS_OEC.U = 0xA1;
      CBS_OEC.U = 0x5E; // OCDS enable sequence
    }
    while(CBS_OSTATE.B.OEN == 0)  __asm("nop");

    /** CBS_OCNTRL can only be updated once the CBS_OSTATE.B.OEN bit is set to 1 and the OCDS mode is enabled. */
    CBS_OCNTRL.U = 0x00001000; //Watchdog Timer(s) suspended as long as OCDS is enabled

    /** We disable the ENDINIT protection for the safety and CPU0, this will set the watchdog in timeout mode.
    However we just disabled the watchdog timers earlier when enabling the OCDS mode.
    Thus, no timeout alarm will be produced even if the watchdogs are set to timeout mode.
    The ENDINIT protection can also be disabled for the CPU1 and CPU2 this way.*/
    unlock_wdtcon('s');
      SCU_WDTSCON1.U |= 0x8;
    lock_wdtcon('s');
    unlock_wdtcon('s');

    unlock_wdtcon('0');
      SCU_WDTCPU0CON1.U |= 0x8;
    lock_wdtcon('0');
    unlock_wdtcon('0');

  #endif
}

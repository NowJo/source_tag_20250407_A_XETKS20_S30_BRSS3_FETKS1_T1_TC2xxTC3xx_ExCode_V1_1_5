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
/*  COPYRIGHT (c) ETAS GmbH 2004-2015                                         */
/*  All Rights Reserved                                                       */
/******************************************************************************/


#include "../ETK_Integration_Cfg.h"     /* for the configuration and register addresses */

#include "../CompileTime.h"
#include "ETK_CodeCheck.h"
#include "../endinit.h"



//******************************************************************************
//ETK Presence check
#ifndef HIGHTEC_COMPILER
  #pragma section all "ETK_Presence_checkMemClass"
  #pragma noclear
#else
  #pragma section ".ETK_Presence_checkMemClass" aw 4
#endif
const volatile uint32 ETK_PRESENCE_PATTERN_ETK = 0;
#ifndef HIGHTEC_COMPILER
  #pragma clear
  #pragma section all
#else
  #pragma section
#endif

//******************************************************************************
//ETK Code check pattern in RAM section
#ifndef HIGHTEC_COMPILER
  #pragma section all "code_checkMemClassRam"
  #pragma noclear
#else
  #pragma section ".code_checkMemClassRam" aw 4
#endif
volatile uint32 CODE_CHECK_PATTERN_RAM_DATE;
volatile uint32 CODE_CHECK_PATTERN_RAM_TIME;
#ifndef HIGHTEC_COMPILER
  #pragma clear
  #pragma section all
#else
  #pragma section
#endif

//ETK Code check pattern in EMURAM section
#ifndef HIGHTEC_COMPILER
  #pragma section all "code_checkMemClassEMURam"
  #pragma noclear
#else
  #pragma section ".code_checkMemClassEMURam" aw 4
#endif
volatile uint32 CODE_CHECK_PATTERN_EMURAM_DATE;
volatile uint32 CODE_CHECK_PATTERN_EMURAM_TIME;
#ifndef HIGHTEC_COMPILER
  #pragma clear
  #pragma section all
#else
  #pragma section
#endif

uint8 CodeCheck_WP_passed = 0;
uint8 pageSwitch_after_CodeCheck = 0;

//checks if an ETK is connected to controller
uint8 PresenceCheck(void)
{
  //local variables
  uint32 addr;
  uint8 detected;

  addr = ((uint32) &ETK_PRESENCE_PATTERN_ETK);


  //check for ETK presence pattern
  if (ETK_PRESENCE_PATTERN_ETK  == ETK_PRESENCE_PATTERN_VALUE)
  {
    //ETK presence pattern found
    detected = 1;

  }

  else
  {
    //ETK presence pattern not found
    detected = 0;

  }

  return (detected);
}


//copy CodeCheck Pattern cyclic in ECU RAM - Used for ACC - Advanced Code Check
void CopyCodeCheckPattern_cyclic(uint8 Error_Pattern)
{
  if (Error_Pattern == write_wrong_RAM_Pattern)
  {
    //write error pattern in ECU RAM
    CODE_CHECK_PATTERN_RAM_DATE = ERROR_PATTERN_DATE;
    CODE_CHECK_PATTERN_RAM_TIME = ERROR_PATTERN_TIME;
  }

  else
  {
    //write ECU CodeCheck pattern from ECU code section to ECU RAM section
    CODE_CHECK_PATTERN_RAM_DATE = CodeCheckPattern_ECUCode_Date;
          CODE_CHECK_PATTERN_RAM_TIME = CodeCheckPattern_ECUCode_Time;
  }
}

void initRAMCheckPattern (void)
{
    //write ECU CodeCheck pattern from ECU code section to ECU RAM section
  CODE_CHECK_PATTERN_RAM_DATE = CodeCheckPattern_ECUCode_Date;
  CODE_CHECK_PATTERN_RAM_TIME = CodeCheckPattern_ECUCode_Time;
}

/* If this define is set the code will check if the advanced code check pattern for WP in the EMU ram was written by the ETK before doing a page switch.
    When set the PAGESWITCH_DEPENDING_ON_CODECHECK macro value will be used to define if this verification can block or not a page switching. */
#ifdef CODECHECK_WP_BEFORE_PAGESWITCH

  //do a code check before page switch RP -> WP
  uint8 CodeCheck_WP(void)
  {
    //local variables
    uint8 passed;
    uint32 CodeCheckPattern_Date;
    uint32 CodeCheckPattern_Time;

    // now read the code check pattern, which is located in the ECU EmuRAM region and compare with the pattern in the ECU code section
    CodeCheckPattern_Date = CODE_CHECK_PATTERN_EMURAM_DATE;
    CodeCheckPattern_Time = CODE_CHECK_PATTERN_EMURAM_TIME;


    //check for Code Check pattern
    if ((CodeCheckPattern_Date == CodeCheckPattern_ECUCode_Date) && (CodeCheckPattern_Time == CodeCheckPattern_ECUCode_Time))
    {
      passed = 1;
      CodeCheck_WP_passed = 1;

    }

    else
    {
      passed = 0;
      CodeCheck_WP_passed = 0;

    }

    return (passed);
  }
#endif // #ifdef CODECHECK_WP_BEFORE_PAGESWITCH

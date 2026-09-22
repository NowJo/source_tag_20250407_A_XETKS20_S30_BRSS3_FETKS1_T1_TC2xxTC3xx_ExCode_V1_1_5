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
/*  COPYRIGHT (c) ETAS GmbH 2012-2018                                         */
/*  All Rights Reserved                                                       */
/******************************************************************************/

#include "ETK_Integration_Cfg.h"
#include "GetTime.h"
// #include "../base.h"    // is needed for referring to registers from .sfr file



#ifdef HIGHTEC_COMPILER
  #include <machine/intrinsics.h> // reference to the __mfcr() function
#endif

void BASE_waitTime(uint32 waitTime)
{
  uint32 startTime, endTime;

  startTime = BASE_GetSystemTime();
  endTime = startTime + waitTime;

  while(BASE_GetSystemTime() < endTime)
  {
  }
}

uint8 BASE_Check_Timeout(uint32 startTime, uint32 waitPeriodMicroseconds)
{
    uint32 currentTime = STM0_TIM0.U;
    uint32 passedTicks = currentTime - startTime;
    uint32 passedMicroseconds = (passedTicks / TICKS_TO_USEC); // ticks / (ticks/�sec) = ticks * �sec/ticks = �sec :)

    return (passedMicroseconds >= waitPeriodMicroseconds);
}

/*32bit counter for getting System Time information */
uint32 BASE_GetSystemTime(void)
{
  return(STM0_TIM0.U);
}

/* 32bit counter for getting System Time information */
uint32 GetSystemTime(void)
{
  return(STM0_TIM0.U);
}




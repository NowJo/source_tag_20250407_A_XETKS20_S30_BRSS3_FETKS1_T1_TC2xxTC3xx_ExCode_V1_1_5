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

#include "../ETK_Integration_cfg.h" /* ETK Integration Configuration */
#include "../../a_std_type.h"       /* uint8, uint16, uint32, etc. */
#include "../GetTime.h"             /* GetSystemTime */
#include "ETK_SER_Coldstart.h"      /* Coldstart function prototypes */
#include "../ETK_SER_Handshake.h"   /* Handshake function prototypes */

static uint32 CheckTimeoutWait(uint32 waitTime);
static uint32 CheckTimeoutReady(uint32 waitTime);

#ifdef TASKING_COMPILER
  #pragma section all  "ETK_ColdStart_MemClass"
  #pragma noclear
#endif // #ifdef TASKING_COMPILER

#ifdef HIGHTEC_COMPILER
  #pragma section ".ETK_ColdStart_MemClass" 4
#endif

uint32 ColdStart_pattern;
struct Handshake_Coldstart_M Handshake_Coldstart_M = { 0 };
uint32 ColdStartStates;

#ifdef TASKING_COMPILER
  #pragma clear
  #pragma section all
#endif // #ifdef TASKING_COMPILER

#ifdef HIGHTEC_COMPILER
  #pragma section
#endif

#ifdef TASKING_COMPILER
  #pragma section all  "coldstart_parameters"
  #pragma noclear
#endif // #ifdef TASKING_COMPILER

#ifdef HIGHTEC_COMPILER
  #pragma section ".coldstart_parameters" 4
#endif

const uint32 Coldstart_Ready_Timeout = 50000; // 50 ms (expected <50ms) wait time for the READY pattern after the ETK handshake

#ifdef TASKING_COMPILER
  #pragma clear
  #pragma section all
#endif // #ifdef TASKING_COMPILER

#ifdef HIGHTEC_COMPILER
  #pragma section
#endif

/* Saves the time after the ECU triggers the end phase of the handshake, in order to calculate the timestamps for the coldstart functionality.
  Will only execute if the MC_Wait bit is set to 1 and the coldstart is requested during the handshake.*/
void Start_Timing_Coldstart(void)
{
  if (ECU_ETK_Status.ETK_MC_Wait == 1) // coldstart requested - The coldstart time calculations will not display a high value this way if no coldstart is executed.
  {
    Handshake_Coldstart_M.StartTime = GetSystemTime();
  }
}

/* Clears the coldstart value.  */
void SER_ETK_Clear_Coldstart_Value(void)
{
  #ifdef COLDSTART_DEBUG_PATTERN
    /* save the previous coldstart pattern value */
    Handshake_Coldstart_M.PatternAtStartRoutine = ColdStart_pattern;
  #endif
  /* coldstart pattern reset value defined in the A2L */
  ColdStart_pattern = COLDSTART_RESET_PATTERN; // 0x12345678 by default
}


static uint32 CheckTimeoutReady(uint32 waitTime)
{
  while (! ((COLDSTART_READY_PATTERN == ColdStart_pattern) ||
            (BASE_Check_Timeout(Handshake_Coldstart_M.StartTime, waitTime))))
  {
      /* Do nothing */
  }
  return ColdStart_pattern;
}


/* Coldstart function, waits for the READY pattern
Delays the start of the user code until either the READY pattern is detected  in the coldstart mailbox or a timeout occurs.
This function will only be executed if the MC_Wait bit has been set during the handshake.
Return values:
    0x00 = if no Coldstart due to ETK not detected
    0x02 = Timeout Ready
    0x03 = Initial Ready Detected
    0x05 = Ready_Loop Ready Detected */
uint8 SER_ETK_Check_Coldstart(void)
{
  uint32 pattern;

  // Get the Coldstart Pattern content
  if (COLDSTART_READY_PATTERN == ColdStart_pattern) // Initial READY pattern detected
  {
    #ifdef COLDSTART_EXECUTION_TIME
      Handshake_Coldstart_M.CounterEndReady = GetSystemTime();
    #endif
    Handshake_Coldstart_M.ReturnValue = COLDSTART_RETURN_INITIAL_READY_DETECTED;

  }
  else
  {
    pattern = CheckTimeoutReady(Coldstart_Ready_Timeout);
    #ifdef COLDSTART_EXECUTION_TIME
      Handshake_Coldstart_M.CounterEndReady = GetSystemTime();
    #endif
    switch (pattern)
    {
      case COLDSTART_READY_PATTERN:
        Handshake_Coldstart_M.ReturnValue = COLDSTART_RETURN_READY_LOOP_READY_DETECTED;

        break;

      default:
        Handshake_Coldstart_M.ReturnValue = COLDSTART_RETURN_TIMEOUT_WAITING_READY;

        break;
    }
  }

  #ifdef COLDSTART_DEBUG_PATTERN
    Handshake_Coldstart_M.PatternAtEndRoutine = ColdStart_pattern;
  #endif
  return Handshake_Coldstart_M.ReturnValue;
}

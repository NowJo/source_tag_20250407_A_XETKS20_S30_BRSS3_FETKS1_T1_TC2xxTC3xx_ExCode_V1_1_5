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

#ifndef _ETK_SER_COLDSTART_H_
#define _ETK_SER_COLDSTART_H_

#include "../ETK_Integration_Cfg.h"   /* ETK Integration Configuration */

void Start_Timing_Coldstart(void);
void SER_ETK_Clear_Coldstart_Value(void);
uint8 SER_ETK_Check_Coldstart(void);

extern struct Handshake_Coldstart_M {
  #ifdef COLDSTART_DEBUG_PATTERN
    uint32 PatternAtStartRoutine;
    uint32 PatternAtEndRoutine;
  #endif
  #ifdef COLDSTART_EXECUTION_TIME
    uint32 CounterEndReady;
  #endif
    uint32 StartTime;
    uint8 ReturnValue;
} Handshake_Coldstart_M;

extern uint32 ColdStartStates;

enum {
    COLDSTART_RETURN_NO_ETK = 0,
    COLDSTART_RETURN_TIMEOUT_WAITING_READY = 2,
    COLDSTART_RETURN_INITIAL_READY_DETECTED = 3,
    COLDSTART_RETURN_READY_LOOP_READY_DETECTED = 5,
};

enum {
#if defined PROCESSOR_MEM_TYPE_MOTOROLA
    COLDSTART_READY_PATTERN = 0x52454459, // REDY
    COLDSTART_WAIT_PATTERN = 0x57414954,  // WAIT
#elif defined PROCESSOR_MEM_TYPE_INTEL
    COLDSTART_READY_PATTERN = 0x59444552, // YDER
    COLDSTART_WAIT_PATTERN = 0x54494157, // TIAW
#else
#error "Please define either PROCESSOR_MEM_TYPE_MOTOROLA or PROCESSOR_MEM_TYPE_INTEL"
#endif
    COLDSTART_RESET_PATTERN = 0x12345678,
};

#endif /* _ETK_SER_COLDSTART_H_ */

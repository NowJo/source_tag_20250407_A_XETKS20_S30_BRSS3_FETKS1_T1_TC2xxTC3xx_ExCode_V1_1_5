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
/*  COPYRIGHT (c) ETAS GmbH 2017                                              */
/*  All Rights Reserved                                                       */
/******************************************************************************/

/***************************************************************************//**
* \file  Trace_Fkt.c
* \brief file containing the function implementation to process Trace measurements
*
* This file contains the definition of the function that processes an individual
* (and active) Trace measurement event
*******************************************************************************/

#include "../../a_std_type.h"         /* for the standard types */
#include "../ETK_Integration_cfg.h"   /* for the configuration */

#include "../D17/Distab17_Fct.h"      /* for the Distab17 functions */
#include "Trace_Fct.h"                /* for the Trace functions */


/** The Trigger value that the FETK-T will read must be a 4 byte value. However the FETK-T will only recognize 255 triggers (1-255). */
void TraceExecTriggerByValue (uint8 triggerValue)
{
  /** Due to a µC bug (same cause as MCDS_TC.059) the value of the trigger must be read before writing it to prevent
  a double detection of the trigger. The bug happens with TC2xx controllers but not with TC3xx at the moment. */
  /* When using ASM code the address of the trace triggers are hardcoded ! Here we use the address 0x70000020 ! */
  #ifdef TRACE_TRIGGER_ASM_WORKAROUND
   #ifdef HIGHTEC_COMPILER
    __asm__ (
    "  movh.a  %a15,0x7000 \n\t"
    "  ld.w %d15, [%a15]0x20  \n\t"
    "  st.w [%a15]0x20,%d4  \n\t"
    );
   #elif TASKING_COMPILER
    __asm(
    "  movh.a  a15,#0x7000 \n\t"
    "  ld.w d15, [a15]0x20  \n\t"
    "  st.w [a15]0x20,d4  \n\t"
    );
   #else
     #error "Undefined ASM call function in TraceExecTriggerByValue !"
   #endif
  #elif (defined TRACE_TRIGGER_ASM_NOP_WORKAROUND)
   #ifdef HIGHTEC_COMPILER
    __asm__ (
     "  movh.a  %a15,0x7000 \n\t"
    "  nop \n\t nop \n\t nop \n\t nop \n\t nop \n\t nop \n\t nop \n\t nop \n\t "
    "  st.w [%a15]0x20,%d4  \n\t"
    );
   #elif TASKING_COMPILER
    __asm(
     "  movh.a  a15,#0x7000 \n\t"
    "  nop \n\t nop \n\t nop \n\t nop \n\t nop \n\t nop \n\t nop \n\t nop \n\t "
    "  st.w [a15]0x20,d4  \n\t"
    );
   #endif
  #else
    volatile uint32 readValue;
    readValue = TraceTriggerByValue;

    TraceTriggerByValue = triggerValue;
  #endif
}


/** The Trigger value that the FETK-T will read must be a 4 byte value. However the FETK-T will only recognize 255 triggers (1-255).
This function will use a different trigger address. It will only be used by the Distab17 function when configured to be a supplementary Distab. */
void TraceExecTriggerByValue_SupplDistab (uint8 triggerValue)
{
  /** Due to a µC bug (same cause as MCDS_TC.059) the value of the trigger must be read before writing it to prevent
  a double detection of the trigger.*/
  /* When using ASM code the address of the trace triggers are hardcoded ! Here we use the address 0x70000024 ! */
  #ifdef TRACE_TRIGGER_ASM_WORKAROUND
   #ifdef HIGHTEC_COMPILER
    __asm__ (
    "  movh.a  %a15,0x7000 \n\t"
    "  ld.w %d15, [%a15]0x24  \n\t"
    "  st.w [%a15]0x24,%d4  \n\t"
    );
   #elif TASKING_COMPILER
    __asm(
    "  movh.a  a15,#0x7000 \n\t"
    "  ld.w d15, [a15]0x24  \n\t"
    "  st.w [a15]0x24,d4  \n\t"
    );
   #endif
    
  #elif (defined TRACE_TRIGGER_ASM_NOP_WORKAROUND)
   #ifdef HIGHTEC_COMPILER
    __asm__ (
    "  movh.a  %a15,0x7000 \n\t"
    "  nop \n\t nop \n\t nop \n\t nop \n\t nop \n\t nop \n\t nop \n\t nop \n\t "
    "  st.w [%a15]0x24,%d4  \n\t"
    );
   #elif TASKING_COMPILER
    __asm(
    "  movh.a  a15,#0x7000 \n\t"
    "  nop \n\t nop \n\t nop \n\t nop \n\t nop \n\t nop \n\t nop \n\t nop \n\t "
    "  st.w [a15]0x24,d4  \n\t"
    );
   #endif
    
  #else
    volatile uint32 readValue;
    readValue = TraceTriggerByValue_SupplDistab;

    TraceTriggerByValue_SupplDistab = triggerValue;
  #endif
}


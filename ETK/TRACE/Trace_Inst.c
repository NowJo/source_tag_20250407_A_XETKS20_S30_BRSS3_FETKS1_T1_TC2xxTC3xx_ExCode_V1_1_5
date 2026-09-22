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
* \file  Trace_Inst.c
* \brief file containing the definition of Trace data structs

* All data structures needed for the ECU part to support Trace measurements are
* instantiated here.
*
* Note: no initialisation is done here -- please either change the statement,
* locate in a cleared section or initialise explicitly if required.
*******************************************************************************/

#include "../../a_std_type.h"         /* for the standard types */
#include "../ETK_Integration_cfg.h"   /* for the configuration */
#include "Trace_Inst.h"               /* for the Trace instance */

  #ifdef TASKING_COMPILER
    #pragma section all "ETK_Trace_TriggerByValue"
  #endif
  #ifdef HIGHTEC_COMPILER
    #pragma section ".ETK_Trace_TriggerByValue" 4
  #endif


  /** The Trigger value that the FETK-T1 will read must be a 4 byte value. However the FETK-T1 will only recognize 255 triggers (1-255). 
  This example show both the use of static trace and supplementary distab dynamic triggers. 
  However only 1 Trace trigger is needed and defined in an A2L file. */
  volatile uint32 TraceTriggerByValue, /* This trigger value is always active without any configuration. */
  TraceTriggerByValue_SupplDistab; /* This trigger value will be used by the supplementary Distabs. */

  #ifdef TASKING_COMPILER
    #pragma clear
    #pragma section all
  #endif

  #ifdef HIGHTEC_COMPILER
    #pragma section
  #endif

/*
################################################################################
#                                                                              #
#    ETK driver example                          |   ETAS GmbH                 #
#    Distab17 instantiation                      |   Stuttgart Feuerbach       #
#    For Demonstration Purpose Only              |   All rights reserved       #
#    sample driver implementation                |   Alle Rechte vorbehalten   #
#                                                                              #
################################################################################
*/

/******************************************************************************/
/* Copyright and Legal Disclaimer:                                            */
/* This Code example was provided by ETAS GmbH, Stutttgart                    */
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

/***************************************************************************//**
* \file  Distab17_Inst.c
* \brief file containing the definition of the Distab17 data structs

* All data structures needed for the ECU part of using the Distab17 are
* instantiated here.
*
* Note: no initialisation is done here -- please either change the statement,
* locate in a cleared section or initialise explicitly if required.
*******************************************************************************/

#include "Distab17_Inst.h"            /* for the configuration */
#include "../ETK_Integration_cfg.h"   /* for the configuration */

#ifdef TASKING_COMPILER
  #pragma section all "ETK_DisTab17_EventList"    /* Distab17 Event List */
#endif // #ifdef TASKING_COMPILER

#ifdef TRG_SERIAL_ETK

  #ifdef TASKING_COMPILER
    #pragma noclear
  #endif // #ifdef TASKING_COMPILER

#endif // #ifdef TRG_SERIAL_ETK

#ifdef HIGHTEC_COMPILER
  #pragma section ".ETK_DisTab17_EventList" 4   /* Distab17 Event List */
#endif


#ifdef USE_DISTAB_CONST_DEF
DISTAB_CONST_DEF
#endif /* #ifdef USE_DISTAB_CONST_DEF */
#ifdef USE_VOLATILE_DEF
VOLATILE_DEF
#endif /* #ifdef USE_VOLATILE_DEF */
tD17EventList Distab17EventList /* = {} */;

  #ifdef TRG_SERIAL_ETK
    #ifdef TASKING_COMPILER
      #pragma clear
    #endif // #ifdef TASKING_COMPILER
  #endif // #ifdef TRG_SERIAL_ETK

  #ifdef TASKING_COMPILER
    #pragma section all
  #endif // #ifdef TASKING_COMPILER

  #ifdef HIGHTEC_COMPILER
    #pragma section
  #endif


  #ifdef TASKING_COMPILER
    #pragma section all "ETK_DisTab17_EventConfigArea"    /* Distab17 Configuration Area */
  #endif // #ifdef TASKING_COMPILER

  #ifdef HIGHTEC_COMPILER
    #pragma section ".ETK_DisTab17_EventConfigArea" 4     /* Distab17 Configuration Area */
  #endif

  #ifdef TARGET_SH7xxx_Renesas
    #pragma section ETK_DisTab17_EventConfigArea
  #endif // #ifdef TARGET_SH7xxx_Renesas


  #ifdef USE_FAR_DEF
    FAR_DEF
  #endif // #ifdef USE_FAR_DEF
  #ifdef USE_VOLATILE_DEF
    VOLATILE_DEF
  #endif // #ifdef USE_VOLATILE_DEF
  uint8 Distab17EventConfigArea [ D17_EVENT_CONFIG_AREA_SIZE ];



  #ifdef TRG_SERIAL_ETK
    #ifdef TASKING_COMPILER
      #pragma clear
    #endif // #ifdef TASKING_COMPILER
  #endif // #ifdef TRG_SERIAL_ETK

  #ifdef TASKING_COMPILER
    #pragma section all
  #endif // #ifdef TASKING_COMPILER

  #ifdef HIGHTEC_COMPILER
    #pragma section
  #endif


  #ifdef TASKING_COMPILER
    #pragma section all "ETK_DisTab17_EventOutputArea"    /* Distab17 Output Area */
  #endif // #ifdef TASKING_COMPILER

  #ifdef HIGHTEC_COMPILER
    #pragma section ".ETK_DisTab17_EventOutputArea" 4     /* Distab17 Output Area */
  #endif

  #ifdef TARGET_SH7xxx_Renesas
    #pragma section ETK_DisTab17_EventOutputArea
  #endif // #ifdef TARGET_SH7xxx_Renesas


  #ifdef USE_FAR_DEF
    FAR_DEF
  #endif // #ifdef USE_FAR_DEF
  #ifdef USE_VOLATILE_DEF
    VOLATILE_DEF
  #endif // #ifdef USE_VOLATILE_DEF
  uint8 Distab17EventOutputArea [ D17_EVENT_OUTPUT_AREA_SIZE ];     /* Distab17 Output Area calculation size */



  #ifdef TRG_SERIAL_ETK
    #ifdef TASKING_COMPILER
      #pragma clear
    #endif // #ifdef TASKING_COMPILER
  #endif // #ifdef TRG_SERIAL_ETK

  #ifdef TASKING_COMPILER
    #pragma section all
  #endif // #ifdef TASKING_COMPILER

  #ifdef HIGHTEC_COMPILER
    #pragma section
  #endif

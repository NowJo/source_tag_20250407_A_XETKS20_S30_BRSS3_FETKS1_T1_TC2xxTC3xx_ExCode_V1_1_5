/*
################################################################################
#                                                                              #
#    ETK driver example                          |   ETAS GmbH                 #
#    Distab17 function definition                |   Stuttgart Feuerbach       #
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
/*  COPYRIGHT (c) ETAS GmbH 2012-2017                                         */
/*  All Rights Reserved                                                       */
/******************************************************************************/

/***************************************************************************//**
* \file  Distab17_Fct.c
* \brief file containing the function implementation to process a Distab event
*
* This file contains the definition of the function that processes an individual
* (and active) non-TDM Distab event
*******************************************************************************/

#include "../ETK_Integration_cfg.h"   /* for the configuration */
#include "Distab17_Fct.h"             /* for the configuration */



uint8 distab17fkt(tD17EventConfig *config)
{
/*******************************************************************************
* \fn  distab17fkt
* \brief processes the Distab, copies data
* \param config Distab17 event configuration for this event
*
* The distab function parses the display table (ordered address table written
* by a measurement or rapid prototyping tool) and copies the values at these
* ECU memory addresses to a dedicated output buffer that can be read by these
* tools.
*
* The caller must have asserted that this event is valid (contains the
* correct version number, a valid pointer to the output area, and is not
* configured for TDM).
*******************************************************************************/

    /* pointers to measurement output buffers */
    uint8 *dst_ptr1;
    uint16 *dst_ptr2;
    uint32 *dst_ptr4;
    ustr64 *dst_ptr8;

    /* the current data source */
    void **adr_ptr;

    /* pointer to storage buffer */
    dst_ptr8 = (ustr64 *) &(config->Header.Output->FirstEntry);

    /* Number of 8, 4, 2, and 1 byte variables */
    uint16 no_of_val_8 = config->NoOfVal_8;
    uint16 no_of_val_4 = config->NoOfVal_4;
    uint16 no_of_val_2 = config->NoOfVal_2;
    uint16 no_of_val_1 = config->NoOfVal_1;

    /* pointer to first first address to copy */
    adr_ptr = &(config->FirstAddress);

    /**********************     ustr64      ***********************************/

    /* data acquisition: copy all 8-byte values */
    while (no_of_val_8 > 0)
    {
        *dst_ptr8++ = * ((ustr64 *) *adr_ptr++);
        no_of_val_8--;
    }

    /**********************     uint32      ***********************************/

    dst_ptr4 = (uint32 *) dst_ptr8;
    while (no_of_val_4 > 0)
    {
        *dst_ptr4++ = * ((uint32 *) *adr_ptr++);
        no_of_val_4--;
    }

    /**********************     uint16      ***********************************/

    dst_ptr2 = (uint16 *) dst_ptr4;
    while (no_of_val_2 > 0)
    {
        *dst_ptr2++ = * ((uint16 *) *adr_ptr++);
        no_of_val_2--;
    }

    /**********************     uint8       ***********************************/

    dst_ptr1 = (uint8 *) dst_ptr2;
    while (no_of_val_1 > 0)
    {
        *dst_ptr1++ = * ((uint8 *) *adr_ptr++);
        no_of_val_1--;
    }

    return D17_DATA_ACQUISITION_SUCCESS;
}

#if (defined CPUCLASS_TC2D5 || defined CPUCLASS_TC3XX)
  void Set_ETKTrigger_Indirect_D17(uint16 myTrigger, uint32 *myTrigAdr, uint32 myTrigId_Val)
  {
    // Set Trigger Identifier Flag
    *myTrigAdr =  (uint32) myTrigId_Val;
    EXEC_AUD_Trigger(myTrigger);
  }
  
  
  /** Errata OCDS_TC.039 Cerberus IO_READ_TRIG may not always clear the TRIG bit being read.
  The IO_READ_TRIG operation from DAP/JTAG can sometimes fail to clear a
  trigger bit in the highest priority trigger register TRIGy being read.
  The issue occurs on a software write to register TRIGS, which must happen in
  the same core clock cycle as an IO_READ_TRIG operation accesses the
  highest-priority trigger register TRIGy, where the same bit position set by
  TRIGS in TRIGx is already set in TRIGy, the previous IO_READ_TRIG
  operation accessed trigger register TRIGx, and x?y. The correct value is
  returned to the DAP for TRIGy, but the affected single trigger bit in TRIGy is not
  cleared.
  The effect is that sometimes a trigger bit may not be cleared after it has been
  read by the IOClient using IO_READ_TRIG. The same trigger may be
  presented a second time (or more) even though it has only been triggered once
  by software. No triggers are lost.
  Workaround 1
  Use just the 24 different trigger bits 0 to 23 of TRIG0.*/
  inline void EXEC_AUD_Trigger(uint16 myTrigger)
  {
    CBS_TRIGS.U = myTrigger; // write the trigger number to trigger into the register - The Trigger 0 will raise the first bit in the trigger register...
  }

#endif


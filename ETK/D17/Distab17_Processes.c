/*
################################################################################
#                                                                              #
#    ETK driver example                          |   ETAS GmbH                 #
#    Distab17 process                            |   Stuttgart Feuerbach       #
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
/*  COPYRIGHT (c) ETAS GmbH 2012-2019                                         */
/*  All Rights Reserved                                                       */
/******************************************************************************/

//#include "global.h"
#include "../ETK_Integration_cfg.h"

#include "Distab17_Def.h"
#include "Distab17_Fct.h"
#include "Distab17_Inst.h"

//#include "ETK_Trigger.h"

#ifdef TRACE_SUPPORT
  #include "../Trace/Trace_Inst.h"
  #include "../Trace/Trace_Fct.h"
#endif

static void D17Trigger(uint16 triggerEntry,
                       uint32 *triggerFlag,
                       uint32 triggerFlagValue)
{
/*******************************************************************************
* \fn D17Trigger
* \brief Trigger this event
* \param triggerEntry the (possibly virtual) trigger to use
* \param triggerFlag the address to write the trigger value to
* \param triggerFlagValue the value to write to the trigger flag address
*
* This function invokes the low-level trigger function, after possibly
* looking up the actual trigger id to be set.
*******************************************************************************/
    uint16 hardwareTrigger = 0;

    #ifdef D17_TRACE_SUPPL_DISTAB
      // Trace trigger virtual ID detection and triggering
      if ((triggerEntry > D17_TRACE_ID_OFFSET) && (triggerEntry <= (D17_TRACE_ID_OFFSET + D17_TRACE_MAX_TRIGGER)) )
      {
        hardwareTrigger = triggerEntry - D17_TRACE_ID_OFFSET;

        #if (defined TRACE_TRIGGER_ASM_WORKAROUND || defined TRACE_TRIGGER_ASM_NOP_WORKAROUND)
          TraceExecTriggerByValue_SupplDistab((uint8) hardwareTrigger);
        #else
          TraceTriggerByValue_SupplDistab = hardwareTrigger; // Writing the TRACE trigger.
        #endif
      }
      else
      {
    #endif

    #ifdef D17_USE_VIRTUAL_TRIGGER
      int i;

      /* check the table if the trigger entry is a redirection */
      /* note: if there are many indirections, this implementation might be too
         slow */
      for (i = 0; (! hardwareTrigger) && (i < ECUTriggerTable.Entries); i++)
      {
          if (triggerEntry == ECUTriggerTable.Table[i].ECU_ID)
          {
              hardwareTrigger = ECUTriggerTable.Table[i].TriggerNumber;
          }
      }
    #endif

    /* not a redirection -- use the trigger number directly */
    if (! hardwareTrigger)
    {
        hardwareTrigger = (uint8) triggerEntry;
    }

    /** Tricore TC2xx and MPC57xx specific - The trigger needs to be offset back with 1 because the value written to the register will trigger the bit with this number.
     Thus if we write the trigger number 1 given by the Event config area it will set the bit numbered 1 to true which will give the value 0x2 because there is also a bit numbered 0.*/
    Set_ETKTrigger_Indirect_D17(hardwareTrigger - 1, triggerFlag, triggerFlagValue);

    #ifdef D17_TRACE_SUPPL_DISTAB
      }
    #endif
}

uint8 Distab17_Process(uint8 eventNumber)
{
/*******************************************************************************
* \fn Distab17_Process
* \brief Process the configuration for the given event
* \param eventNumber the current event
*
* This function looks up the event configuration, decides whether this event is
* enabled, used TDM or the output table, if necessary invokes the data copy
* and finally executes the trigger.
*******************************************************************************/

    uint8 result = D17_EVENT_NOT_ACTIVE;
    uint32 config = Distab17EventList.Config[eventNumber-1];
    uint32 triggerFlagValue = ((eventNumber << 0x18) |
                               (eventNumber << 0x10) |
                               (eventNumber << 0x08) |
                               (eventNumber << 0x00));

    int configIsEnabled = config & 1;
    #ifdef D17_ETK_TRIGGER_FLAG_CLEAR
      volatile int t = 0;
    #endif

    if (configIsEnabled)
    {
        /* The entry in the configuration list is a tagged pointer, and aligned
           to four bytes, with the lowest two bits containing other information
           -- e.g. the "is enabled" setting
         */
        tD17EventConfigHeader *eventConfig =
            // (tD17EventConfigHeader *) (config & 0xfffffffc);
            // (tD17EventConfigHeader *) ((config >> 2) << 2);
            (tD17EventConfigHeader *) (config & ~3);

        if (D17_VERSION_NUMBER == Distab17EventList.Version)
        {
            tD17EventOutputHeader *output = eventConfig->Output;

#ifdef D17_ETK_TRIGGER_FLAG_CLEAR
            while (triggerFlagValue == output->TriggerFlag)
            {
                /* Busy loop - Waiting for a different event using the same output area with indirect triggering to be ready ? */
                t++;
            }
#endif

            output->EcuStatus = ~0;

            result = ((eventConfig->Config & 1)) // checks if TDMconfig  is enabled with the bit 0 of Config set to 1
                ? D17_DATA_ACQUISITION_SUCCESS
                : distab17fkt((tD17EventConfig *) eventConfig);

            D17Trigger(eventConfig->Trigger,
                       &(output->TriggerFlag),
                       triggerFlagValue);

            output->EcuStatus = 0;
        }
        else
        {
            result = D17_EVENT_UNSUPPORTED_VERSION;
        }
    }
    return result;
}

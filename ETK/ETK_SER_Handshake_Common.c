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


#include "ETK_Integration_Cfg.h"       /* for the configuration */
#include "GetTime.h"                  /* for the GetSystemTime() function */
#include "ETK_SER_Handshake.h"        /* for the ETK handshake */
#ifdef COLDSTART_SUPPORTED
#include "CS/ETK_SER_Coldstart.h"     /* for the coldstart functions */
#endif
#ifdef CALIBRATION_SUPPORTED
#include "CALIB/ETK_SER_Page_Ctrl_Fct.h"  /* for the ETK page control functions */
#endif
#include "../GPIO.h"                    /* for the GPIO functions */
#ifdef DISTAB_SUPPORTED
#include "D17/Distab17_Inst.h"        /* for the Distab17 instance */
#endif

/** Some definitions:
MC_WAIT -> Measurement and Calibration Wait (Handshake / Coldstart)
RP_WAIT -> Rapid Prototyping Wait (Handshake)
ES device -> Interface Module between the ETK and the PC
*/



/* Detection of the ETK at the startup and execution of the initial handshake. If no ETK protocol is found until the timeout the ECU will initialize without the ETK given settings. */
unsigned char SER_ETK_Detect(void)
{
  uint16 protocolSuccess; //  ETK Detection Pattern Protocol read success (1) /fail (0) value

  protocolSuccess = SER_ETK_Detection_Protocol_Handshake(); // Executes the ETK Detection Pattern Protocol read from the COMDATA register
  if (protocolSuccess == 1 ) // ETK Detection Pattern Protocol read successful  (ETK detected)
  {
    if (ECU_ETK_Status.ETK_Available)
    {
      Enable_OCDS_Triggers(); // Enables the OCDS mode in order to be able to use the triggers. Required starting from the B-step.
    }

    #ifdef PD_DEVICE

      /* The DSPR ram used for emulation will be cleared upon an ECU reset by the Infineon boot code. */

    #else // ED device
      #ifdef CALIBRATION_SUPPORTED
      Configure_EMEM_Tiles_For_ECU_RAM(); //  Sets the EMEM tiles that are used internally by the ECU as RAM to calibration/RAM mode.
      #endif

      if ( ECU_ETK_Status.ETK_RAM_Valid ) // RAM valid.
      { }
      else  // RAM powerfail detected. Initialize the ED (EMEM) RAM used for calibration.
      {
        /* Initialize the program memory and the EMEM memory with 0 initial values  for ECC.
        The EMEM memory of the Tricore TC2D5 has to be written entirely once before being accessible after a start. */
        EMEM_RAM_ECC_Initialize();
      }
    #endif // #ifdef PD_DEVICE

    // Clear the OMD mailbox error code
    #ifdef CALIBRATION_SUPPORTED
      Page_Switch_Mailbox.Error_Code = 0; // Clear the error code for the page switching
    #endif

    /** Disable Distab 13/17 since Distab RAM areas should not be initialized by controller, and the areas might not be consistent. */
    #ifdef DISTAB_SUPPORTED
    SER_ETK_Disable_Distabs();
    #endif
  }
  else // ETK Detection Pattern Protocol read failed (no ETK detected)
  {
    #ifdef PD_DEVICE
      /** Disable Distab 13/17 since Distab RAM areas should not be initialized by controller, and the areas might not be consistent. */
      #ifdef DISTAB_SUPPORTED
      SER_ETK_Disable_Distabs();
      #endif

      /* Disables the calibration handles. Switch to the RP. */
      #ifdef CALIBRATION_SUPPORTED
      Disable_Calibration_Handles();
      #endif

    #else // ED device
      #ifdef CALIBRATION_SUPPORTED
      Configure_EMEM_Tiles_For_ECU_RAM(); //  Sets the EMEM tiles that are used internally by the ECU as RAM to calibration/RAM mode.
      #endif

      /** Disable Distab 13/17 since Distab RAM areas should not be initialized by controller, and the areas might not be consistent. */
      #ifdef DISTAB_SUPPORTED
      SER_ETK_Disable_Distabs();
      #endif

      /* Disables the calibration handles. Switch to the RP. */
      #ifdef CALIBRATION_SUPPORTED
      Disable_Calibration_Handles();
      #endif

      /* Initialize the program memory and the EMEM memory with 0 initial values  for ECC.
      The EMEM memory has to be written entirely once before being accessible after a start. */
      EMEM_RAM_ECC_Initialize();
    #endif // #ifdef PD_DEVICE
    
  } // firstHandshake with no ETK detected

  return ECU_ETK_Status.ETK_Available;
}

void SER_Initial_Handshake_Execute(void)
{
  SER_ETK_Detect();

  if (ECU_ETK_Status.Protocol_Success == 1) // Protocol detected
  {
    #ifdef COLDSTART_SUPPORTED                                            
    SER_ETK_Clear_Coldstart_Value(); // Clears the coldstart value
    #endif

    #ifdef CALIBRATION_SUPPORTED
      SER_ETK_Page_Init_and_Check_WP(1);  // 1-> PBA, 0-> DRA
    #endif

    SER_ETK_Handshake_End(); // End the handshake from the ECU side

    if (ECU_ETK_Status.Handshake_End_State == 0) // The handshake ended correctly
    {
      //GPIO_SetLEDPinHigh(2); // sets the 3rd LED from left to the ON state on a Triboard if the ETK Handshake is successful

      if (ECU_ETK_Status.ETK_MC_Wait) //MC_Wait requested
      { 
        #ifdef COLDSTART_SUPPORTED
        ColdStartStates = SER_ETK_Check_Coldstart();
        #endif
      }

      if (ECU_ETK_Status.ETK_RP_Wait) //RP_Wait requested
      {
        /** The RP_Wait functionality is currently not implemented for the ETK. */
      }
    }
    else
    {
      //GPIO_SetLEDPinLow(2);  // sets the 3rd LED from left to the OFF state on a Triboard if the ETK Handshake fails
    }

    
  }
  else
  {
    GPIO_SetLEDPinLow(2);  // sets the 3rd LED from left to the OFF state on a Triboard if the ETK Handshake fails
  }
}

void SER_Cyclic_Handshake_Check_And_Execute(void)
{
  // If the 1st handshake was executed successfully without a timeout
  if (ECU_ETK_Status.Protocol_Success==1 && ECU_ETK_Status.Handshake_End_State==0)
  {
    if (SER_ETK_Check_Protocol_Handshake() == 1) // If the handshake protocol is detected - ETK pattern is present in COMDATA
    {

      if (ECU_ETK_Status.ETK_Available)
      {
        Enable_OCDS_Triggers(); // Enables the OCDS mode in order to be able to use the triggers.  Required starting from the B-step.
      }
      #ifdef CALIBRATION_SUPPORTED
      Configure_EMEM_Tiles_For_ECU_RAM(); //  Sets the EMEM tiles that are used internally by the ECU as RAM to calibration/RAM mode.
      #endif

      // According to the RAM_Valid bit will initialize the RAM and configure/clear the distab and mailboxes
      SER_ETK_RAM_Validity_Process();

      #ifdef CALIBRATION_SUPPORTED
        SER_ETK_Page_Init_and_Check_WP(1); // 1-> PBA, 0-> DRA
      #endif


      SER_ETK_Handshake_End(); // End the handshake from the ECU side
      
    }
  }
}

// According to the RAM_Valid bit will initialize the RAM and configure/clear the distab and mailboxes
void SER_ETK_RAM_Validity_Process(void)
{
  if ( ECU_ETK_Status.ETK_RAM_Valid ) // RAM valid. Clear the M_Distab Headers and the mailboxes in RAM
  {
    #ifdef CALIBRATION_SUPPORTED
      Page_Switch_Mailbox.Error_Code = 0; // Clear the error code for the page switching
    #endif
  }
  else  // RAM powerfail detected. Invalidate the Distab and bypass functions
  {
    /* Disable Distab (Deactivate) since Distab RAM Areas should not be initialized by controller, as the area might not be consistent. */
    #ifdef DISTAB_SUPPORTED
    SER_ETK_Disable_Distabs();
    #endif

    /* Initialize the program memory and the EMEM memory with 0 initial values  for ECC.
    The EMEM memory of the Tricore TC2D5 has to be written entirely once before being accessible after a start. */
    EMEM_RAM_ECC_Initialize();
  }
}


// Disable all Distab Functions (e.g. in case off power fail)
// it has to be called when the content of the RAM Area
// where the distab is located might not be correct anymore
// Return values: none (void)
void SER_ETK_Disable_Distabs(void)
{
  // Disable all DISTAB 17 events - Clearing the D17 event list
  #ifdef D17_MAX_EVENT_NO && DISTAB_SUPPORTED
    uint32 eventNum;

    for (eventNum = 0 ; eventNum < D17_MAX_EVENT_NO ; eventNum++)
    {
      Distab17EventList.Config[eventNum] = 0; // The last bit of the event is the event active bit.
    }
    Distab17EventList.Version = 0;
    Distab17EventList.Change  = 0;
    Distab17EventList.First   = 0;
    Distab17EventList.Number  = 0;
  #endif // D17_MAX_EVENT_NO

}


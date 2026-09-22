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

#include "../ETK_Integration_cfg.h"       /* for the configuration and register addresses */
// #include "../../Base.h"         /* for the register addresses */
#include "../endinit.h"         /* for the endinit protection */


#include "ETK_SER_Page_Ctrl_Fct.h"      /* for the ETK page control functions */
#include "../ETK_SER_Handshake.h"     /* for the ETK handshake */

  /** Some definitions for page switching:
  RPREQ -> Reference Page REQuest
  RPACK -> Reference Page configuration ACKnowledge
  RPERR -> Reference Page configuration ERRor
  WPREQ -> Working Page REQuest
  WPACK -> Working Page configuration ACKnowledge
  WPERR -> Working Page configuration ERRor
  MOREQ -> Modify Overlay REQuest
  MOACK -> Modify Overlay memory layout ACKnowledge (Page switch)
  MOERR -> Modify Overlay configuration ERRor
  */

  /* For the OMD mailbox the value by the ETK is given as an uint32 as opposed to the OCT were it was with uint8s.
Therefore the value to be compared will always be the BIG ENDIAN value.  */
  #ifdef OMD_TYPE_DEF
    #define RPACK_PATTERN  0x5250414B    // RPAK
    #define WPACK_PATTERN  0x5750414B    // WPAK
    #define RPREQ_PATTERN  0x52505251    // RPRQ
    #define WPREQ_PATTERN  0x57505251    // WPRQ
    #define WPERR_PATTERN  0x57504552    // WPER
    #define RPERR_PATTERN  0x52504552    // RPER
    #define MOREQ_PATTERN  0x4D4F5251    // MORQ
    #define MOACK_PATTERN  0x4D4F414B    // MOAK
    #define MOERR_PATTERN  0x4D4F4552    // MOER
  #endif




void Disable_Interrupts(void);
void Enable_Interrupts(void);



// **********************************************************
  #ifdef TASKING_COMPILER
    #pragma section all  "ETK_PageSwitch_MemClass"
    #pragma noclear
  #endif // #ifdef TASKING_COMPILER

  #ifdef HIGHTEC_COMPILER
    #pragma section ".ETK_PageSwitch_MemClass" 4
  #endif

  #ifdef TARGET_SH7xxx_Renesas
    #pragma section ETK_PageSwitch_MemClass
  #endif // #ifdef TARGET_SH7xxx_Renesas

  #ifdef OMD_TYPE_DEF
     VOLATILE_DEF struct Page_Switching_Mailbox_Struct Page_Switch_Mailbox ;
  #endif

  #ifdef TASKING_COMPILER
    #pragma clear
    #pragma section all
  #endif // #ifdef TASKING_COMPILER

  #ifdef HIGHTEC_COMPILER
    #pragma section
  #endif

  #ifdef TARGET_SH7xxx_Renesas
    #pragma section
  #endif // #ifdef TARGET_SH7xxx_Renesas


#ifdef TASKING_COMPILER
  #pragma clear
  #pragma section all
#endif // #ifdef TASKING_COMPILER

  #ifdef HIGHTEC_COMPILER
    #pragma section
  #endif

#ifdef TARGET_SH7xxx_Renesas
  #pragma section
#endif // #ifdef TARGET_SH7xxx_Renesas

#ifdef TASKING_COMPILER
  #pragma section all  "ETK_OMDTable_MemClass"
  #pragma noclear
#endif // #ifdef TASKING_COMPILER

  #ifdef HIGHTEC_COMPILER
    #pragma section ".ETK_OMDTable_MemClass" 4
  #endif

#ifdef TARGET_SH7xxx_Renesas
  #pragma section ETK_OMDTable_MemClass
#endif // #ifdef TARGET_SH7xxx_Renesas

#ifdef OMD_TYPE_DEF
  VOLATILE_DEF struct OMD_TABLE  OMD_table ;
#endif

#ifdef TASKING_COMPILER
  #pragma clear
  #pragma section all
#endif // #ifdef TASKING_COMPILER

  #ifdef HIGHTEC_COMPILER
    #pragma section
  #endif

#ifdef TARGET_SH7xxx_Renesas
  #pragma section
#endif // #ifdef TARGET_SH7xxx_Renesas


#ifdef OMD_TYPE_DEF


uint32 firstModifyRequest = 0; // Used to check if a MORQ in the Request mailbox or a WPREQ in the init page is the first executed after a startup so that the RABR, OMASK, and OTAR registers are initialized


void SER_ETK_Page_Init_and_Check_WP(unsigned char P_ProtocolBasedPageSwitch)
{

  VOLATILE_DEF struct OMD_TABLE  *OMD_table_Ptr = &OMD_table;


  // If (Power_Fail_detected (Or no ETK present -> then Powerfail is also set to true))
  // Disable Overlay to make sure ECU runs from Flash
  if (ECU_ETK_Status.ETK_Data_Valid == false) // Data powerfail detected. Invalidate the calibration data, and switch to the RP
  {

    /* Disables the calibration handles. Switch to the RP. */
    Disable_Calibration_Handles();


    if (P_ProtocolBasedPageSwitch)
    {
      Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = RPACK_PATTERN;
    }


  }
  else // no POWERFAIL detected
  {
    if (P_ProtocolBasedPageSwitch) // Protocol Based Page switch enabled
    {
      if (RPREQ_PATTERN == Page_Switch_Mailbox.Start_Page) // Reference page Requested
      {


        // verifies the OMD header (CID, version, number of EMU-Handles defined, and the OMD table size compared to the expected one for the target)
        if ( Check_OMD(OMD_table_Ptr) == 1 ) // OMD Table header correctly defined
        {
          // checks the OMD checksum
          if ( Check_OMD_Checksum(OMD_table_Ptr) == 1 ) // checksum test passed
          {
            Process_OMD_RPREQ_Start_Page(OMD_table_Ptr);

            Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = RPACK_PATTERN;

          }
          else // wrong checksum
          {
            /* Disables the calibration handles. Switch to the RP. */
            Disable_Calibration_Handles();

            Page_Switch_Mailbox.Error_Code = 0xC000000E;  // Start Page RP request - Incorrect OMD - Forced to RP state
            Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = RPACK_PATTERN;

          }
        }
        else // incorrect OMD table header
        {
          /* Disables the calibration handles. Switch to the RP. */
          Disable_Calibration_Handles();

          Page_Switch_Mailbox.Error_Code = 0xC000000E;  // Start Page RP request - Incorrect OMD - Forced to RP state
          Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = RPACK_PATTERN;

        }
      } // Reference page Requested
      else if (WPREQ_PATTERN == Page_Switch_Mailbox.Start_Page) // Working Page  Requested
      {

        #ifdef CODECHECK_WP_BEFORE_PAGESWITCH
        /* Advanced Code check used to check if the ECU pattern was copied by the ETK in the A2L specified RAM region
        Doesn't block the process if it fails with PAGESWITCH_DEPENDING_ON_CODECHECK set to false. Blocks the process if PAGESWITCH_DEPENDING_ON_CODECHECK is set to true.  */
        if (CodeCheck_WP() == 1  || ( ! PAGESWITCH_DEPENDING_ON_CODECHECK ))
        {
        #endif // #ifdef CODECHECK_WP_BEFORE_PAGESWITCH
          // verifies the OMD header (CID, version, number of EMU-Handles defined, and the OMD table size compared to the expected one for the target)
          if ( Check_OMD(OMD_table_Ptr) == 1 ) // OMD Table header correctly defined
          {
            // checks the OMD checksum
            if ( Check_OMD_Checksum(OMD_table_Ptr) == 1 ) // checksum test passed
            {
              // checks if the Activate EMU-Handles in the OMD table only use the available EMU-Handles
              if ( Check_availableHandles(OMD_table_Ptr) == 1 ) // check passed - no unavailable handle activated
              {
                // Processes the OMD table for the Target Adress changes and configures the different calibration registers - WP request specific function
                if ( Process_OMD_WPREQ_Start_Page(OMD_table_Ptr) == 1 ) // process OMD was successful
                {
                  Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = WPACK_PATTERN;

                }
                else // process OMD failed
                {
                  Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = WPERR_PATTERN;

                }
              }
              else // Invalid EMU-Handle
              {
                Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = WPERR_PATTERN;

              }
            }
            else // wrong checksum
            {
              Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = WPERR_PATTERN;

            }
          }
          else // incorrect OMD table header
          {
            Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = WPERR_PATTERN;

          }
        #ifdef CODECHECK_WP_BEFORE_PAGESWITCH
        }
        else // CodeCheck_WP failed and PAGESWITCH_DEPENDING_ON_CODECHECK set to true - Codecheck blocks the page switching if it fails
        {
          Page_Switch_Mailbox.Error_Code = 0xC000000A; // CodeCheck pattern WP failed
          Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = WPERR_PATTERN;

        }
        #endif // #ifdef CODECHECK_WP_BEFORE_PAGESWITCH
      }  // Working Page  Requested
      else // Unknown or Unexpected value
      {

        if (ECU_ETK_Status.ETK_RAM_Valid) // If the Ram is valid, the OMD is not cleared by the ECU and can be read.
        {
          // verifies the OMD header (CID, version, number of EMU-Handles defined, and the OMD table size compared to the expected one for the target)
          if ( Check_OMD(OMD_table_Ptr) == 1 ) // OMD Table header correctly defined
          {
            // checks the OMD checksum
            if ( Check_OMD_Checksum(OMD_table_Ptr) == 1 ) // checksum test passed
            {
              Process_OMD_RPREQ_Start_Page(OMD_table_Ptr); // Initialize the calibration registers if possible

              Page_Switch_Mailbox.Error_Code = 0xC0000008; // Unknown or unexpected pattern found in the page switch mailbox start page
              Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = RPACK_PATTERN;

            }
            else // wrong checksum
            {
              /* Disables the calibration handles. Switch to the RP. */
              Disable_Calibration_Handles();

              Page_Switch_Mailbox.Error_Code = 0xC0000008; // Unknown or unexpected pattern found in the page switch mailbox start page
              Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = RPACK_PATTERN;

            }
          }
          else // incorrect OMD table header
          {
            /* Disables the calibration handles. Switch to the RP. */
            Disable_Calibration_Handles();

            Page_Switch_Mailbox.Error_Code = 0xC0000008; // Unknown or unexpected pattern found in the page switch mailbox start page
            Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = RPACK_PATTERN;

          }
        }
        else // If the EMU RAM was cleared due to an invalid RAM, then the mailbox and the OMD have also been reset. Initialize to the RP in this case.
        {
          /* Disables the calibration handles. Switch to the RP. */
          Disable_Calibration_Handles();

          Page_Switch_Mailbox.Error_Code = 0xC0000009; // Trying to read the mailbox after a RAM invalidation without a DATA invalidation will give this strange error case as the mailbox will also be cleared by the RAM invalidation...
          Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = RPACK_PATTERN;
        }
      }// Unknown or Unexpected value
    } // if Protocol Based Pageswitch
  } // if Data Valid
}


unsigned char SER_ETK_Check_PAGE_SWITCH_BY_ECU(void)
{
  // Check if Pageswitch is necessary
  if (   RPREQ_PATTERN == Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK
      || WPREQ_PATTERN == Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK
      || MOREQ_PATTERN == Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK  )
  {

    return 1; // Page switch requested
  }
  else
  {
    return 0;  // No Page switch requested
  }
}

void SER_ETK_PAGE_SWITCH_BY_ECU(void)
{

  VOLATILE_DEF struct OMD_TABLE  *OMD_table_Ptr = &OMD_table;



  if (RPREQ_PATTERN == Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK) // Reference page Requested
  {

    // verifies the OMD header (CID, version, number of EMU-Handles defined, and the OMD table size compared to the expected one for the target)
    if ( Check_OMD(OMD_table_Ptr) == 1 ) // OMD Table header correctly defined
    {
      // checks the OMD checksum
      if ( Check_OMD_Checksum(OMD_table_Ptr) == 1 ) // checksum test passed
      {
        #ifdef ENDINIT_PROTECTION_DISABLED
          Disable_Calibration_Handles(); // Disables the calibration handles
        #else
          Disable_Interrupts();

          Disable_Calibration_Handles(); // Disables the calibration handles

          Enable_Interrupts();
        #endif

        Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = RPACK_PATTERN;

      }
      else // wrong checksum
      {
        Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = RPERR_PATTERN;

      }
    }
    else // incorrect OMD table header
    {
      Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = RPERR_PATTERN;

    }
  }  // Reference page Requested
  else if (WPREQ_PATTERN == Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK) // Working Page  Requested
  {


    #ifdef CODECHECK_WP_BEFORE_PAGESWITCH
    /* Advanced Code check used to check if the ECU pattern was copied by the ETK in the A2L specified RAM region
        Doesn't block the process if it fails with PAGESWITCH_DEPENDING_ON_CODECHECK set to 0. Blocks the process if PAGESWITCH_DEPENDING_ON_CODECHECK is set to 1.  */
    if (CodeCheck_WP() == 1  || ( ! PAGESWITCH_DEPENDING_ON_CODECHECK ))
    {
    #endif // #ifdef CODECHECK_WP_BEFORE_PAGESWITCH
      // verifies the OMD header (CID, version, number of EMU-Handles defined, and the OMD table size compared to the expected one for the target)
      if ( Check_OMD(OMD_table_Ptr) == 1 ) // OMD Table header correctly defined
      {
        // checks the OMD checksum
        if ( Check_OMD_Checksum(OMD_table_Ptr) == 1 ) // checksum test passed
        {
          // checks if the Activate EMU-Handles in the OMD table only use the available EMU-Handles
          if ( Check_availableHandles(OMD_table_Ptr) == 1 ) // check passed - no unavailable handle activated
          {
            #ifdef ENDINIT_PROTECTION_DISABLED
              Set_Calibration_Handles(OMD_table_Ptr); // Activates/Disables the OMD defined calibration handles
            #else
              Disable_Interrupts();

              Set_Calibration_Handles(OMD_table_Ptr); // Activates/Disables the OMD defined calibration handles

              Enable_Interrupts();
            #endif

            Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = WPACK_PATTERN;

          }
          else // Invalid EMU-Handle
          {
            Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = WPERR_PATTERN;

          }
        }
        else // wrong checksum
        {
          Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = WPERR_PATTERN;

        }
      }
      else // incorrect OMD table header
      {
        Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = WPERR_PATTERN;

      }
    #ifdef CODECHECK_WP_BEFORE_PAGESWITCH
    }
    else // CodeCheck_WP failed and PAGESWITCH_DEPENDING_ON_CODECHECK set to true - Codecheck blocks the page switching if it fails
    {
      Page_Switch_Mailbox.Error_Code = 0xC000000A; // CodeCheck pattern WP failed
      Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = WPERR_PATTERN;

    }
    #endif // #ifdef CODECHECK_WP_BEFORE_PAGESWITCH
  }  // Working Page  Requested
  /* MOREQ is currently not supported on startup. This will switch to the Reference page, and also send a RPACK pattern. */
  else if (MOREQ_PATTERN == Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK) // Modify Request
  {

    // verifies the OMD header (CID, version, number of EMU-Handles defined, and the OMD table size compared to the expected one for the target)
    if ( Check_OMD(OMD_table_Ptr) == 1 ) // OMD Table header correctly defined
    {
      // checks the OMD checksum
      if ( Check_OMD_Checksum(OMD_table_Ptr) == 1 ) // checksum test passed
      {
        // checks if the Activate EMU-Handles in the OMD table only use the available EMU-Handles
        if ( Check_availableHandles(OMD_table_Ptr) == 1 ) // check passed - no unavailable handle activated
        {
          // Processes the OMD table for the Target Adress changes and configures the different calibration registers - Modify request specific function
          if ( Process_OMD_MORQ(OMD_table_Ptr) == 1 ) // process OMD was successful
          {
            Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = MOACK_PATTERN;

          }
          else // process OMD failed
          {
            Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = MOERR_PATTERN;

          }
        }
        else // Invalid EMU-Handle
        {
          Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = MOERR_PATTERN;

        }
      }
      else // wrong checksum
      {
        Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = MOERR_PATTERN;

      }
    }
    else // incorrect OMD table header
    {
      Page_Switch_Mailbox.Page_Switching_Pattern_REQ_ACK = MOERR_PATTERN;

    }
  }  // Modify Request
}


/* Calculates and then checks the OMD checksum and returns 1 if the test passes, or 0 if it fails. The size of the OMD should be a multiple of 4 bytes.
The OMD checksum calculates the checksum with an XOR function starting from the OMD_Size field in the OMD just under the Checksum field and it calculates it until the end of the table. */
uint8 Check_OMD_Checksum(VOLATILE_DEF struct OMD_TABLE * OMD_table_Ptr)
{
  uint32* Ptr_Checksum;
  uint32* EndPtr_Checksum;
  uint32 checksum;
  uint32 OMD_Length_32BitValues = (OMD_table_Ptr->OMD_Size + 16) >> 2; // Adding the OMD header to get the end of the table into the checksum, then dividing by 2 to get the number of 32 bit values

  Ptr_Checksum     = (uint32 *)(OMD_table_Ptr) + 3; // The checksum start value is the OMD_Size value (4th 32bit value in the OMD)
  EndPtr_Checksum  = (uint32 *)(OMD_table_Ptr) + OMD_Length_32BitValues; // points to the end of the OMD

  checksum = 0;

  // Calculate the OMD Checksum
  while (Ptr_Checksum < EndPtr_Checksum )
  {
    checksum = checksum ^ *Ptr_Checksum;
    Ptr_Checksum++;
  }

  if ( checksum == OMD_table_Ptr->Checksum ) // calculated checksum corresponds to the checksum in the OMD table
  {
    return 1;  // Checksum test passed
  }
  else // wrong checksum value
  {
    Page_Switch_Mailbox.Error_Code = 0xC0000005; // Checksum of OMD is invalid

    return 0;  // Checksum test failed
  }
}

/* Checks the OMD table to see if the defined value for the EMU handles number corresponds to the maximum supported by the target ECU.
  Also checks if the OMD_Size is the same as the one expected by the ECU.
  Returns 1 upon success, or 0 if the test fails.
  The CID and OMD_Size values can be read with the 0 value in the initial startup phase. This would meand that the OMD table is empty and was not set up by the ETK.
*/
uint8 Check_OMD(VOLATILE_DEF struct OMD_TABLE * OMD_table_Ptr)
{

  if (OMD_table_Ptr->CID != IMPLEMENTED_OMD_CID) // Incorrect CID of the OMD - Currently implemented value is 2 - Indicates incompatible layout changes.
  {
    if (OMD_table_Ptr->CID == 0 && OMD_table_Ptr->OMD_Size == 0) //  The OMD table is empty and was not set up by the ETK.
    {
      // This can happen in the initial ECU startup phase and should not always be considered as an error. Therefore we don't use an error code in the mailbox in this case.
      return 0;
    }
    else // The OMD table was set up by the ETK with a wrond CID.
    {
      Page_Switch_Mailbox.Error_Code = 0xC0000001; // Incorrect CID of the OMD

      return 0;
    }
  }
  else if (OMD_table_Ptr->Version != IMPLEMENTED_OMD_VERSION) //  Incorrect extension version of the CID in the OMD - Currently implemented value is 0 - Compatible extensions check for an existing CID.
  {
    Page_Switch_Mailbox.Error_Code = 0xC0000002; //  Incorrect version of the OMD

    return 0;
  }
  else if (OMD_table_Ptr->EMU_Handles_Number > NUMBER_OMD_MAX_CALIB_HANDLES) // Exceeded the maximum number of calibration handles value
  {
    Page_Switch_Mailbox.Error_Code = 0xC0000003; // Exceeded the maximum number of calibration handles value

    return 0;
  }
  // The following check is for the OMDv2 table (CID 2) only and will return an error when using the smaller OMDv1 (CID 1) table.
  else if (OMD_table_Ptr->OMD_Size != ((OMD_table_Ptr->EMU_Handles_Number * 12) + 4 + (16* NUMBER_4BYTE_VALUES_PER_CALIB_DEFINITION))) // Incorrect OMD table size
  {
    Page_Switch_Mailbox.Error_Code = 0xC0000004; // Incorrect OMD table size defined


    return 0;
  }
  else // The OMD table is correctly defined. -> Check passed
  {

    return 1;
  }
}

/* Returns 1 if no unavailable EMU handle are activated in the OMD, and 0 if an illegal activation is found. */
uint8 Check_availableHandles (VOLATILE_DEF struct OMD_TABLE * OMD_table_Ptr)
{
  uint32 compareTmp;
  uint32 result = 0;
  int i;


  for ( i=0; i < NUMBER_4BYTE_VALUES_PER_CALIB_DEFINITION ; i++)
  {
    compareTmp = OMD_table_Ptr->Available_EMU_Handles[i];
    // If we try to activate a non available handle we will get a value different from 0 after an XOR.
    result |= ( compareTmp | OMD_table_Ptr->Activate_EMU_Handles[i] ) ^ compareTmp;
  }

  if (result == 0) // no unavailable EMU handle activated
  {

    return 1;
  }
  else
  {
    Page_Switch_Mailbox.Error_Code = 0xC0000006; // Not available EMU-Handle activated in OMD

    return 0;
  }
}


/* Returns an OR - Accumulated value of the activated handles.
If the returned value is 0 then no handles are activated in the OMD table. */
uint32 Get_Activated_EMU_Handles_Accum (VOLATILE_DEF struct OMD_TABLE * OMD_table_Ptr)
{
  uint32 result = 0;
  int i;

    for ( i=0; i < NUMBER_4BYTE_VALUES_PER_CALIB_DEFINITION ; i++)
  {
    result |= OMD_table_Ptr->Activate_EMU_Handles[i] ;
  }

  return result;
}


/* Updates the RABR, OTAR, and OMASK registers in all the CPUs with the defined values of the OMD if required.
The RABR and OMASK registers are set up at the first execution of this function after a restart.
The tiles are also set up and configured for calibration access if available according to the Allocate EMU-Handle value of the OMD.
Returns 1 upon success, or 0 if it fails. */
uint8 Process_OMD_MORQ (VOLATILE_DEF struct OMD_TABLE * OMD_table_Ptr)
{

  // Initial configuration of the RABR and OMASK registers.
  if (firstModifyRequest == 0)
  {

    #ifdef ENDINIT_PROTECTION_DISABLED
      // Configures the initial value of the RABR, OTAR and OMASK registers used for the calibration configuration, and sets the OVC_ENABLE value for the required CPU cores
      Initialize_Calib_Registers();
    #else
      Disable_Interrupts();
      // Configures the initial value of the RABR, OTAR and OMASK registers used for the calibration configuration, and sets the OVC_ENABLE value for the required CPU cores
      Initialize_Calib_Registers();

      Enable_Interrupts();
    #endif

    firstModifyRequest = 1; // first step of the first Modify request processed
  }

  #ifndef ENDINIT_PROTECTION_DISABLED
    Disable_Interrupts();
  #endif

  /** In some cases the OMD table is set up during the startup of the ECU with the activate EMU-Handles bit set for the WP mode.
  The table contains the correct state for the WP mode calibration handle activation.
  However, the problem is that the Working page request (WPRQ) comes only after a Modify Request (MORQ) in the case of the XETK-S20.
  And the calibration handles should only be disabled during a MORQ.
  The activation of the WP mode will be done after through a WPRQ written in the mailbox by the ETK.
  Thus we check here if the calibration mode is set to the reference page (RP), which means that no emulation is currently activated.
  If we are in RP mode, the calibration handles are not set with the value present in the OMD table as all the handles are disabled. */
  if (Check_Calibration_Handles_in_RP_mode() != 1) // Only updates the calibration handles activation register if the ECU is not in RP.
  {
    // Sets the calibration handles to Activate/Disabled state
    Set_Calibration_Handles(OMD_table_Ptr); // Activates/Disables the OMD defined calibration handles
  }

  if (firstModifyRequest == 1) // first Modifiy request currently processed - initializing all the OTAR registers
  {
    // Configures ALL the OTAR registers with the OMD target addresses for the emulation target addresses
    Update_All_OTAR(OMD_table_Ptr);

    firstModifyRequest = 2; // first Modify request completely processed
  }
  else // standard modify request - only updating the OTAR registers with the modify bits set to 1
  {
    // Configures the OTAR register with the modifications in the OMD for the emulation target addresses
    Update_OTAR(OMD_table_Ptr);
  }

  #ifndef ENDINIT_PROTECTION_DISABLED
    Enable_Interrupts();
  #endif

  return 1;
}



/* Start page WPREQ specific function - Same as Process_OMD_MORQ but the Set_Calibration_Handles function is executed after the Update_OTAR function !!
This function should only be called in the handshake procedure, because the tiles are not configured inside it.
Updates the RABR, OTAR, and OMASK registers in all the CPUs with the defined values of the OMD if required.
The RABR and OMASK registers are set up at the first execution of this function after a restart.
The tiles don't need to be set up and configured for calibration access because this step has already been executed during the previous step of the handshake when checking for the RAM_Valid bit.
Returns 1 upon success, or 0 if it fails. */
uint8 Process_OMD_WPREQ_Start_Page (VOLATILE_DEF struct OMD_TABLE * OMD_table_Ptr)
{
  // Initial configuration of the RABR and OMASK registers.

  if (firstModifyRequest == 0)
  {

    #ifdef ENDINIT_PROTECTION_DISABLED
      // Configures the initial value of the RABR, OTAR and OMASK registers used for the calibration configuration, and sets the OVC_ENABLE value for the required CPU cores
      Initialize_Calib_Registers();
    #else
      Disable_Interrupts();
      // Configures the initial value of the RABR, OTAR and OMASK registers used for the calibration configuration, and sets the OVC_ENABLE value for the required CPU cores
      Initialize_Calib_Registers();

      Enable_Interrupts();
    #endif

    firstModifyRequest = 1; // first step of the first WP request in the start page processed
  }


  #ifndef ENDINIT_PROTECTION_DISABLED
    Disable_Interrupts();
  #endif

  if (firstModifyRequest == 1) // first Modifiy request currently processed - initializing all the OTAR registers
  {
    // Configures ALL the OTAR registers with the OMD target addresses for the emulation target addresses
    Update_All_OTAR(OMD_table_Ptr);

    firstModifyRequest = 2; // first WP request in the start page completely processed
  }
  else // standard modify request - only updating the OTAR registers with the modify bits set to 1
  {
    // TODO remove if not needed for second handshake ?
    // Configures the OTAR register with the modifications in the OMD for the emulation target addresses
    Update_OTAR(OMD_table_Ptr);
  }

  // Sets the calibration handles to Activate/Disabled state
  Set_Calibration_Handles(OMD_table_Ptr); // Activates/Disables the OMD defined calibration handles

  #ifndef ENDINIT_PROTECTION_DISABLED
    Enable_Interrupts();
  #endif

  return 1;
}


/* Start page RPREQ specific function - Same as Process_OMD_MORQ but the Set_Calibration_Handles function is executed after the Update_OTAR function !!
This function should only be called in the handshake procedure, because the tiles are not configured inside it.
This function will only set up the calibratoin registers if needed and then disable them.
Updates the RABR, OTAR, and OMASK registers in all the CPUs with the defined values of the OMD if required.
The RABR and OMASK registers are set up at the first execution of this function after a restart.
The tiles don't need to be set up and configured for calibration access because this step has already been executed during the previous step of the handshake when checking for the RAM_Valid bit.
Returns 1 upon success, or 0 if it fails. */
uint8 Process_OMD_RPREQ_Start_Page (VOLATILE_DEF struct OMD_TABLE * OMD_table_Ptr)
{
  // Initial configuration of the RABR and OMASK registers.

  if (firstModifyRequest == 0)
  {

    #ifdef ENDINIT_PROTECTION_DISABLED
      // Configures the initial value of the RABR, OTAR and OMASK registers used for the calibration configuration, and sets the OVC_ENABLE value for the required CPU cores
      Initialize_Calib_Registers();
    #else
      Disable_Interrupts();
      // Configures the initial value of the RABR, OTAR and OMASK registers used for the calibration configuration, and sets the OVC_ENABLE value for the required CPU cores
      Initialize_Calib_Registers();

      Enable_Interrupts();
    #endif

    firstModifyRequest = 1; // first step of the first WP request in the start page processed
  }


  #ifndef ENDINIT_PROTECTION_DISABLED
    Disable_Interrupts();
  #endif

  if (firstModifyRequest == 1) // first Modifiy request currently processed - initializing all the OTAR registers
  {
    // Configures ALL the OTAR registers with the OMD target addresses for the emulation target addresses
    Update_All_OTAR(OMD_table_Ptr);

    firstModifyRequest = 2; // first WP request in the start page completely processed
  }

  // Disables the calibration handles
  Disable_Calibration_Handles();

  #ifndef ENDINIT_PROTECTION_DISABLED
    Enable_Interrupts();
  #endif

  return 1;
}

#endif //#ifdef OMD_TYPE_DEF


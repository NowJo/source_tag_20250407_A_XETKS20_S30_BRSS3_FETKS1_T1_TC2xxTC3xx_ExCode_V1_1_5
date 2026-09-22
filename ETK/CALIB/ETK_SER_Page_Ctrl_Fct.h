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


#ifndef _ETK_SER_PAGE_CTRL_FKTs_H
#define _ETK_SER_PAGE_CTRL_FKTs_H

/* ---------------------------------------------------------------------------*/
/* This file is used for Page Control header Info                             */
/*                                                                            */
/*                                                                            */
/* ---------------------------------------------------------------------------*/

#include "../ETK_Integration_cfg.h"   /* for the configuration */


#ifdef OMD_TC2XX
  #if !(defined OMD_TYPE_DEF || defined OCT_TYPE_DEF)
    #define OMD_TYPE_DEF
  #else
    #error  "MULTIPLE OCT_TYP and/or OMD_TYP Definitions !!"
  #endif
#endif

#ifdef OMD_TYPE_DEF

  /* OMD specific structures with a specific mailbox containing an error code amd an OMD table instead of the OCT table. */
   struct Page_Switching_Mailbox_Struct {
          uint32  Page_Switching_Pattern_REQ_ACK;  // Page Switch Mailbox for the patterns MORQ, MOAK, MOER, WPRQ, WPAK, WPER, RPRQ, RPAK, RPER
          uint32  Error_Code;                      // Error Code defined in the A2L file
          uint32  Start_Page;                      // Defined Start Page ("WP" enforces the preceding configuration of the Overlay Mechanism)
         };

  // OMD table for the version 2 of the CID
   struct OMD_TABLE {
        // OMD HEADER
          uint32  CID;		// CID of the OMD
          uint32  Version;		// Version of the OMD table (defined with the MBX_Version value in the A2L file) -> Version 1 does not support MORQ/MOAK/MOER and Version 2 supports it
          uint32  Checksum;			// Checksum of the OMD body without most of the header structure but with the OMD_Size
          uint32  OMD_Size;			// Size of the OMD body without the header
        // OMD BODY
          uint32 EMU_Handles_Number;  // Number of EMU-Handles specified by the structure
          uint32 Available_EMU_Handles[NUMBER_4BYTE_VALUES_PER_CALIB_DEFINITION]; // Available calibration handles defined in the A2L file
          uint32 Allocate_EMU_Handles[NUMBER_4BYTE_VALUES_PER_CALIB_DEFINITION];  // Allocate the calibration handles (set the corresponding tile to calibration mode)
          uint32 Activate_EMU_Handles[NUMBER_4BYTE_VALUES_PER_CALIB_DEFINITION];  // Enable/Disable the corresponding calibration handle
          uint32 Modify_EMU_Handles[NUMBER_4BYTE_VALUES_PER_CALIB_DEFINITION];    // Modify the Target Address of the corresponding calibration handle
          uint32 Target_Address[3 * NUMBER_OMD_MAX_CALIB_HANDLES];  // Target/Emulation Address and size of the corresponding calibration handle
          };

  /* Extern structure instance defines for OMD - Used by ETK_SER_User_Spec_Fkts.c */
  extern VOLATILE_DEF struct Page_Switching_Mailbox_Struct Page_Switch_Mailbox ;
  extern VOLATILE_DEF struct OMD_TABLE  OMD_table ;


  /* Not Target specific function defines for OMD. */
  void SER_ETK_Page_Init_and_Check_WP(unsigned char P_ProtocolBasedPageSwitch);
  unsigned char SER_ETK_Check_PAGE_SWITCH_BY_ECU(void);
  void SER_ETK_PAGE_SWITCH_BY_ECU(void);
  uint8 Check_OMD_Checksum(VOLATILE_DEF struct OMD_TABLE * OMD_table_Ptr);
  uint8 Check_OMD(VOLATILE_DEF struct OMD_TABLE * OMD_table_Ptr);
  uint8 Check_availableHandles (VOLATILE_DEF struct OMD_TABLE * OMD_table_Ptr);
  uint32 Get_Activated_EMU_Handles_Accum (VOLATILE_DEF struct OMD_TABLE * OMD_table_Ptr);
  uint8 Process_OMD_MORQ (VOLATILE_DEF struct OMD_TABLE * OMD_table_Ptr);
  uint8 Process_OMD_WPREQ_Start_Page (VOLATILE_DEF struct OMD_TABLE * OMD_table_Ptr);
  uint8 Process_OMD_RPREQ_Start_Page (VOLATILE_DEF struct OMD_TABLE * OMD_table_Ptr);

  /* Tricore Specific - Target specific function defines for OMD. */
  #ifdef OMD_TC2XX

    /* Target Tricore TC2D5 CPU Class Specific functions (more generic) */
    void Init_RABR (uint32 *registerPtr);
    void Init_OTAR (uint32 *registerPtr);
    void Init_OMASK (uint32 *registerPtr);


    /* Target Tricore TC2D5 CPU Specific functions */
    void Update_OTAR (VOLATILE_DEF struct OMD_TABLE * OMD_table_Ptr);
    void Update_All_OTAR (VOLATILE_DEF struct OMD_TABLE * OMD_table_Ptr);
    void Initialize_Calib_Registers(void);
    void Disable_Calibration_Handles (void);
    void Set_Calibration_Handles (VOLATILE_DEF struct OMD_TABLE * OMD_table_Ptr);
    uint8 Check_Calibration_Handles_in_RP_mode(void);
    
    void dataCacheInvalidate(void);

  #endif // #ifdef OMD_TC2XX

#endif // #ifdef OMD_TYPE_DEF



#endif // #ifndef _ETK_SER_PAGE_CTRL_FKTs_H





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

#ifndef _ETK_SER_HANDSHAKE_H_
#define _ETK_SER_HANDSHAKE_H_

#include "../a_std_type.h"      /* for the standard types */
#include "ETK_Integration_Cfg.h"    /* for the configuration */


    /* ETK Detection Pattern Protocol
    Different access methods: ETK_Detection_Pattern_Protocol.ProtocolType or ETK_Detection_Pattern_Protocol.U (all the value)*/
    union ETK_Detection_Pattern_Protocol
    {
      struct
      {
        unsigned int ETKRequest : 1; /* If the ETK writes to the COMDATA this will always be set to 1. This way we can see if the COMDATA was written by the ECU. */
        unsigned int DataValid : 1; /* If data powerfail detected : DataValid == 0. Invalidate the calibration data, and switch to the RP */
        unsigned int RAMValid : 1; /* If RAM powerfail detected : RAMValid == 0. Invalidate the Distab and bypass functions */
        unsigned int MCWait : 1; /* If MCWait == 1. Wait for MC configuration in the coldstart. Coldstart measurement wil be configured after the handshake. */
        unsigned int RPWait : 1; /* If RPWait == 1.  Wait for RP configuration in the coldstart. Bypass will be configured after the handshake. */
        unsigned int ESMasterPresent: 1; /* If ESMasterPresent == 1. ES device present to control the ETK */
        unsigned int CalWakeUp : 1; /* If CalWakeUp == 1. CalWakeUp activated*/
        unsigned int FlashProgRequest: 1; /* If FlashProgRequest == 1. Flash programming was requested by the tool */
        unsigned int ProtocolSpecificParameters: 8; /* Reserved for future use */
        unsigned int ETKPresent : 1; /* The ETK is present. */
        unsigned int DebuggerPresent : 1; /* The debugger is present. */
        unsigned int ToolType : 6; /* Reserved for future use. Tool type to distinguish the different present tools. */
        unsigned int ProtocolType: 8; /* Protocol type to distinguish between the different communication protocols and purposes. */
      } B;
        unsigned int U;
    };



    struct ECU_ETK_Protocol_Status {
        uint8 Protocol_Success;    // Sucessful read of the ETK Detection Protocol
        uint8 Handshake_End_State;  // How the handshake ended: 0 -> COMDATA cleared by the ETK correctly, 1 -> COMDATA cleared by the ECU after a timeout, 2 -> COMDATA cleared by the ETK before the ECU wrote its COMDATA handshake part
        uint8 ETK_Available;       // ETK detected during the handshake
        uint8 Debugger_Available;  // Debugger detected during the handshake
        uint8 ETK_Data_Valid;      // 1 -> Data Valid - 0-> Data powerfail detected. Invalidate the calibration data, and switch to the RP
        uint8 ETK_RAM_Valid;       // 1 -> RAM Valid - 0 -> RAM powerfail detected. Invalidate the Distab and bypass functions
        uint8 ETK_MC_Wait;         // Wait for MC configuration in the coldstart. Coldstart measurement wil be configured after the handshake.
        uint8 ETK_RP_Wait;         // Wait for RP configuration in the coldstart. Bypass will be configured after the handshake.
        uint8 ETK_ES_Master_Present;  // ES device present to control the ETK
        uint8 ETK_CalWakeUp;       // CalWakeUp activated
        uint8 ETK_Flash_Request;   // Flash programming was requested by the tool
    };

    extern struct ECU_ETK_Protocol_Status ECU_ETK_Status;

    extern uint32 ETK_Protocol_Handshake_End_Timing_Start;
    extern uint32 ETK_Protocol_Handshake_End_Timing_End;
    extern uint32 ETK_Detection_Protocol_Timing_Start;
    extern uint32 ETK_Detection_Protocol_Timing_End;
    extern uint32 CRSYNC_Detection_Timing_Start;
    extern uint32 CRSYNC_Detection_Timing_End;



    void Start_Timing_Handshake_Protocol_End(void);
    uint8 Check_Timeout_Handshake_Protocol_End(void);
    void Start_Timing_ETK_Detection_Protocol(void);
    uint8 Check_Timeout_ETK_Detection_Protocol(void);
    void Start_Timing_CRSYNC_Detection(void);
    uint8 Check_Timeout_CRSYNC_Detection(void);

    unsigned char SER_ETK_Detect(void);
    void SER_Initial_Handshake_Execute(void);
    void SER_Cyclic_Handshake_Check_And_Execute(void);
    uint16 SER_ETK_Detection_Protocol_Handshake(void);
    uint16 SER_ETK_Check_Protocol_Handshake(void);
    uint8 SER_ETK_Handshake_End(void);
    void SER_ETK_RAM_Validity_Process(void);
    void Configure_EMEM_Tiles_For_ECU_RAM(void) __attribute__ ((used)) ;
    void EMEM_RAM_ECC_Initialize(void);
    void Enable_OCDS_Triggers(void);

    void SER_ETK_Disable_Distabs(void);

#endif /* _ETK_SER_HANDSHAKE_H_ */
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


#include "../ETK_Integration_cfg.h"     /* for the configuration */
// #include "../../Base.h"                 /* for the register addresses */
#include "ETK_SER_Page_Ctrl_Fct.h"      /* for the ETK page control functions */
#include "../ETK_SER_Handshake.h"       /* for the ETK handshake */
#include "../endinit.h"                 /* for the endinit protection */

#ifdef HIGHTEC_COMPILER
  #include <machine/intrinsics.h> // reference to the __mfcr(), __enable() function
#endif



/** Disables interrupts. This is needed when accessing ENDINIT protected registers for the page switching.
This could be replaced by a code raising the processor priority level.
It is important to check that the ENDINIT watchdog does not run into a timeout until the ENDINIT protection is enabled again. */
void Disable_Interrupts(void)
{
  #ifdef ASM_CODE_ENABLE_DISABLE
    #ifdef HIGHTEC_COMPILER
      __asm__("disable");
    #else
      __asm("disable");
    #endif
  #else
    #ifdef HIGHTEC_COMPILER
      _disable();
    #else
      __disable();
    #endif
  #endif
}

/** Enables interrupts. This is needed when accessing ENDINIT protected registers for the page switching.
This could be replaced by a code lowering the processor priority level.
It is important to check that the ENDINIT watchdog does not run into a timeout until the ENDINIT protection is enabled again. */
void Enable_Interrupts(void)
{
  #ifdef ASM_CODE_ENABLE_DISABLE
    #ifdef HIGHTEC_COMPILER
      __asm__("enable");
    #else
      __asm("enable");
    #endif
  #else
    #ifdef HIGHTEC_COMPILER
      _enable();
    #else
      __enable();
    #endif
  #endif
}


/** Initializes the RABR registers for a CPU starting from the given address.
This will configure the calibration handles to disable the calibration, and to select the EMURAM (or DSPR RAM for a PD device) as the base emulation memory used for the calibration.
It will also configure the base address to be used in the EMURAM for each handle.
The base address offset is defined with the CALIB_HANDLE_SIZE macro.
The base address may be changed by a modify request when using dynamic calibration. */

void Init_RABR (uint32 *registerPtr)
{
    uint32 oBaseOffset = 0;
    int i;

    for ( i=0; i< NUMBER_OMD_MAX_CALIB_HANDLES; i++)
    {
      #ifdef CPUCLASS_TC3XX
        #ifdef PD_DEVICE
          #ifdef CPUTYPE_TC38X
            *registerPtr = (0x08000000 | oBaseOffset) ; // Disable overlay, redirection to LMU, and OBase Address
          #elif defined CPUTYPE_TC36X
            *registerPtr = (0x08000000 | oBaseOffset) ; // Disable overlay, redirection to LMU, and OBase Address
          #elif defined CPUTYPE_TC37X
            *registerPtr = (0x08000000 | oBaseOffset) ; // Disable overlay, redirection to LMU, and OBase Address
          #elif defined CPUTYPE_TC33X
            *registerPtr = (0x08000000 | oBaseOffset) ; // Disable overlay, redirection to LMU, and OBase Address
          #else
            #error "Undefined RABR initial configuration for the currently used cpu type !"
          #endif
        #else
          *registerPtr = (0x09000000 | oBaseOffset) ; // Disable overlay, redirection to EMEM, and OBase Address
        #endif
      #endif

      registerPtr += 3; // 3 registers offset of 0x0C
      oBaseOffset += CALIB_HANDLE_SIZE; // Calibration handle size added as offset for the next handle.
    }
}

/** Initializes the OTAR registers for a CPU starting from the given address.
This will reset all the emulation target addresses to 0x00000000 in order to prevent an inconsistency with an earlier state. */
void Init_OTAR (uint32 *registerPtr)
{
    int i;

    for ( i=0; i< NUMBER_OMD_MAX_CALIB_HANDLES; i++)
    {
      *registerPtr = 0x0 ; // OTAR initialized to 0 value (initial reset value)
      registerPtr += 3; // 3 registers offset of 0x0C
    }
}

/** Initializes the OMASK registers for a CPU starting from the given address.
This will define the size of the calibration handles and does't need to be modified after the initial configuration.
The value written to the register can be defined with the  OMASK_TRICORE_CALIB_HANDLE_CONFIG macro.*/
void Init_OMASK (uint32 *registerPtr)
{
    int i;

    for ( i=0; i< NUMBER_OMD_MAX_CALIB_HANDLES; i++)
    {
      *registerPtr = OMASK_TRICORE_CALIB_HANDLE_CONFIG ; // Set ONE and OMASK values to the value corresponding to the desired calibration handle size
      registerPtr += 3; // 3 registers offset of 0x0C
    }
}

/** This function returns the configuration value for the RABR register based on the address of the used overlay RAM memory given in its input. */
static uint32 RABR_Configuration (uint32 overlayRamAddr)
{
  uint32 config = 0x0; // RABR reset value
  uint32 redirectAddr = ((overlayRamAddr & 0xFF000000) >> 24);
  uint32 redirectConfig;

  switch (redirectAddr)
  {
    #ifdef CPUCLASS_TC3XX
      case 0xB9:
      case 0x99:
        redirectConfig = 0x09000000; // redirection to EMEM
        break;

      case 0xB0:
      case 0x90:
        redirectConfig = 0x08000000; // redirection to LMU
        break;

      case 0x70:
        redirectConfig = 0x00000000; // redirection to Core 0 DSPR/PSPR memory
        break;
      case 0x60:
        redirectConfig = 0x01000000; // redirection to Core 1 DSPR/PSPR memory
        break;
      case 0x50:
        redirectConfig = 0x02000000; // redirection to Core 2 DSPR/PSPR memory
        break;
      case 0x40:
        redirectConfig = 0x03000000; // redirection to Core 3 DSPR/PSPR memory
        break;
      case 0x30:
        redirectConfig = 0x04000000; // redirection to Core 4 DSPR/PSPR memory
        break;
      case 0x10:
        redirectConfig = 0x05000000; // redirection to Core 5 DSPR/PSPR memory
        break;

      default:
        // ERROR - Setting the default value to LMU/EMEM redirection
        #ifdef PD_DEVICE
          #ifdef CPUTYPE_TC38X
            redirectConfig = 0x08000000; // redirection to LMU
          #elif defined CPUTYPE_TC37X
            redirectConfig = 0x08000000; // redirection to LMU
          #elif defined CPUTYPE_TC36X
            redirectConfig = 0x08000000; // redirection to LMU
          #elif defined CPUTYPE_TC33X
            redirectConfig = 0x08000000; // redirection to LMU
          #else
            #error "Undefined default overlay RAM initial configuration for the currently used cpu type !"
          #endif
        #else
          redirectConfig = 0x09000000; // redirection to EMEM
        #endif
        break;
    #else
      #error "Missing RABR configuration code for the current cpu class !"
    #endif
  }

  config = (redirectConfig | (overlayRamAddr & 0x003fffe0)) ; // Disable overlay, redirection to selected RAM, and OBase Address

  return config;
}

/** This function returns the configuration value for the OMASK register based on the handle size (in bytes) requested in its input. */
static uint32 OMASK_Configuration (uint32 handleSize)
{
  uint32 config = 0x0fffffe0; // reset value with 32 byte block

  switch (handleSize)
  {
    case 0x20000: // 128 KB
      config = 0x0ffe0000;
      break;
    case 0x10000:  // 64 KB
      config = 0x0fff0000;
      break;

    case 0x8000:  // 32 KB
      config = 0x0fff8000;
      break;
    case 0x4000:  // 16 KB
      config = 0x0fffc000;
      break;
    case 0x2000:  // 8 KB
      config = 0x0fffe000;
      break;
    case 0x1000:  // 4 KB
      config = 0x0ffff000;
      break;

    case 0x800:   // 2 KB
      config = 0x0ffff800;
      break;
    case 0x400:   // 1 KB
      config = 0x0ffffc00;
      break;
    case 0x200:   // 512 bytes
      config = 0x0ffffe00;
      break;
    case 0x100:   // 256 bytes
      config = 0x0fffff00;
      break;

    case 0x80:    // 128 bytes
      config = 0x0fffff80;
      break;
    case 0x40:    // 64 bytes
      config = 0x0fffffc0;
      break;
    case 0x20:    // 32 bytes
      config = 0x0fffffe0;
      break;

    default :
    /* ERROR - Using 32 byte size reset value if this is reached due to an incorrect configuration of the OMD table. */
      config = 0x0fffffe0;
      break;
  }

  return config;
}


/** Updates the OTAR registers for all the CPUs if a modify bit is given in the OMD.
This will update the emulation target addresses if it is requested by the corresponding  modify bit in the OMD. */
void Update_OTAR (VOLATILE_DEF struct OMD_TABLE * OMD_table_Ptr)
{
  int i, relativeCalibHandlePos;
  uint32 RABRConfig, OTARConfig, OMASKConfig, oBaseOffset, modifyEMUHandles=0, modifyHandleValue;


  for ( i=0; i < NUMBER_OMD_MAX_CALIB_HANDLES; i++)
  {
    relativeCalibHandlePos = i%32;
    if ( relativeCalibHandlePos == 0) // updates the modify value with the right one if the modify table is bigger than 32 bits
    {
      modifyEMUHandles = OMD_table_Ptr->Modify_EMU_Handles[i/32];
    }

    modifyHandleValue = (modifyEMUHandles >> relativeCalibHandlePos) & 0x00000001;

    if (modifyHandleValue) // calibration handles has to be allocated
    {
      /** RABR configuration */
      oBaseOffset = OMD_table_Ptr->Target_Address[3*i]; // Physical Address: The address of the overlay memory (RAM) that shall overlay another memory range

      RABRConfig = RABR_Configuration(oBaseOffset);

      #ifdef CPUTYPE_TC33X
        *((uint32 *)(&CPU0_RABR0 + (3*i))) = RABRConfig;
      #elif (defined CPUTYPE_TC36X)
        *((uint32 *)(&CPU0_RABR0 + (3*i))) = RABRConfig;
        *((uint32 *)(&CPU1_RABR0 + (3*i))) = RABRConfig;
      #elif (defined CPUTYPE_TC37X)
        *((uint32 *)(&CPU0_RABR0 + (3*i))) = RABRConfig;
        *((uint32 *)(&CPU1_RABR0 + (3*i))) = RABRConfig;
        *((uint32 *)(&CPU2_RABR0 + (3*i))) = RABRConfig;
      #elif (defined CPUTYPE_TC38X)
        *((uint32 *)(&CPU0_RABR0 + (3*i))) = RABRConfig;
        *((uint32 *)(&CPU1_RABR0 + (3*i))) = RABRConfig;
        *((uint32 *)(&CPU2_RABR0 + (3*i))) = RABRConfig;
        *((uint32 *)(&CPU3_RABR0 + (3*i))) = RABRConfig;
      #elif (defined CPUTYPE_TC39X)
        *((uint32 *)(&CPU0_RABR0 + (3*i))) = RABRConfig;
        *((uint32 *)(&CPU1_RABR0 + (3*i))) = RABRConfig;
        *((uint32 *)(&CPU2_RABR0 + (3*i))) = RABRConfig;
        *((uint32 *)(&CPU3_RABR0 + (3*i))) = RABRConfig;
        *((uint32 *)(&CPU4_RABR0 + (3*i))) = RABRConfig;
        *((uint32 *)(&CPU5_RABR0 + (3*i))) = RABRConfig;
      #else
        #error "Missing RABR configuration code for the current cpu type !"
      #endif


      /** OTAR configuration */
      OTARConfig = ( OMD_table_Ptr->Target_Address[3*i+1] & 0x0fffffe0 ); // Virtual Address: The address of the memory (read-only flash) that will be emulated

      #ifdef CPUTYPE_TC33X
        *((uint32 *)(&CPU0_OTAR0 + (3*i))) = OTARConfig;
      #elif (defined CPUTYPE_TC36X)
        *((uint32 *)(&CPU0_OTAR0 + (3*i))) = OTARConfig;
        *((uint32 *)(&CPU1_OTAR0 + (3*i))) = OTARConfig;
      #elif (defined CPUTYPE_TC37X)
        *((uint32 *)(&CPU0_OTAR0 + (3*i))) = OTARConfig;
        *((uint32 *)(&CPU1_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
        *((uint32 *)(&CPU2_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
      #elif (defined CPUTYPE_TC38X)
        *((uint32 *)(&CPU0_OTAR0 + (3*i))) = OTARConfig;
        *((uint32 *)(&CPU1_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
        *((uint32 *)(&CPU2_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
        *((uint32 *)(&CPU3_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
      #elif (defined CPUTYPE_TC39X)
        *((uint32 *)(&CPU0_OTAR0 + (3*i))) = OTARConfig;
        *((uint32 *)(&CPU1_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
        *((uint32 *)(&CPU2_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
        *((uint32 *)(&CPU3_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
        *((uint32 *)(&CPU4_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
        *((uint32 *)(&CPU5_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
      #else
        #error "Missing OTAR configuration code for the current cpu type !"
      #endif


      /** OMASK configuration */
      OMASKConfig = OMASK_Configuration(OMD_table_Ptr->Target_Address[3*i+2]);

      #ifdef CPUTYPE_TC33X
        *((uint32 *)(&CPU0_OMASK0 + (3*i))) = OMASKConfig;
      #elif (defined CPUTYPE_TC36X)
        *((uint32 *)(&CPU0_OMASK0 + (3*i))) = OMASKConfig;
        *((uint32 *)(&CPU1_OMASK0 + (3*i))) = OMASKConfig;
      #elif (defined CPUTYPE_TC37X)
        *((uint32 *)(&CPU0_OMASK0 + (3*i))) = OMASKConfig;  // writes the config to the OMASK register corresponding to the handle number
        *((uint32 *)(&CPU1_OMASK0 + (3*i))) = OMASKConfig;
        *((uint32 *)(&CPU2_OMASK0 + (3*i))) = OMASKConfig;
      #elif (defined CPUTYPE_TC38X)
        *((uint32 *)(&CPU0_OMASK0 + (3*i))) = OMASKConfig;  // writes the config to the OMASK register corresponding to the handle number
        *((uint32 *)(&CPU1_OMASK0 + (3*i))) = OMASKConfig;
        *((uint32 *)(&CPU2_OMASK0 + (3*i))) = OMASKConfig;
        *((uint32 *)(&CPU3_OMASK0 + (3*i))) = OMASKConfig;
      #elif (defined CPUTYPE_TC39X)
        *((uint32 *)(&CPU0_OMASK0 + (3*i))) = OMASKConfig;  // writes the config to the OMASK register corresponding to the handle number
        *((uint32 *)(&CPU1_OMASK0 + (3*i))) = OMASKConfig;
        *((uint32 *)(&CPU2_OMASK0 + (3*i))) = OMASKConfig;
        *((uint32 *)(&CPU3_OMASK0 + (3*i))) = OMASKConfig;
        *((uint32 *)(&CPU4_OMASK0 + (3*i))) = OMASKConfig;
        *((uint32 *)(&CPU5_OMASK0 + (3*i))) = OMASKConfig;
      #else
        #error "Missing OMASK configuration code for the current cpu type !"
      #endif
    }
  }
}



/** Updates the all OTAR registers for all the CPUs without checking the modify bit in the OMD.
This will initialize the emulation target addresses with the target addresses found in the OMD. */
void Update_All_OTAR (VOLATILE_DEF struct OMD_TABLE * OMD_table_Ptr)
{
  int i;
  uint32 RABRConfig, OTARConfig, OMASKConfig, oBaseOffset;


  for ( i=0; i < NUMBER_OMD_MAX_CALIB_HANDLES; i++)
  {
    /** RABR configuration */
    oBaseOffset = OMD_table_Ptr->Target_Address[3*i]; // Physical Address: The address of the overlay memory (RAM) that shall overlay another memory range

    RABRConfig = RABR_Configuration(oBaseOffset);

    #ifdef CPUTYPE_TC33X
      *((uint32 *)(&CPU0_RABR0 + (3*i))) = RABRConfig;
    #elif (defined CPUTYPE_TC36X)
      *((uint32 *)(&CPU0_RABR0 + (3*i))) = RABRConfig;
      *((uint32 *)(&CPU1_RABR0 + (3*i))) = RABRConfig;
    #elif (defined CPUTYPE_TC37X)
      *((uint32 *)(&CPU0_RABR0 + (3*i))) = RABRConfig;  // writes the config to the RABR register corresponding to the handle number
      *((uint32 *)(&CPU1_RABR0 + (3*i))) = RABRConfig;
      *((uint32 *)(&CPU2_RABR0 + (3*i))) = RABRConfig;
    #elif (defined CPUTYPE_TC38X)
      *((uint32 *)(&CPU0_RABR0 + (3*i))) = RABRConfig;  // writes the config to the RABR register corresponding to the handle number
      *((uint32 *)(&CPU1_RABR0 + (3*i))) = RABRConfig;
      *((uint32 *)(&CPU2_RABR0 + (3*i))) = RABRConfig;
      *((uint32 *)(&CPU3_RABR0 + (3*i))) = RABRConfig;
    #elif (defined CPUTYPE_TC39X)
      *((uint32 *)(&CPU0_RABR0 + (3*i))) = RABRConfig;  // writes the config to the RABR register corresponding to the handle number
      *((uint32 *)(&CPU1_RABR0 + (3*i))) = RABRConfig;
      *((uint32 *)(&CPU2_RABR0 + (3*i))) = RABRConfig;
      *((uint32 *)(&CPU3_RABR0 + (3*i))) = RABRConfig;
      *((uint32 *)(&CPU4_RABR0 + (3*i))) = RABRConfig;
      *((uint32 *)(&CPU5_RABR0 + (3*i))) = RABRConfig;
    #else
      #error "Missing RABR configuration code for the current cpu type !"
    #endif


    /** OTAR configuration */
    OTARConfig = ( OMD_table_Ptr->Target_Address[3*i+1] & 0x0fffffe0 ); // Virtual Address: The address of the memory (read-only flash) that will be emulated


    #ifdef CPUTYPE_TC33X
      *((uint32 *)(&CPU0_OTAR0 + (3*i))) = OTARConfig;
    #elif (defined CPUTYPE_TC36X)
      *((uint32 *)(&CPU0_OTAR0 + (3*i))) = OTARConfig;
      *((uint32 *)(&CPU1_OTAR0 + (3*i))) = OTARConfig;
    #elif (defined CPUTYPE_TC37X)
      *((uint32 *)(&CPU0_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
      *((uint32 *)(&CPU1_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
      *((uint32 *)(&CPU2_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
    #elif (defined CPUTYPE_TC38X)
      *((uint32 *)(&CPU0_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
      *((uint32 *)(&CPU1_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
      *((uint32 *)(&CPU2_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
      *((uint32 *)(&CPU3_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
    #elif (defined CPUTYPE_TC39X)
      *((uint32 *)(&CPU0_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
      *((uint32 *)(&CPU1_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
      *((uint32 *)(&CPU2_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
      *((uint32 *)(&CPU3_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
      *((uint32 *)(&CPU4_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
      *((uint32 *)(&CPU5_OTAR0 + (3*i))) = OTARConfig;  // writes the config to the OTAR register corresponding to the handle number
    #else
      #error "Missing OTAR configuration code for the current cpu type !"
    #endif


    /** OMASK configuration */
    OMASKConfig = OMASK_Configuration(OMD_table_Ptr->Target_Address[3*i+2]);

    #ifdef CPUTYPE_TC33X
      *((uint32 *)(&CPU0_OMASK0 + (3*i))) = OMASKConfig;
    #elif (defined CPUTYPE_TC36X)
      *((uint32 *)(&CPU0_OMASK0 + (3*i))) = OMASKConfig;
      *((uint32 *)(&CPU1_OMASK0 + (3*i))) = OMASKConfig;
    #elif (defined CPUTYPE_TC37X)
      *((uint32 *)(&CPU0_OMASK0 + (3*i))) = OMASKConfig;  // writes the config to the OMASK register corresponding to the handle number
      *((uint32 *)(&CPU1_OMASK0 + (3*i))) = OMASKConfig;
      *((uint32 *)(&CPU2_OMASK0 + (3*i))) = OMASKConfig;
    #elif (defined CPUTYPE_TC38X)
      *((uint32 *)(&CPU0_OMASK0 + (3*i))) = OMASKConfig;  // writes the config to the OMASK register corresponding to the handle number
      *((uint32 *)(&CPU1_OMASK0 + (3*i))) = OMASKConfig;
      *((uint32 *)(&CPU2_OMASK0 + (3*i))) = OMASKConfig;
      *((uint32 *)(&CPU3_OMASK0 + (3*i))) = OMASKConfig;
    #elif (defined CPUTYPE_TC39X)
      *((uint32 *)(&CPU0_OMASK0 + (3*i))) = OMASKConfig;  // writes the config to the OMASK register corresponding to the handle number
      *((uint32 *)(&CPU1_OMASK0 + (3*i))) = OMASKConfig;
      *((uint32 *)(&CPU2_OMASK0 + (3*i))) = OMASKConfig;
      *((uint32 *)(&CPU3_OMASK0 + (3*i))) = OMASKConfig;
      *((uint32 *)(&CPU4_OMASK0 + (3*i))) = OMASKConfig;
      *((uint32 *)(&CPU5_OMASK0 + (3*i))) = OMASKConfig;
    #else
      #error "Missing OMASK configuration code for the current cpu type !"
    #endif
  }
}


/** Configures the initial value of the RABR, OTAR and OMASK registers used for the calibration configuration, and sets the OVC_ENABLE value for the required CPU cores */
void Initialize_Calib_Registers(void)
{

  #ifdef CPUTYPE_TC33X
    Init_RABR((uint32 *)&CPU0_RABR0);    // Initializing RABR registers for CPU0

    Init_OTAR((uint32 *)&CPU0_OTAR0);    // Initializing OTAR registers for CPU0

    Init_OMASK((uint32 *)&CPU0_OMASK0);    // Initializing OMASK registers for CPU0
  #elif (defined CPUTYPE_TC36X)
    Init_RABR((uint32 *)&CPU0_RABR0);    // Initializing RABR registers for CPU0
    Init_RABR((uint32 *)&CPU1_RABR0);    // Initializing RABR registers for CPU1

    Init_OTAR((uint32 *)&CPU0_OTAR0);    // Initializing OTAR registers for CPU0
    Init_OTAR((uint32 *)&CPU1_OTAR0);    // Initializing OTAR registers for CPU1

    Init_OMASK((uint32 *)&CPU0_OMASK0);    // Initializing OMASK registers for CPU0
    Init_OMASK((uint32 *)&CPU1_OMASK0);    // Initializing OMASK registers for CPU1
  #elif (defined CPUTYPE_TC37X)
    Init_RABR((uint32 *)&CPU0_RABR0);    // Initializing RABR registers for CPU0
    Init_RABR((uint32 *)&CPU1_RABR0);    // Initializing RABR registers for CPU1
    Init_RABR((uint32 *)&CPU2_RABR0);    // Initializing RABR registers for CPU2

    Init_OTAR((uint32 *)&CPU0_OTAR0);    // Initializing OTAR registers for CPU0
    Init_OTAR((uint32 *)&CPU1_OTAR0);    // Initializing OTAR registers for CPU1
    Init_OTAR((uint32 *)&CPU2_OTAR0);    // Initializing OTAR registers for CPU2

    Init_OMASK((uint32 *)&CPU0_OMASK0);    // Initializing OMASK registers for CPU0
    Init_OMASK((uint32 *)&CPU1_OMASK0);    // Initializing OMASK registers for CPU1
    Init_OMASK((uint32 *)&CPU2_OMASK0);    // Initializing OMASK registers for CPU2
  #elif (defined CPUTYPE_TC38X)
    Init_RABR((uint32 *)&CPU0_RABR0);    // Initializing RABR registers for CPU0
    Init_RABR((uint32 *)&CPU1_RABR0);    // Initializing RABR registers for CPU1
    Init_RABR((uint32 *)&CPU2_RABR0);    // Initializing RABR registers for CPU2
    Init_RABR((uint32 *)&CPU3_RABR0);    // Initializing RABR registers for CPU3

    Init_OTAR((uint32 *)&CPU0_OTAR0);    // Initializing OTAR registers for CPU0
    Init_OTAR((uint32 *)&CPU1_OTAR0);    // Initializing OTAR registers for CPU1
    Init_OTAR((uint32 *)&CPU2_OTAR0);    // Initializing OTAR registers for CPU2
    Init_OTAR((uint32 *)&CPU3_OTAR0);    // Initializing OTAR registers for CPU3

    Init_OMASK((uint32 *)&CPU0_OMASK0);    // Initializing OMASK registers for CPU0
    Init_OMASK((uint32 *)&CPU1_OMASK0);    // Initializing OMASK registers for CPU1
    Init_OMASK((uint32 *)&CPU2_OMASK0);    // Initializing OMASK registers for CPU2
    Init_OMASK((uint32 *)&CPU3_OMASK0);    // Initializing OMASK registers for CPU3
  #elif (defined CPUTYPE_TC39X) 
    Init_RABR((uint32 *)&CPU0_RABR0);    // Initializing RABR registers for CPU0
    Init_RABR((uint32 *)&CPU1_RABR0);    // Initializing RABR registers for CPU1
    Init_RABR((uint32 *)&CPU2_RABR0);    // Initializing RABR registers for CPU2
    Init_RABR((uint32 *)&CPU3_RABR0);    // Initializing RABR registers for CPU3
    Init_RABR((uint32 *)&CPU4_RABR0);    // Initializing RABR registers for CPU4
    Init_RABR((uint32 *)&CPU5_RABR0);    // Initializing RABR registers for CPU5

    Init_OTAR((uint32 *)&CPU0_OTAR0);    // Initializing OTAR registers for CPU0
    Init_OTAR((uint32 *)&CPU1_OTAR0);    // Initializing OTAR registers for CPU1
    Init_OTAR((uint32 *)&CPU2_OTAR0);    // Initializing OTAR registers for CPU2
    Init_OTAR((uint32 *)&CPU3_OTAR0);    // Initializing OTAR registers for CPU3
    Init_OTAR((uint32 *)&CPU4_OTAR0);    // Initializing OTAR registers for CPU4
    Init_OTAR((uint32 *)&CPU5_OTAR0);    // Initializing OTAR registers for CPU5

    Init_OMASK((uint32 *)&CPU0_OMASK0);    // Initializing OMASK registers for CPU0
    Init_OMASK((uint32 *)&CPU1_OMASK0);    // Initializing OMASK registers for CPU1
    Init_OMASK((uint32 *)&CPU2_OMASK0);    // Initializing OMASK registers for CPU2
    Init_OMASK((uint32 *)&CPU3_OMASK0);    // Initializing OMASK registers for CPU3
    Init_OMASK((uint32 *)&CPU4_OMASK0);    // Initializing OMASK registers for CPU4
    Init_OMASK((uint32 *)&CPU5_OMASK0);    // Initializing OMASK registers for CPU5
  #else
    #error "Missing OMASK/OTAR/RABR initial configuration code for the current cpu type !"
  #endif



  #ifdef CPUTYPE_TC33X
    // enables OVC on the CPU 0 in the SCU_OVCENABLE register
    #ifdef ENDINIT_PROTECTION_DISABLED
      SCU_OVCENABLE.U = 0x01;
    #else
      unlock_wdtcon('s');
      SCU_OVCENABLE.U = 0x01;  // Access only with supervisor mode, 32 bit access and safety endinit disabled
      lock_wdtcon('s');
    #endif
  #elif (defined CPUTYPE_TC36X)
    // enables OVC on the CPU 0-1 in the SCU_OVCENABLE register
    #ifdef ENDINIT_PROTECTION_DISABLED
      SCU_OVCENABLE.U = 0x03;
    #else
      unlock_wdtcon('s');
      SCU_OVCENABLE.U = 0x03;  // Access only with supervisor mode, 32 bit access and safety endinit disabled
      lock_wdtcon('s');
    #endif
  #elif (defined CPUTYPE_TC37X)
    // enables OVC on the CPU 0-2 in the SCU_OVCENABLE register
    #ifdef ENDINIT_PROTECTION_DISABLED
      SCU_OVCENABLE.U = 0x07;
    #else
      unlock_wdtcon('s');
      SCU_OVCENABLE.U = 0x07;  // Access only with supervisor mode, 32 bit access and safety endinit disabled
      lock_wdtcon('s');
    #endif
  #elif (defined CPUTYPE_TC38X)
    // enables OVC on the CPU 0-3 in the SCU_OVCENABLE register
    #ifdef ENDINIT_PROTECTION_DISABLED
      SCU_OVCENABLE.U = 0x0F;
    #else
      unlock_wdtcon('s');
      SCU_OVCENABLE.U = 0x0F;
      lock_wdtcon('s');
    #endif
  #elif (defined CPUTYPE_TC39X)
    // enables OVC on the CPU 0-5 in the SCU_OVCENABLE register
    #ifdef ENDINIT_PROTECTION_DISABLED
      SCU_OVCENABLE.U = 0x3F;
    #else
      unlock_wdtcon('s');
      SCU_OVCENABLE.U = 0x3F;  // Access only with supervisor mode, 32 bit access and safety endinit disabled
      lock_wdtcon('s');
    #endif
  #else
    #error "Missing configuration code for SCU_OVCENABLE for the current cpu type !"
  #endif

}

#ifdef PD_DEVICE

void Configure_EMEM_Tiles_For_ECU_RAM(void)
{ }


#else // ED device

void Configure_EMEM_Tiles_For_ECU_RAM(void)
{
  EMEM_TILECONFIG.U = CONFIG_ALL_TILES_TO_CALIB_MODE;

  #ifdef CPUTYPE_TC37X
    EMEM_TILECC.U = 0x00000FFF; // Tiles in Tool mode to allow ETK/debugger access through BBB
    EMEM_TILECT.U = 0x00000000; // No tile is used for TRACE
  #elif (defined CPUTYPE_TC39X)
    EMEM_TILECC.U = 0x0000FFFF; // Tiles in Tool mode to allow ETK/debugger access through BBB
    EMEM_TILECT.U = 0x00000000; // No tile is used for TRACE
  #endif
}

#endif //#ifdef PD_DEVICE



/** Disables the calibration handles (same as switching to the reference page)
This function will clear the overlay bits in the RABRx registers. */
void Disable_Calibration_Handles (void)
{

    #if (defined CPUTYPE_TC33X)
      CPU0_OSEL.U = 0x0; // Disable all the overlays in the CPU0 when OVC_CON.OVSTRT is set to 1
      // Disable OVC for the CPU 0 in the SCU_OVCCON register -> OVCx_RABRy.OVEN cleared for all the CPUs
      // The OVCCON register is not protected by the safety endinit module starting from the TC3xx family.
      SCU_OVCCON.U = 0x00060001; // OVC_OVSTP, DCINVAL and OVC_CSEL0

    #elif (defined CPUTYPE_TC36X)
      CPU0_OSEL.U = 0x0; // Disable all the overlays in the CPU0 when OVC_CON.OVSTRT is set to 1
      CPU1_OSEL.U = 0x0; // Disable all the overlays in the CPU1 when OVC_CON.OVSTRT is set to 1

      // Disable OVC for the CPU 0-1 in the SCU_OVCCON register -> OVCx_RABRy.OVEN cleared for all the CPUs
      // The OVCCON register is not protected by the safety endinit module starting from the TC3xx family.
      SCU_OVCCON.U = 0x00060003; // OVC_OVSTP, DCINVAL and OVC_CSEL0 to OVC_CSEL1

    #elif (defined CPUTYPE_TC37X)
      CPU0_OSEL.U = 0x0; // Disable all the overlays in the CPU0 when OVC_CON.OVSTRT is set to 1
      CPU1_OSEL.U = 0x0; // Disable all the overlays in the CPU1 when OVC_CON.OVSTRT is set to 1
      CPU2_OSEL.U = 0x0; // Disable all the overlays in the CPU2 when OVC_CON.OVSTRT is set to 1

      // Disable OVC for the CPU 0-2 in the SCU_OVCCON register -> OVCx_RABRy.OVEN cleared for all the CPUs
      // The OVCCON register is not protected by the safety endinit module starting from the TC3xx family.
      SCU_OVCCON.U = 0x00060007; // OVC_OVSTP, DCINVAL and OVC_CSEL0 to OVC_CSEL2

    #elif (defined CPUTYPE_TC38X)
      CPU0_OSEL.U = 0x0; // Disable all the overlays in the CPU0 when OVC_CON.OVSTRT is set to 1
      CPU1_OSEL.U = 0x0; // Disable all the overlays in the CPU1 when OVC_CON.OVSTRT is set to 1
      CPU2_OSEL.U = 0x0; // Disable all the overlays in the CPU2 when OVC_CON.OVSTRT is set to 1
      CPU3_OSEL.U = 0x0; // Disable all the overlays in the CPU3 when OVC_CON.OVSTRT is set to 1

      // Disable OVC for the CPU 0-3 in the SCU_OVCCON register -> OVCx_RABRy.OVEN cleared for all the CPUs
      // The OVCCON register is not protected by the safety endinit module starting from the TC3xx family.
      SCU_OVCCON.U = 0x0006000F; // OVC_OVSTP, DCINVAL and OVC_CSEL0 to OVC_CSEL3


    #elif (defined CPUTYPE_TC39X)
      CPU0_OSEL.U = 0x0; // Disable all the overlays in the CPU0 when OVC_CON.OVSTRT is set to 1
      CPU1_OSEL.U = 0x0; // Disable all the overlays in the CPU1 when OVC_CON.OVSTRT is set to 1
      CPU2_OSEL.U = 0x0; // Disable all the overlays in the CPU2 when OVC_CON.OVSTRT is set to 1
      CPU3_OSEL.U = 0x0; // Disable all the overlays in the CPU3 when OVC_CON.OVSTRT is set to 1
      CPU4_OSEL.U = 0x0; // Disable all the overlays in the CPU4 when OVC_CON.OVSTRT is set to 1
      CPU5_OSEL.U = 0x0; // Disable all the overlays in the CPU5 when OVC_CON.OVSTRT is set to 1

      // Disable OVC for the CPU 0-5 in the SCU_OVCCON register -> OVCx_RABRy.OVEN cleared for all the CPUs
      // The OVCCON register is not protected by the safety endinit module starting from the TC3xx family.
      SCU_OVCCON.U = 0x0006003F; // OVC_OVSTP, DCINVAL and OVC_CSEL0 to OVC_CSEL5

    #else
        #error "Undefined Disable_Calibration_Handles function for the current µC !"
    #endif


}


/** Sets all the overlays in the CPUs to the defined value in the OMD after OVC_CON.OVSTRT is set to 1. */
void Set_Calibration_Handles (VOLATILE_DEF struct OMD_TABLE * OMD_table_Ptr)
{

    #if (defined CPUTYPE_TC33X)
      CPU0_OSEL.U = OMD_table_Ptr->Activate_EMU_Handles[0]; // Sets up the overlays in the CPU0 OSEL. Initialized when OVC_CON.OVSTRT is set to 1.
    #elif (defined CPUTYPE_TC36X)
      CPU0_OSEL.U = OMD_table_Ptr->Activate_EMU_Handles[0]; // Sets up the overlays in the CPU0 OSEL. Initialized when OVC_CON.OVSTRT is set to 1.
      CPU1_OSEL.U = OMD_table_Ptr->Activate_EMU_Handles[0]; // Sets up the overlays in the CPU1 OSEL. Initialized when OVC_CON.OVSTRT is set to 1.

    #elif (defined CPUTYPE_TC37X)
      CPU0_OSEL.U = OMD_table_Ptr->Activate_EMU_Handles[0]; // Sets up the overlays in the CPU0 OSEL. Initialized when OVC_CON.OVSTRT is set to 1.
      CPU1_OSEL.U = OMD_table_Ptr->Activate_EMU_Handles[0]; // Sets up the overlays in the CPU1 OSEL. Initialized when OVC_CON.OVSTRT is set to 1.
      CPU2_OSEL.U = OMD_table_Ptr->Activate_EMU_Handles[0]; // Sets up the overlays in the CPU2 OSEL. Initialized when OVC_CON.OVSTRT is set to 1.

    #elif (defined CPUTYPE_TC38X)
      CPU0_OSEL.U = OMD_table_Ptr->Activate_EMU_Handles[0]; // Sets up the overlays in the CPU0 OSEL. Initialized when OVC_CON.OVSTRT is set to 1.
      CPU1_OSEL.U = OMD_table_Ptr->Activate_EMU_Handles[0]; // Sets up the overlays in the CPU1 OSEL. Initialized when OVC_CON.OVSTRT is set to 1.
      CPU2_OSEL.U = OMD_table_Ptr->Activate_EMU_Handles[0]; // Sets up the overlays in the CPU2 OSEL. Initialized when OVC_CON.OVSTRT is set to 1.
      CPU3_OSEL.U = OMD_table_Ptr->Activate_EMU_Handles[0]; // Sets up the overlays in the CPU3 OSEL. Initialized when OVC_CON.OVSTRT is set to 1.

    #elif (defined CPUTYPE_TC39X)
      CPU0_OSEL.U = OMD_table_Ptr->Activate_EMU_Handles[0]; // Sets up the overlays in the CPU0 OSEL. Initialized when OVC_CON.OVSTRT is set to 1.
      CPU1_OSEL.U = OMD_table_Ptr->Activate_EMU_Handles[0]; // Sets up the overlays in the CPU1 OSEL. Initialized when OVC_CON.OVSTRT is set to 1.
      CPU2_OSEL.U = OMD_table_Ptr->Activate_EMU_Handles[0]; // Sets up the overlays in the CPU2 OSEL. Initialized when OVC_CON.OVSTRT is set to 1.
      CPU3_OSEL.U = OMD_table_Ptr->Activate_EMU_Handles[0]; // Sets up the overlays in the CPU3 OSEL. Initialized when OVC_CON.OVSTRT is set to 1.
      CPU4_OSEL.U = OMD_table_Ptr->Activate_EMU_Handles[0]; // Sets up the overlays in the CPU4 OSEL. Initialized when OVC_CON.OVSTRT is set to 1.
      CPU5_OSEL.U = OMD_table_Ptr->Activate_EMU_Handles[0]; // Sets up the overlays in the CPU5 OSEL. Initialized when OVC_CON.OVSTRT is set to 1.
    #else
      #error "Undefined cpu type in the function Set_Calibration_Handles() !"
    #endif

    /* Start the OVC for the CPU 0-5 in the SCU_OVCCON register and invalidate the DMI cache.
    -> OVCx_RABRy.OVEN set for the CPUs defined in the OVCx_OSEL registers */
    // The OVCCON register is not protected by the safety endinit module starting from the TC3xx family.
    SCU_OVCCON.U = 0x0005003F; // OVC_OVSTRT, DCINVAL and OVC_CSEL0 to OVC_CSEL5


}

/** Returns 1 if no calibration handles is active. This means the ECU is in RP mode.*/
uint8 Check_Calibration_Handles_in_RP_mode(void)
{

  #if (defined CPUTYPE_TC33X)
    if (CPU0_OSEL.U == 0)                     // All handles are in RP mode (not activated)
  #elif (defined CPUTYPE_TC36X)
    if (CPU0_OSEL.U == 0 && CPU1_OSEL.U == 0) // All handles are in RP mode (not activated)
  #elif defined CPUTYPE_TC37X
    if (CPU0_OSEL.U == 0 && CPU1_OSEL.U == 0 && CPU2_OSEL.U == 0)
  #elif defined CPUTYPE_TC38X
    if (CPU0_OSEL.U == 0 && CPU1_OSEL.U == 0 && CPU2_OSEL.U == 0 && CPU3_OSEL.U == 0)
  #elif defined CPUTYPE_TC39X
    if (CPU0_OSEL.U == 0 && CPU1_OSEL.U == 0 && CPU2_OSEL.U == 0 && CPU3_OSEL.U == 0 && CPU4_OSEL.U == 0 && CPU5_OSEL.U == 0)
  #else
    #error "Undefined CPUx_OSEL test in Check_Calibration_Handles_in_RP_mode() !"
  #endif
    {
      return 1;
    }
    else
    {
      return 0;
    }
}

/** This function should invalidate the data cache (DMI) for all the cores.
If this function is called periodically it should ensure that the calibrated parameters value will get updated inside the cpu cache.
Note: If data cache contains data modified by the cpu, it has to be written back and invalidated by the user ! */
void dataCacheInvalidate(void)
{
  #ifdef CPUCLASS_TC2D5
    #ifdef ENDINIT_PROTECTION_DISABLED
      SCU_OVCCON.U = 0x00040007; // DCINVAL and OVC_CSEL0 to OVC_CSEL2
    #else
      // data cache invalidate
      unlock_wdtcon('s');
      // Invalidate the data cache (DMI) for the CPU 0, 1 and 2 in the SCU_OVCCON register
      SCU_OVCCON.U = 0x00040007; // DCINVAL and OVC_CSEL0 to OVC_CSEL2
      lock_wdtcon('s');
    #endif
  #elif (defined CPUCLASS_TC3XX)
      /* Invalidate the data cache (DMI) for the CPU 0-5 in the SCU_OVCCON register. */
      // The OVCCON register is not protected by the safety endinit module starting from the TC3xx family.
      SCU_OVCCON.U = 0x0004003F; // DCINVAL and OVC_CSEL0 to OVC_CSEL5

  #else
    #error "Undefined cpu type in the function dataCacheInvalidate() !"
  #endif
}
/*
################################################################################
#                                                                              #
#    ETK driver example                          |   ETAS GmbH                 #
#    Distab17 data type definition               |   Stuttgart Feuerbach       #
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
* \file  Distab17_Def.h
* \brief file containing the type definitions for distab data types
*
* This file contains the base type definitions for all distab based ECU routines
* supporting measurement and bypass solutions supported by ETAS tools
*******************************************************************************/

/* TODO: All that is here is effectively 1-byte aligned, or is it?
         -> if not, we'd have to introduce special structures (this time really
            one byte aligned) that contain the individual configuration entries
            packed into a 32-bit field of the containing struct.
*/

#ifndef DISTAB17_DEF_H
#define DISTAB17_DEF_H

//#include "a_std_type.h"
// #include "../../Base.h"         /* for the register addresses --> move to a  */
#include "../ETK_Integration_cfg.h"     /* for the configuration */
#include "Distab17_Cfg.h"       /* for the configuration */

/* ---------------------------------------------------------------------------*/
/* COMMON DEFINES AND SIMPLE DATATYPES                                        */
/* ---------------------------------------------------------------------------*/
typedef     struct ulonglong
                   {
                    uint32      hWrd;
                    uint32      lWrd;
                   } ustr64;


/***************************************************************************//**
* \typedef tD17EventOutputHeader
* \brief This datatype is the header for a particular event's output area
*
* The Distab17 event output area contains the ECU status and the trigger flag,
* followed by the measurement data.
*
* This entry is written by the ECU and read by the ETK system.
*
* If D17_ETK_TRIGGER_FLAG_CLEAR is set, the ETK has to clear the trigger
* flag before the ECU issues another trigger -- this requires ECU read and
* ETK write access.
*/
/** \struct D17EventOutputHeader
* \brief This structure is the header for a particular event's output area
*
* The Distab17 event output area contains the ECU status and the trigger flag,
* followed by the measurement data.
*
* This entry is written by the ECU and read by the ETK system.
*
* If D17_ETK_TRIGGER_FLAG_CLEAR is set, the ETK has to clear the trigger
* flag before the ECU issues another trigger -- this requires ECU read and
* ETK write access.
*******************************************************************************/
typedef struct D17EventOutputHeader
{
    uint32 EcuStatus; /**< ECU status, set to (! 0) by the ECU when processing this event */
    uint32 TriggerFlag; /**< Trigger Flag, set to (! 0) by the ECU to indicate new data for this event */

    void *FirstEntry; /**< Start of the measurement data */
} tD17EventOutputHeader;

/***************************************************************************//**
* \typedef tD17EventConfigHeader
* \brief This datatype describes the configuration for a particular event
*
* A Distab17 event configuration contains the version number, a change indicator,
* and the (possibly virtual) trigger to be used (ref. "ECU_ID" AML entry).
*
* This entry is written by the ETK system and read by the ECU.
*/
/** \struct D17EventConfigHeader
* \brief This structure describes the generic configuration for a particular event
*
* A Distab17 event configuration contains the version number, a change indicator,
* and the (possibly virtual) trigger to be used (ref. "ECU_ID" AML entry).
*
* This entry is written by the ETK system and read by the ECU.
*******************************************************************************/
typedef struct D17EventConfigHeader
{
    uint8 Change; /**< Incremented whenever this configuration has been changed */
    uint8 Config; /**< Defines some properties of the event. Bit 0 defines if TDM is used or not. */
    uint16 Trigger; /**< The (possibly virtual) trigger to be set after the ECU has provided the data */
    tD17EventOutputHeader *Output; /**< The start of the output area for this event */
} tD17EventConfigHeader;

/***************************************************************************//**
* \typedef tD17EventConfig
* \brief This datatype describes an event in case TDM is not used
*
* In addition to the generated event configuration header, this datatype also
* contains the number of measurement variables of 8-, 4-, 2-, and 1-byte,
* followed by the respective source addresses.
*
* This entry is written by the ETK system and read by the ECU.
*/
/** \struct D17EventConfig
* \brief This datatype describes the configuration of an event in case TDM is not used
*
* In addition to the generated event configuration header, this datatype also
* contains the number of measurement variables of 8-, 4-, 2-, and 1-byte,
* followed by the respective source addresses.
*
* This entry is written by the ETK system and read by the ECU.
*******************************************************************************/
typedef struct D17EventConfig
{
    tD17EventConfigHeader Header; /**< The generic event config header */

    uint16 NoOfVal_8; /**< number of 8 byte (64 bit) values in the Distab */
    uint16 NoOfVal_4; /**< number of 4 byte (32 bit) values in the Distab */
    uint16 NoOfVal_2; /**< number of 2 byte (16 bit) values in the Distab */
    uint16 NoOfVal_1; /**< number of 1 byte (8 bit) values in the Distab */

    void *FirstAddress; /**< address of the first address to be measured */
} tD17EventConfig;

/***************************************************************************//**
* \typedef tD17EventList
* \brief This datatype describes all available events
*
* While the header contains currently applicable configuration items, each
* member of the event configuration pointer array describes an event in detail.
*
* This entry is written by the ETK system and read by the ECU.
*/
/** \struct D17EventList
* \brief This structure describes all available events
*
* While the header contains currently applicable configuration items, each
* member of the event configuration pointer array describes an event in detail.
*
* This entry is written by the ETK system and read by the ECU.
*******************************************************************************/
typedef struct D17EventList
{
    uint8 Version; /**< The Distab version number, has to be 17 */
    uint8 Change; /**< Incremented modulo 0xFF whenever the event configuration changes */
    uint8 First; /**< Set to the index of the first active event config pointer */
    uint8 Number; /**< The number of currently active events */

    uint32 Config[D17_MAX_EVENT_NO]; /**< One entry for each event */
} tD17EventList;

#ifdef D17_USE_VIRTUAL_TRIGGER
/***************************************************************************//**
* \typedef tETK_Trigger_Map
* \brief This datatype describes a mapping from virtual to actual triggers
*
* This entry is constant.
*/
/** \struct ETKTriggerMap
* \brief This structure describes a mapping from virtual to actual triggers
*
* This entry is constant.
*******************************************************************************/
typedef struct ECUTriggerMap
{
    uint16 ECU_ID; /**< The ID specified in the A2L file */
    uint8 TriggerNumber; /**< The actual trigger issued by the ECU, also specified in the A2L file */
} tECUTriggerMap;

/***************************************************************************//**
* \typedef tETK_Trigger_Table
* \brief This datatype describes the number of mappings from virtual to actual triggers
*
* This entry is constant.
*/
/** \struct ETK_Trigger_Table
* \brief This structure describes the number of mappings from virtual to actual triggers
*
* This entry is constant.
*******************************************************************************/
typedef struct ECUTriggerTable
{
    uint8 Entries; /**< The number of entries in the table */
    tECUTriggerMap *Table; /**< Pointer to a table containing the mapping */
} tECUTriggerTable;
#endif

/* possible return values for distab17Process() */
#define D17_EVENT_NOT_ACTIVE                  0x00 /**< possible return values for distab17Process(): event not active */
#define D17_EVENT_UNSUPPORTED_VERSION         0x01 /**< possible return values for distab17Process(): version field contains unexpected and unsupported value */
#define D17_EVENT_INVALID_OUTPUT_TABLE        0x02 /**< possible return values for distab17Process(): the pointer to the output table is required, but not set */
#define D17_DATA_ACQUISITION_SUCCESS          0xFF /**< possible return values for distab17Process(): Distab was successfully processed */

#endif /* DISTAB17_DEF_H */

/*
################################################################################
#                                                                              #
#    ETK driver example                          |   ETAS GmbH                 #
#    Distab17 configuration                      |   Stuttgart Feuerbach       #
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
* \file  Distab17_Cfg.h
* \brief file containing global constants required for Distab17 usage
*
* This file is used to define the necessary information to configure
* distab based measurement
*******************************************************************************/

#ifndef DISTAB17_CFG_H
#define DISTAB17_CFG_H

#define D17_MAX_EVENT_NO                (0xFF) /**< The maximum number of events supported */
#define D17_VERSION_NUMBER              (17) /**< The version number to check for */
#define D17_ADDRESS_ALIGNMENT           (32) // TODO: Do we need this here?!
//#define D17_USE_VIRTUAL_TRIGGER /**< Whether this instance requires a map from virtual to real triggers */
//#define D17_ETK_TRIGGER_FLAG_CLEAR /**< Whether the ETK system does clear the trigger flag after processing a trigger */

#ifdef TRACE_SUPPORT
  /* Ads support for Trace with supplementary Distab. 
  The Distab function will detect the TRACE specific ECU ID triggers and use a TRACE trigger instead of the register trigger. */
  #define D17_TRACE_SUPPL_DISTAB
  #define D17_TRACE_ID_OFFSET 1024 // Defines a 1024 offset for the virtual ID
  #define D17_TRACE_MAX_TRIGGER 255 // Defines the maximal value of the Trace triggers
#endif

#define D17_EVENT_CONFIG_AREA_SIZE      (0x2000) /**<  The size in bytes of the Event Config Area. Contains the address table. */
#define D17_EVENT_OUTPUT_AREA_SIZE      (0x2000) /**<  The size in bytes of the Event Output Area. */

#endif /*DISTAB17_CFG_H */

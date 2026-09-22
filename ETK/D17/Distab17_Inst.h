/*
################################################################################
#                                                                              #
#    ETK driver example                          |   ETAS GmbH                 #
#    Distab17 instatiation header                |   Stuttgart Feuerbach       #
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
* \file  Distab17_Inst.h
* \brief file containing the declaration of the Distab17 data structs
*******************************************************************************/

#ifndef DISTAB17_INST_H
#define DISTAB17_INST_H

#include "../ETK_Integration_Cfg.h"
#include "Distab17_Def.h"

extern
#ifdef USE_DISTAB_CONST_DEF
DISTAB_CONST_DEF
#endif /* #ifdef USE_DISTAB_CONST_DEF */
#ifdef USE_VOLATILE_DEF
VOLATILE_DEF
#endif /* #ifdef USE_VOLATILE_DEF */
tD17EventList Distab17EventList;


#ifdef D17_USE_VIRTUAL_TRIGGER
extern
#ifdef USE_DISTAB_CONST_DEF
DISTAB_CONST_DEF
#endif /* #ifdef USE_DISTAB_CONST_DEF */
#ifdef USE_VOLATILE_DEF
VOLATILE_DEF
#endif /* #ifdef USE_VOLATILE_DEF */
tECUTriggerTable ECUTriggerTable;
#endif

#endif /* DISTAB17_INST_H */

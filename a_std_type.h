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


#ifndef __A_STD_TYPE_H
#define __A_STD_TYPE_H

/******************************************************************************
**  FILE:           A_STD_TYPE.H
**
**  DESCRIPTION:    central header file to be included from all ASCET
**                  projects files.
**
**  VISIBILITY:     internal
**
**  COPYRIGHT:      ETAS GmbH
**                  Stuttgart / Germany
**                  All rights reserved
**
*******************************************************************************/

/******************************************************************************
**    MACROS AND INLINE FUNCTIONS
*******************************************************************************/
#include <string.h>
#include <stdbool.h>

/* define C boolean if not yet happens */
#ifndef false
    #define false (0)
#endif
#ifndef true
    #define true  (!false)
#endif
#ifndef FALSE
	#define FALSE   (0)
#endif
#ifndef TRUE
	#define TRUE    (!FALSE)
#endif

/* macros for optimized hi part access - toBeDesigned */
#define getHighPart_32(x) ((x)>>16)
#define getHighPart_16(x) ((x)>>8)

/* min, max macros */
#ifndef min
	#define min(a, b)        (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
	#define max(a, b)        (((a) > (b)) ? (a) : (b))
#endif

/******************************************************************************
**    TYPE DEFINITIONS
*******************************************************************************/
#ifndef _STD_TYPE_H
	#define _STD_TYPE_H

	// typedef unsigned char  bool;

	typedef unsigned char  bit8;
	typedef unsigned short bit16;
	typedef unsigned long  bit32;

	typedef unsigned int   uint;
	typedef signed int     sint;
	typedef double         real;


#ifdef __ASD_REQUIRES_RTE_INFACE
	#include "Platform_Types.h"
#else
	typedef signed char         sint8;          /*        -128 .. +127            */
	typedef unsigned char       uint8;          /*           0 .. 255             */
	typedef signed short        sint16;         /*      -32768 .. +32767          */
	typedef unsigned short      uint16;         /*           0 .. 65535           */
	typedef signed long         sint32;         /* -2147483648 .. +2147483647     */
	typedef unsigned long       uint32;         /*           0 .. 4294967295      */
#endif

	typedef float          real32;
	typedef double         real64;

 	#if !defined(_C166)
		/* tasking compiler uses bit type */
		typedef unsigned char  bit;
	#endif

	#if defined(__DCC__)  || defined(__use64integers__) && !defined(__QA_C_CHECKER)
		/* diab compiler supports long long type. */
		/* explicitly define  __use64integers__ for other compilers supporting this type. */
		typedef unsigned long long uint64;
		typedef signed long long   sint64;
    #else
		typedef unsigned long uint64;
		typedef signed long   sint64;
    #endif
#endif /* _STD_TYPE_H */
#endif /* __A_STD_TYPE_H */

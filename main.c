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


#include "ETK/ETK_Integration_Cfg.h"            // for ETK configuration
#include "Base.h"
#include "main.h"
#include "ISR.h"
#include "ETK/ETK_SER_Handshake.h"              // for ETK_COMDATA_HANDSHAKE
#ifdef CALIBRATION_SUPPORTED
#include "ETK/CALIB/ETK_SER_Page_Ctrl_Fct.h"    // for CALIBRATION_SUPPORTED
#endif
#include "Init.h"
#include "GPIO.h"
#ifdef ACC_SUPPORTED
#include "ETK/ACC/ETK_CodeCheck.h"            // for ACC_SUPPORTED
#endif
#ifdef DISTAB_SUPPORTED
#include "ETK/D17/Distab17_Processes.h"       // for DISTAB_SUPPORTED
#endif
#ifdef TRACE_SUPPORT   // for TRACE_SUPPORT - works only when DISTAB_SUPPORTED is not defined
    #include "ETK/TRACE/Trace_Fct.h" 
#endif
#include "countersPerf_CPU0.h"
#include "calibrationParam.h"
#include "version_defs.h"

#ifdef DECLAREVARTYPE
	typedef signed char         sint8;          /*        -128 .. +127            */
	typedef unsigned char       uint8;          /*           0 .. 255             */
	typedef signed short        sint16;         /*      -32768 .. +32767          */
	typedef unsigned short      uint16;         /*           0 .. 65535           */
	typedef signed long         sint32;         /* -2147483648 .. +2147483647     */
	typedef unsigned long       uint32;         /*           0 .. 4294967295      */
  typedef unsigned long long  uint64;
#endif



//flash key pattern
//used to avoid inconsistancy between ECU projects and prof flow
//startup value is 0xCCCC 0001
//the 16 most significant bits are used for identification
//after every memory layout change the value has to be increased from e.g. 0xCCCC 0001 -> 0xCCCC 0002
#ifndef HIGHTEC_COMPILER
  #pragma section all "FLASH_KEY"
#else
  #pragma section ".FLASH_KEY" aw 4
#endif
  const uint32 FlashKey = FLASHKEY_PATTERN;
#ifndef HIGHTEC_COMPILER
  #pragma section all
#else
  #pragma section
#endif


#ifndef HIGHTEC_COMPILER
  #pragma section all "VERSION_XYZ"
#else
  #pragma section ".VERSION_XYZ" aw 4
#endif
volatile uint32 version_X, version_Y, version_Z;
#ifndef HIGHTEC_COMPILER
  #pragma clear
  #pragma section all
#else
  #pragma section
#endif


//variables for section CAL_PARAM
#ifndef HIGHTEC_COMPILER
  #pragma section all "measure_parameters"
#else
  #pragma section ".measure_parameters" aw 4
#endif

volatile const struct MEASURE_PARAMETERS measure_parameters = {
  //task 1
  255,
  0, // dummy
  65535,
  4294967295,
  100,             //default value of alarm_task, 10�s tick * 100 is 1ms alarm task for example shown in INCA EE
  // task 2
  255,
  0, // dummy
  65535,
  4294967295,
  500,             //default value of alarm_task, 10�s tick * 500 is 5ms alarm task for example shown in INCA EE
  //task 3
  255,
  0, // dummy
  65535,
  4294967295,
  1000             //default value of alarm_task, 10�s tick * 1000 is 10ms alarm task for example shown in INCA EE
};

#ifndef HIGHTEC_COMPILER
  #pragma section all
#else
  #pragma section
#endif

#ifndef HIGHTEC_COMPILER
  #pragma section all "measure_variables"
  #pragma noclear
#else
  #pragma section ".measure_variables" aw 4
#endif

  volatile struct MEASURE_VALUES measure_values = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
  };

#ifndef HIGHTEC_COMPILER
  #pragma clear
  #pragma section all
#else
  #pragma section
#endif

// dummy variables for section IRAM
#ifndef HIGHTEC_COMPILER
  #pragma section all "measure_dummies"
  #pragma noclear
#else
  #pragma section ".measure_dummies" aw 4
#endif

  /* 256 x 8bit variable  */
  volatile uint8 Dummy_Matrix_1Byte[256] = {
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x08,0x08
  };

  /* 256 x 16bit variable  */
  volatile uint16 Dummy_Matrix_2Byte[256] = {
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,0x1616,
  0x1616,0x1616,0x1616,0x1616,0x1616,0x1616
  };

  /* 255 x 32bit variable  */
  volatile uint32 Dummy_Matrix_4Byte[255] = {
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,0x32323232,
  0x32323232,0x32323232,0x32323232,0x32323232,0x32323232
  };

  /* 255 x 64bit double floating variable  */
  volatile real64 Dummy_Matrix_8Byte[255] = {
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,
  64.0,64.0,64.0,64.0,64.0
  };

#ifndef HIGHTEC_COMPILER
  #pragma clear
  #pragma section all
#else
  #pragma section
#endif

// dummy variables for section IRAM
#ifndef HIGHTEC_COMPILER
  #pragma section all "dT_measurement"
  #pragma noclear
#else
  #pragma section ".dT_measurement" aw 4
#endif

   volatile struct DT_MEASUREMENT dT_measurement = { 0, 0, 0, 0, 0, 0
   };

#ifndef HIGHTEC_COMPILER
  #pragma clear
  #pragma section all
#else
  #pragma section
#endif



#ifndef HIGHTEC_COMPILER
  #pragma section all "IRAM"
  #pragma noclear
#else
  #pragma section ".IRAM" aw 4
#endif

//local variables
volatile uint32 getRasterTimeStart, getDistabTimeStart, getTimeStop, loopCounter;
uint32 Led_Counter_01;


#ifndef HIGHTEC_COMPILER
  #pragma clear
  #pragma section all
#else
  #pragma section
#endif



//main function
int main(void)
{
  #ifndef HIGHTEC_COMPILER
  /** We use a jump absolute command here to force the ECU to change its Program Counter to the address of the cached flash section if this was compiled as cached code in the linker file. */
    __asm("ja\t_absoluteJump");
    __asm("_absoluteJump:");
  #endif

  // Enable instruction cache only for cached code sections (0x8xxxx xxxx)
  if (BASE_AccessDetection() == 0)
  {
    //enable instruction cache
    BASE_Enable_InstructionCache();
  }

  Init_STM_CMP0_Timer_And_Interrupt();  //setup system tick timer and interrupt, start STM0 clock
  Init_GPIO();                 //initialize ports

  #ifdef ACC_SUPPORTED
  initRAMCheckPattern();
  #endif

  SER_Initial_Handshake_Execute();

  // If the initial handshake was not executed successfully or ran into a timeout
  if (ECU_ETK_Status.Protocol_Success == 0 || ECU_ETK_Status.Handshake_End_State != 0)
  {
    /** Resets the LEDs 4-7 in the LED off state (= high output). 
    Changes the first 4 LED display state for the TC3xx Triboard.
    This is currently used to signalize a missing ETK handshake.*/
    GPIO_LedBlinkingOFF();
  }

  


  version_X = VERSION_X;
  version_Y = VERSION_Y;
  version_Z = VERSION_Z;


    while (1)
    {
      if (measure_parameters.task1_alarm_task !=0 && ISR_counter_task1 >= measure_parameters.task1_alarm_task)          //periodic raster time = ISR_counter * eMIOS channel 0 time base
      {
        //reset ISR_counter_task
        ISR_counter_task1 = 0;

        GPIO_ToggleLED(1);

        //*** task 1 ***
        getRasterTimeStart = BASE_GetSystemTime();        //task 1 time get start time for INCA measurement

        // 8 bit counters
        measure_values.task1_counter_uint8_1 = Counter_8bit (measure_values.task1_counter_uint8_1, measure_parameters.task1_counter_uint8_max);
        measure_values.task1_counter_uint8_2 = Counter_8bit (measure_values.task1_counter_uint8_2, measure_parameters.task1_counter_uint8_max);
        measure_values.task1_counter_uint8_3 = Counter_8bit (measure_values.task1_counter_uint8_3, measure_parameters.task1_counter_uint8_max);


        // 16 bit counters
        measure_values.task1_counter_uint16_1 = Counter_16bit (measure_values.task1_counter_uint16_1, measure_parameters.task1_counter_uint16_max);
        measure_values.task1_counter_uint16_2 = Counter_16bit (measure_values.task1_counter_uint16_2, measure_parameters.task1_counter_uint16_max);
        measure_values.task1_counter_uint16_3 = Counter_16bit (measure_values.task1_counter_uint16_3, measure_parameters.task1_counter_uint16_max);

        //32 bit counters
        measure_values.task1_counter_uint32_1 = Counter_32bit (measure_values.task1_counter_uint32_1, measure_parameters.task1_counter_uint32_max);
        measure_values.task1_counter_uint32_2 = Counter_32bit (measure_values.task1_counter_uint32_2, measure_parameters.task1_counter_uint32_max);
        measure_values.task1_counter_uint32_3 = Counter_32bit (measure_values.task1_counter_uint32_3, measure_parameters.task1_counter_uint32_max);

        cpu0CounterTask(0); // writes 8/16/32/64 bit counters in DSPR0, DSPR1 and LMU RAM - virtual task 0 input

        getDistabTimeStart = BASE_GetSystemTime();  //Distab time get start time for INCA measurement


        // Distab 17 measurement for raster 1
        // 1st parameter -> Event number (starting from 1)
        #ifdef DISTAB_SUPPORTED
        Distab17_Process(1);
        #endif

    /* The Trace trigger is here set in a static way without using supplementary Distab.
    This is not needed if the Trace trigger is set dynamically by supplementary Distab. */
    #ifdef TRACE_SUPPORT
      TraceExecTriggerByValue(1); // Triggering the trigger 1 with Trace by value
    #endif

        getTimeStop = BASE_GetSystemTime();         //task 1 & Distab time get stop time for INCA measurement

        //calculate dT of task 1 (raster 1)
        if (getTimeStop > getRasterTimeStart)
          dT_measurement.dT_Task_01 = getTimeStop - getRasterTimeStart;
        else
          dT_measurement.dT_Task_01 = getRasterTimeStart - getTimeStop;

        //calculate dT of Distab in task 1
        if (getTimeStop > getDistabTimeStart)
          dT_measurement.dT_Distab_01 = getTimeStop - getDistabTimeStart;
        else
          dT_measurement.dT_Distab_01 = getDistabTimeStart - getTimeStop;
      }

      if (measure_parameters.task2_alarm_task !=0 && ISR_counter_task2 >= measure_parameters.task2_alarm_task)
      {
        //reset ISR_counter_task
        ISR_counter_task2 = 0;

        GPIO_ToggleLED(2);

        //*** task 2 ***
        getRasterTimeStart = BASE_GetSystemTime();        //task 2 time get start time for INCA measurement

        //8 bit counters
        measure_values.task2_counter_uint8_4 = Counter_8bit (measure_values.task2_counter_uint8_4, measure_parameters.task2_counter_uint8_max);
        measure_values.task2_counter_uint8_5 = Counter_8bit (measure_values.task2_counter_uint8_5, measure_parameters.task2_counter_uint8_max);
        measure_values.task2_counter_uint8_6 = Counter_8bit (measure_values.task2_counter_uint8_6, measure_parameters.task2_counter_uint8_max);

        //16 bit counters
        measure_values.task2_counter_uint16_4 = Counter_16bit (measure_values.task2_counter_uint16_4, measure_parameters.task2_counter_uint16_max);
        measure_values.task2_counter_uint16_5 = Counter_16bit (measure_values.task2_counter_uint16_5, measure_parameters.task2_counter_uint16_max);
        measure_values.task2_counter_uint16_6 = Counter_16bit (measure_values.task2_counter_uint16_6, measure_parameters.task2_counter_uint16_max);

        //32 bit counters
        measure_values.task2_counter_uint32_4 = Counter_32bit (measure_values.task2_counter_uint32_4, measure_parameters.task2_counter_uint32_max);
        measure_values.task2_counter_uint32_5 = Counter_32bit (measure_values.task2_counter_uint32_5, measure_parameters.task2_counter_uint32_max);
        measure_values.task2_counter_uint32_6 = Counter_32bit (measure_values.task2_counter_uint32_6, measure_parameters.task2_counter_uint32_max);

        cpu0CounterTask(1); // writes 8/16/32/64 bit counters in DSPR0, DSPR1 and LMU RAM - virtual task 1 input

        getDistabTimeStart = BASE_GetSystemTime();  //Distab time get start time for INCA measurement

        // Distab 17 measurement for raster 2
        // 1st parameter -> Event number (starting from 1)
        #ifdef DISTAB_SUPPORTED
        Distab17_Process(2);
        #endif

        /* The Trace trigger is here set in a static way without using supplementary Distab.
        This is not needed if the Trace trigger is set dynamically by supplementary Distab. */
        #ifdef TRACE_SUPPORT
          TraceExecTriggerByValue(2); // Triggering the trigger 1 with Trace by value
        #endif

        getTimeStop = BASE_GetSystemTime();         //task 2 & Distab time get stop time for INCA measurement

        //calculate dT of task 2 (raster 2)
        if (getTimeStop > getRasterTimeStart)
          dT_measurement.dT_Task_02 = getTimeStop - getRasterTimeStart;
        else
          dT_measurement.dT_Task_02 = getRasterTimeStart - getTimeStop;

        //calculate dT of Distab in task 2
        if (getTimeStop > getDistabTimeStart)
          dT_measurement.dT_Distab_02 = getTimeStop - getDistabTimeStart;
        else
          dT_measurement.dT_Distab_02 = getDistabTimeStart - getTimeStop;

      }
      if (measure_parameters.task3_alarm_task !=0 && ISR_counter_task3 >= measure_parameters.task3_alarm_task)
      {
        //reset ISR_counter_task
        ISR_counter_task3 = 0;

        GPIO_ToggleLED(3);

        //*** task 3 ***
        getRasterTimeStart = BASE_GetSystemTime();        //task 2 time get start time for INCA measurement

        //8 bit counters
        measure_values.task3_counter_uint8_7 = Counter_8bit (measure_values.task3_counter_uint8_7, measure_parameters.task3_counter_uint8_max);
        measure_values.task3_counter_uint8_8 = Counter_8bit (measure_values.task3_counter_uint8_8, measure_parameters.task3_counter_uint8_max);
        measure_values.task3_counter_uint8_9 = Counter_8bit (measure_values.task3_counter_uint8_9, measure_parameters.task3_counter_uint8_max);

        //16 bit counters
        measure_values.task3_counter_uint16_7 = Counter_16bit (measure_values.task3_counter_uint16_7, measure_parameters.task3_counter_uint16_max);
        measure_values.task3_counter_uint16_8 = Counter_16bit (measure_values.task3_counter_uint16_8, measure_parameters.task3_counter_uint16_max);
        measure_values.task3_counter_uint16_9 = Counter_16bit (measure_values.task3_counter_uint16_9, measure_parameters.task3_counter_uint16_max);

        //32 bit counters
        measure_values.task3_counter_uint32_7 = Counter_32bit (measure_values.task3_counter_uint32_7, measure_parameters.task3_counter_uint32_max);
        measure_values.task3_counter_uint32_8 = Counter_32bit (measure_values.task3_counter_uint32_8, measure_parameters.task3_counter_uint32_max);
        measure_values.task3_counter_uint32_9 = Counter_32bit (measure_values.task3_counter_uint32_9, measure_parameters.task3_counter_uint32_max);

        cpu0CounterTask(2); // writes 8/16/32/64 bit counters in DSPR0, DSPR1 and LMU RAM - virtual task 2 input
        cpu0CounterTask(3); // writes 8/16/32/64 bit counters in DSPR0, DSPR1 and LMU RAM - virtual task 3 input

        getDistabTimeStart = BASE_GetSystemTime();  //Distab time get start time for INCA measurement

        // Distab 17 measurement for raster 3
        // 1st parameter -> Event number (starting from 1)
        #ifdef DISTAB_SUPPORTED
        Distab17_Process(3);
        #endif

        /* The Trace trigger is here set in a static way without using supplementary Distab.
        This is not needed if the Trace trigger is set dynamically by supplementary Distab. */
        #ifdef TRACE_SUPPORT
          TraceExecTriggerByValue(3); // Triggering the trigger 1 with Trace by value
        #endif

        getTimeStop = BASE_GetSystemTime();         //task 3 & Distab time get stop time for INCA measurement

        //calculate dT of task 3 (raster 3)
        if (getTimeStop > getRasterTimeStart)
          dT_measurement.dT_Task_03 = getTimeStop - getRasterTimeStart;
        else
          dT_measurement.dT_Task_03 = getRasterTimeStart - getTimeStop;

        //calculate dT of Distab in task 3
        if (getTimeStop > getDistabTimeStart)
          dT_measurement.dT_Distab_03 = getTimeStop - getDistabTimeStart;
        else
          dT_measurement.dT_Distab_03 = getDistabTimeStart - getTimeStop;
      }

      //*** task 4 ***
      if (ISR_counter_task4 >= 1000) // 10ms for the handshake and page switching periodic checks
      {
        ISR_counter_task4 = 0;

        SER_Cyclic_Handshake_Check_And_Execute();

        #ifdef CALIBRATION_SUPPORTED
          if (SER_ETK_Check_PAGE_SWITCH_BY_ECU() == 1) // Page switch requested
          {
            SER_ETK_PAGE_SWITCH_BY_ECU();
          }
        #endif
        #ifdef ACC_SUPPORTED
        CopyCodeCheckPattern_cyclic(0); // copies the flash code check pattern to the ram address for the Advanced code check - AAC
        #endif

        updateCalibParamMeasurements(); // updates the measurements of the corresponding test calibration parameters

        // If the last handshake was executed successfully without a timeout
        if (ECU_ETK_Status.Protocol_Success==1 && ECU_ETK_Status.Handshake_End_State==0)
        {
          Led_Counter_01++;
          if (Led_Counter_01 >= 10) // toggle one led every 100 msec
          {
            Led_Counter_01 = 0;
            GPIO_LedBlinkingConsecutive(); // Blinks one of the Leds between the Leds 2 and 7
          }
        }
        /** This function should invalidate the data cache (DMI) for all the cores.
        If this function is called periodically it should ensure that the calibrated parameters value will get updated inside the cpu cache.
        Note: If data cache contains data modified by the cpu, it has to be written back and invalidated by the user ! */
        #ifdef CALIBRATION_SUPPORTED
        dataCacheInvalidate();
        #endif
      }

      //Checking the Data Freeze Safety mailbox and blinking LED 207 for Alive status
      if (ISR_counter_task_alive >= ALIVE_TIME_1s)
      {
        ISR_counter_task_alive = 0;

        //toggle alive LED
        GPIO_ToggleLED(0);

       
      }
    } //while

return 0;
} //main




//counter function uint8
uint8 Counter_8bit (uint8 counterVal, uint8 counterParam)
{
  counterVal++;

  if (counterVal >= counterParam)
    counterVal = 0;

  return counterVal;
}


//counter function uint16
uint16 Counter_16bit (uint16 counterVal, uint16 counterParam)
{
  counterVal++;

  if (counterVal >= counterParam)
    counterVal = 0;

  return counterVal;
}

//counter function uint32
uint32 Counter_32bit (uint32 counterVal, uint32 counterParam)
{
  counterVal++;

  if (counterVal >= counterParam)
    counterVal = 0;

  return counterVal;
}

/*
    uint##T Counter_##T##bit(uint##T v, uint##T p)                \
    {                                                           \
        return (p == v)                                         \
            ? 0                                                 \
            : v + 1;                                            \
    }

COUNTER_FUNC(8);
COUNTER_FUNC(16);
COUNTER_FUNC(32);
*/
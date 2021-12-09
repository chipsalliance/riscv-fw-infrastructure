/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */




#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psp_api.h"
#include "bsp_external_interrupts.h"
/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */
/* Type definitions. */
#if __riscv_xlen == 64
  #define portSTACK_TYPE      uint64_t
  #define portBASE_TYPE      int64_t
  #define portUBASE_TYPE      uint64_t
  #define portMAX_DELAY       ( TickType_t ) 0xffffffffffffffffUL
  #define portPOINTER_SIZE_TYPE   uint64_t
#elif __riscv_xlen == 32
  #define portSTACK_TYPE  uint32_t
  #define portBASE_TYPE  int32_t
  #define portUBASE_TYPE  uint32_t
  #define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#else
  #error Assembler did not define __riscv_xlen
#endif


typedef portSTACK_TYPE StackType_t;
typedef portBASE_TYPE BaseType_t;
typedef portUBASE_TYPE UBaseType_t;
typedef portUBASE_TYPE TickType_t;

/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
not need to be guarded with a critical section. */
#define portTICK_TYPE_IS_ATOMIC 1
/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH      ( -1 )
#define portTICK_PERIOD_MS      ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#ifdef __riscv64
  #error This is the RV32 port that has not yet been adapted for 64.
  #define portBYTE_ALIGNMENT      16
#else
  #define portBYTE_ALIGNMENT 8
#endif
/*-----------------------------------------------------------*/


/* Scheduler utilities. */
#define portYIELD()                              M_PSP_ECALL()
#define portEND_SWITCHING_ISR( xSwitchRequired ) if( xSwitchRequired ) vTaskSwitchContext()
#define portYIELD_FROM_ISR( x )                  portEND_SWITCHING_ISR( x )
/*-----------------------------------------------------------*/


/* Critical section management. */
extern void vTaskEnterCritical( void );
extern void vTaskExitCritical( void );

#define portSET_INTERRUPT_MASK_FROM_ISR() 0

#define portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedStatusValue ) ( void ) uxSavedStatusValue


#define portENTER_CRITICAL()   vTaskEnterCritical()
#define portEXIT_CRITICAL()    vTaskExitCritical()

#if D_ENABLE_FREERTOS_SMP == 1
  #define D_LOCK_TASK        1
  #define D_UNLOCK_TASK      -1
  #define portGET_CORE_ID()  M_PSP_MACHINE_GET_HART_ID()

  /* Note: There are vTaskEnterCritical that calls portDISABLE_INTERRUPTS and portEXIT_CRITICAL that calls portENABLE_INTERRUPTS.
   * So we have to use a global parameter to preserve interrupts status over disable & enable of interrupts */
  extern unsigned int g_uInterruptsPreserveMask[configNUM_CORES], g_uInterruptsDisableCounter[configNUM_CORES];

  // TODO: fixme xassert
  #define xassert(x)

  extern u32_t g_FreeRtosTaskMutex;
  extern u32_t g_FreeRtosIsrMutex;
  extern u32_t g_TaskLockCountArray[configNUM_CORES];
  extern u32_t g_IsrLockCountArray[configNUM_CORES];

  /* ronen TODO: fix me (the extern) */
  extern unsigned int rtosalIsInterruptContext(void);

  D_PSP_INLINE void mutexLockUnlock(u32_t* pMutex, u32_t* pLockCountArray, s32_t siLockUnlock)
  {
    u32_t coreId = portGET_CORE_ID(), uiRes;
    u32_t uiExpectedValue = pLockCountArray[coreId];
    do
    {
      uiRes = M_PSP_ATOMIC_COMPARE_AND_SET(pMutex, uiExpectedValue, uiExpectedValue + siLockUnlock);

    } while(uiRes != 0);
    pLockCountArray[coreId] = uiExpectedValue + siLockUnlock;
  }

  D_PSP_INLINE u32_t disableInterupts(void)
  {
    u32_t /*uiCoreID = M_PSP_MACHINE_GET_HART_ID(),*/ uiTempMask;
    M_PSP_READ_AND_CLEAR_CSR(uiTempMask, D_PSP_MSTATUS_NUM, D_PSP_MSTATUS_MIE_MASK);
    uiTempMask &= D_PSP_MSTATUS_MIE_MASK;
  #if 0
    if (uiTempMask && g_uInterruptsDisableCounter[uiCoreID] == 0)
    {
      //M_PSP_READ_AND_CLEAR_CSR(uiTempMask, D_PSP_MSTATUS_NUM, D_PSP_MSTATUS_MIE_MASK);
      //g_uInterruptsPreserveMask[uiCoreID] = uiTempMask & D_PSP_MSTATUS_MIE_MASK;
      g_uInterruptsPreserveMask[uiCoreID] = uiTempMask;
      //g_uInterruptsDisableCounter[uiCoreID]++;
    }
    g_uInterruptsDisableCounter[uiCoreID]++;
  #endif
    return uiTempMask;
  }

  D_PSP_INLINE void enableInterupts(void)
  {
    //u32_t uiTempMask;
    //u32_t uiCoreID = M_PSP_MACHINE_GET_HART_ID(), uiTempMask;
    //g_uInterruptsDisableCounter[uiCoreID] = 0;
    //uiTempMask = D_PSP_MSTATUS_MIE_MASK;//g_uInterruptsPreserveMask[uiCoreID];
    M_PSP_SET_CSR(D_PSP_MSTATUS_NUM, D_PSP_MSTATUS_MIE_MASK);
    //g_uInterruptsPreserveMask[uiCoreID] = 0;
  }

  #define portDISABLE_INTERRUPTS()              disableInterupts()
  #define portENABLE_INTERRUPTS()               M_PSP_SET_CSR(D_PSP_MSTATUS_NUM, D_PSP_MSTATUS_MIE_MASK) //enableInterupts()
  #define portRESTORE_INTERRUPTS(uiRestoreVal)  M_PSP_SET_CSR(D_PSP_MSTATUS_NUM, uiRestoreVal)

  #if 0
  if (uiRestoreVal & D_PSP_MSTATUS_MIE_MASK) \
                                                { \
                                                  enableInterupts(); \
                                                }
  //M_PSP_SET_CSR(D_PSP_MSTATUS_NUM, uiRestoreVal);
  #endif

  #define portCHECK_IF_IN_ISR()                 (rtosalIsInterruptContext())
  #define portGET_TASK_LOCK()                   mutexLockUnlock(&g_FreeRtosTaskMutex, g_TaskLockCountArray, D_LOCK_TASK)
  #define portRELEASE_TASK_LOCK()               mutexLockUnlock(&g_FreeRtosTaskMutex, g_TaskLockCountArray, D_UNLOCK_TASK)
  #define portYIELD_CORE(xCoreID)               bspGenerateExtInterrupt(xCoreID + D_BSP_IRQ_3, D_PSP_EXT_INT_ACTIVE_HIGH, D_PSP_EXT_INT_LEVEL_TRIG_TYPE );
  #define portGET_ISR_LOCK()                    mutexLockUnlock(&g_FreeRtosIsrMutex, g_IsrLockCountArray, D_LOCK_TASK)
  #define portRELEASE_ISR_LOCK()                mutexLockUnlock(&g_FreeRtosIsrMutex, g_IsrLockCountArray, D_UNLOCK_TASK)
#else
  /* Note: There are vTaskEnterCritical that calls portDISABLE_INTERRUPTS and portEXIT_CRITICAL that calls portENABLE_INTERRUPTS.
   * So we have to use a global parameter to preserve interrupts status over disable & enable of interrupts */
  extern unsigned int g_uInterruptsPreserveMask, g_uInterruptsDisableCounter;
  #define portDISABLE_INTERRUPTS()                         if (g_uInterruptsDisableCounter == 0) \
                                                           { \
  	                                                        M_PSP_READ_AND_CLEAR_CSR(g_uInterruptsPreserveMask, D_PSP_MSTATUS_NUM, D_PSP_MSTATUS_MIE_MASK); \
                                                              g_uInterruptsPreserveMask &= D_PSP_MSTATUS_MIE_MASK; \
                                                           } \
                                                           g_uInterruptsDisableCounter++;

  #define portENABLE_INTERRUPTS()                          g_uInterruptsDisableCounter = 0; \
                                                           M_PSP_SET_CSR(D_PSP_MSTATUS_NUM, g_uInterruptsPreserveMask);
#endif /* D_ENABLE_FREERTOS_SMP */

/*-----------------------------------------------------------*/

/* Architecture specific optimisations. */
#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
  #define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#endif

#if( configUSE_PORT_OPTIMISED_TASK_SELECTION == 1 )

  /* Check the configuration. */
  #if( configMAX_PRIORITIES > 32 )
    #error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32.  It is very rare that a system requires more than 10 to 15 difference priorities as tasks that share a priority will time slice.
  #endif

  /* Store/clear the ready priorities in a bit map. */
  #define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) )
  #define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )

  /*-----------------------------------------------------------*/

  #define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities ) uxTopPriority = ( 31UL - __builtin_clz( uxReadyPriorities ) )

#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */


/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site.  These are
not necessary for to use this port.  They are defined so the common demo files
(which build with all the ports) will build. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

/*-----------------------------------------------------------*/

#define portNOP()              M_PSP_NOP()
#define portINLINE             D_PSP_INLINE
#ifndef portFORCE_INLINE
  #define portFORCE_INLINE   D_PSP_ALWAYS_INLINE
#endif

#define portMEMORY_BARRIER()   M_PSP_MEMORY_BARRIER()

#ifdef __cplusplus
}
#endif


#endif /* PORTMACRO_H */


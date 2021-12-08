/*
* SPDX-License-Identifier: Apache-2.0
* Copyright 2020-2021 Western Digital Corporation or its affiliates.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http:*www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
/**
* @file   psp_interrupts_eh1.h
* @author Nati Rapaport
* @date   14.01.2020
* @brief  The file supplies information and registration API for interrupt and exception service routines on EH1 core.
*/
#ifndef __PSP_INTERRUPTS_EH1_H__
#define __PSP_INTERRUPTS_EH1_H__

/**
* include files
*/

/**
* macros
*/

/**
* types
*/
/* EH1 specific interrupt causes */
typedef enum pspInterruptCauseEH1
{
   E_LAST_COMMON_CAUSE               = E_LAST_CAUSE,
   E_FIRST_EH1_CAUSE                 = 27,
   E_MACHINE_INTERNAL_TIMER1_CAUSE   = 28,
   E_MACHINE_INTERNAL_TIMER0_CAUSE   = 29,
   E_MACHINE_CORRECTABLE_ERROR_CAUSE = 30,
   E_LAST_EH1_CAUSE
} ePspInterruptCauseEH1_t;

/* interrupt handler definition */
typedef void (*fptrPspInterruptHandler_t)(void);

/**
* definitions
*/
#define D_PSP_FIRST_EH1_INT_CAUSE = E_MACHINE_INTERNAL_TIMER1_CAUSE;

/* Enable/Disable bits of Timer0, Timer1 and Correctable-Error-Counter interrupts */
#define D_PSP_INTERRUPTS_MACHINE_INTERNAL_TIMER0              E_MACHINE_INTERNAL_TIMER0_CAUSE
#define D_PSP_INTERRUPTS_MACHINE_INTERNAL_TIMER1              E_MACHINE_INTERNAL_TIMER1_CAUSE
#define D_PSP_INTERRUPTS_MACHINE_CORR_ERR_COUNTER             E_MACHINE_CORRECTABLE_ERROR_CAUSE

/**
* local prototypes
*/

/**
* external prototypes
*/

/**
* macros
*/

/**
* global variables
*/

/**
* APIs
*/

/**
 * @brief - Disable specified interrupt when called in MACHINE-LEVEL
 *                                                     *************
 * IMPORTANT NOTE: When you call this function, you can use either one of the following defined values:
  *************** - D_PSP_INTERRUPTS_MACHINE_SW
                  - D_PSP_INTERRUPTS_MACHINE_TIMER
                  - D_PSP_INTERRUPTS_MACHINE_EXT
                  - D_PSP_INTERRUPTS_SUPERVISOR_SW
                  - D_PSP_INTERRUPTS_SUPERVISOR_TIMER
                  - D_PSP_INTERRUPTS_SUPERVISOR_EXT
                  - D_PSP_INTERRUPTS_USER_SW
                  - D_PSP_INTERRUPTS_USER_TIMER
                  - D_PSP_INTERRUPTS_USER_EXT
                  - D_PSP_INTERRUPTS_MACHINE_INTERNAL_TIMER0
                  - D_PSP_INTERRUPTS_MACHINE_INTERNAL_TIMER1
                  - D_PSP_INTERRUPTS_MACHINE_CORR_ERR_COUNTER
*
* @input parameter - Interrupt number to disable
*
* @return - none
*/
void pspMachineInterruptsDisableIntNumber(u32_t uiInterruptNumber);

/**
*  @brief - Enable specified interrupt when called in MACHINE-LEVEL
*                                                    *************
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values:
  *************** - D_PSP_INTERRUPTS_MACHINE_SW
                  - D_PSP_INTERRUPTS_MACHINE_TIMER
                  - D_PSP_INTERRUPTS_MACHINE_EXT
                  - D_PSP_INTERRUPTS_SUPERVISOR_SW
                  - D_PSP_INTERRUPTS_SUPERVISOR_TIMER
                  - D_PSP_INTERRUPTS_SUPERVISOR_EXT
                  - D_PSP_INTERRUPTS_USER_SW
                  - D_PSP_INTERRUPTS_USER_TIMER
                  - D_PSP_INTERRUPTS_USER_EXT
                  - D_PSP_INTERRUPTS_MACHINE_INTERNAL_TIMER0
                  - D_PSP_INTERRUPTS_MACHINE_INTERNAL_TIMER1
                  - D_PSP_INTERRUPTS_MACHINE_CORR_ERR_COUNTER
*
* @input parameter - Interrupt number to enable
*
* @return - none
*/
void pspMachineInterruptsEnableIntNumber(u32_t uiInterruptNumber);



/**
* @brief - The function returns the address the handler of a given exception
*
* @parameter -  exceptionCause           - exception cause
* @return    -  fptrInterruptHandler     - function pointer to the exception handler
*/
fptrPspInterruptHandler_t pspInterruptsGetExceptionHandler(u32_t uiExceptionCause);

#endif /* __PSP_INTERRUPTS_EH1_H__ */

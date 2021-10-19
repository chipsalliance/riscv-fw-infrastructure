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
* @file   psp_timers_el2.h
* @author Nati Rapaport
* @date   19.08.2020
* @brief  The file defines timer api services for EL2 core.
* 
*/
#ifndef  __PSP_TIMERS_EL2_H__
#define  __PSP_TIMERS_EL2_H__

/**
* include files
*/

/**
* definitions
*/
/* Machine timer and internal timers 1 & 2 are defined in psp_timers_eh1.h */
/* 
   #define D_PSP_INTERNAL_TIMER0 1
   #define D_PSP_INTERNAL_TIMER1 2
*/
/* In SweRV EL2 there is also a 64bit timer */
#define D_PSP_INTERNAL_64BIT_TIMER 3

/**
* types
*/

/**
* local prototypes
*/

/**
* external prototypes
*/

/**
* global variables
*/

/**
* macros
*/

/**
* APIs
*/

/**
* @brief Cascade Timer0 and Timer1 to act as a single timer
*        In this mode, Timer1 counts up when Timer0 reachs its bound value. Timer1 interrupt raises when Timer1 reachs its bound.
*        **Note** In 'cascade' mode HALT-EN, and PAUSE-EN indications must be the set identically for both timers
*                 so part this function also set disable all of them here.
*
* @param - udPeriodCycles - defines the timer's period in cycles
*
*/
void pspMachineInternalTimerSetup64bitTimer(u64_t udPeriodCycles);

#endif /* __PSP_TIMERS_EL2_H__ */

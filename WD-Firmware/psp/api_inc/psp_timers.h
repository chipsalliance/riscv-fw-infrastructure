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
* @file   psp_timers.h
* @author Nati Rapaport
* @date   28.01.2020
* @brief  The file defines core's Machine timer-counter api services.
* 
*/
#ifndef  __PSP_TIMERS_H__
#define  __PSP_TIMERS_H__

/**
* include files
*/

/**
* definitions
*/

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
* @brief Setup and activate  core Machine Timer
*
*
* @param - udPeriodCycles  - defines the timer's period in cycles
*
*/
void pspMachineTimerCounterSetupAndRun(u64_t udPeriodCycles);

/**
* @brief Get Machine Timer counter value
*
*
* @return u64_t      - Timer counter value
*
*/
u64_t pspMachineTimerCounterGet(void);

/**
* @brief Get Machine Time compare counter value
*
*
* @return u64_t      – Time compare counter value
*
*/
u64_t pspMachineTimerCompareCounterGet(void);

#endif /* __PSP_TIMERS_H__*/

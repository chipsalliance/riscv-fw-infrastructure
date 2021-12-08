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
* @file   psp_timers_eh1.h
* @author Nati Rapaport
* @date   05.05.2020
* @brief  The file defines timer api services for EH1 core.
* 
*/
#ifndef  __PSP_TIMERS_EH1_H__
#define  __PSP_TIMERS_EH1_H__

/**
* include files
*/

/**
* definitions
*/
#define D_PSP_INTERNAL_TIMER0 1
#define D_PSP_INTERNAL_TIMER1 2


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
* @brief Setup  Internal core's Timer
*
* @param - uiTimer           - indicates which timer to setup
*
* @param - uiPeriodCycles  - defines the timer's period in cycles
*
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values for uiTimer:
*                   - D_PSP_INTERNAL_TIMER0
*                   - D_PSP_INTERNAL_TIMER1
*
*/
void pspMachineInternalTimerCounterSetup(u32_t uiTimer, u32_t uiPeriodCycles);

/**
* @brief Enable incrementing timer counter(Run)
*
* @param - uiTimer  - indicates which timer to Run
*
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values for uiTimer:
*                   - D_PSP_INTERNAL_TIMER0
*                   - D_PSP_INTERNAL_TIMER1
*/
void pspMachineInternalTimerRun(u32_t uiTimer);

/**
* @brief Disable incrementing timer counter
*
* @param - uiTimer  - indicates which timer to disable
*
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values for uiTimer:
*                   - D_PSP_INTERNAL_TIMER0
*                   - D_PSP_INTERNAL_TIMER1
*/
void pspMachineInternalTimerPause(u32_t uiTimer);

/**
* @brief Get Core Internal Timer counter value
*
* @param - uitimer - indicates from which timer to get the counter value
*
* @return u32_t      - Timer counter value
*
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values for uiTimer:
*                   - D_PSP_INTERNAL_TIMER0
*                   - D_PSP_INTERNAL_TIMER1
*
*/
u32_t pspMachineInternalTimerCounterGet(u32_t uiTimer);


/**
* @brief Get Core Internal Timer compare counter value
*
* @param - uitimer - indicates from which timer to get the compare-counter value
*
* @return u32_t      – Time compare counter value
*
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values for uiTimer:
*                   - D_PSP_INTERNAL_TIMER0
*                   - D_PSP_INTERNAL_TIMER1
*/
u32_t pspMachineInternalTimerCompareCounterGet(u32_t uiTimer);

/**
* @brief Enable Core Internal timer counting when core in sleep mode
*
* @param - uiTimer  - indicates which timer to Enable
*
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values for uiTimer:
*                   - D_PSP_INTERNAL_TIMER0
*                   - D_PSP_INTERNAL_TIMER1
*
*/
void pspMachineInternalTimerEnableCountInSleepMode(u32_t uiTimer);

/**
* @brief Disable Core Internal timer counting when core in sleep mode
*
* @param - uiTimer  - indicates which timer to Disable
*
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values for uiTimer:
*                   - D_PSP_INTERNAL_TIMER0
*                   - D_PSP_INTERNAL_TIMER1
*
*/
void pspMachineInternalTimerDisableCountInSleepMode(u32_t uiTimer);

/**
* @brief Enable Core Internal timer counting when core in in stall
*
* @param - uiTimer  - indicates which timer to Enable
*
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values for uiTimer:
*                   - D_PSP_INTERNAL_TIMER0
*                   - D_PSP_INTERNAL_TIMER1
*
*/
void pspMachineInternalTimerEnableCountInStallMode(u32_t uiTimer);

/**
* @brief Disable Core Internal timer counting when core in in stall
*
* @param - uiTimer  - indicates which timer to Disable
*
*
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values for uiTimer:
*                   - D_PSP_INTERNAL_TIMER0
*                   - D_PSP_INTERNAL_TIMER1
*
*/
void pspMachineInternalTimerDisableCountInStallMode(u32_t uiTimer);




#endif /* __PSP_TIMERS_EH1_H__ */

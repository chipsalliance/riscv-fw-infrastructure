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
* @file   psp_timers.c
* @author Nati Rapaport
* @date   13.11.2019
* @brief  This file implements core's Machine timer-counter api services.
*/

/**
* include files
*/
#include "psp_api.h"

/**
* definitions
*/
#if defined(D_MTIME_ADDRESS) && defined(D_MTIMECMP_ADDRESS)
  #define D_PSP_MTIME_ADDRESS D_MTIME_ADDRESS
  #define D_PSP_MTIMECMP_ADDRESS D_MTIMECMP_ADDRESS
#else
  #error "D_MTIME_ADDRESS or D_MTIMECMP_ADDRESS are not defined"
#endif

/**
* macros
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
* APIs
*/
 
 
/**
* @brief Setup and activate core Machine Timer
*
*
* @param - udPeriodCycles  - defines the timer's period in cycles 
*
*/
D_PSP_TEXT_SECTION void pspMachineTimerCounterSetupAndRun(u64_t udPeriodCycles)
{
  M_PSP_ASSERT((D_PSP_MTIME_ADDRESS != 0) && (D_PSP_MTIMECMP_ADDRESS != 0));

  /* Set the mtime and mtimecmp (memory-mapped registers) per privileged spec */
  volatile u64_t *pMtime       = (u64_t*)D_PSP_MTIME_ADDRESS;
  volatile u64_t *pMtimecmp    = (u64_t*)D_PSP_MTIMECMP_ADDRESS;
  u64_t udNow = *pMtime;
  u64_t udThen = udNow + udPeriodCycles;
  *pMtimecmp = udThen;
}

/**
* @brief Get Machine Timer counter value
*
*
* @return u64_t      - Timer counter value
*
*/
D_PSP_TEXT_SECTION u64_t pspMachineTimerCounterGet(void)
{
  volatile u64_t *pMtime       = (u64_t*)D_PSP_MTIME_ADDRESS;
  return *pMtime;
}

/**
* @brief Get Machine Time compare counter value
*
*
* @return u64_t      – Time compare counter value
*
*/
D_PSP_TEXT_SECTION u64_t pspMachineTimerCompareCounterGet(void)
{
  volatile u64_t *pMtimecmp    = (u64_t*)D_PSP_MTIMECMP_ADDRESS;
  return *pMtimecmp;
}



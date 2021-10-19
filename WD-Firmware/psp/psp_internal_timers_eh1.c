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
* @file   psp_internal_timers_eh1.c
* @author Nati Rapaport
* @date   8.12.2019
* @brief  This file implements EH1 timers service functions
*
*/
/**
* include files
*/
#include "psp_api.h"


/**
* definitions
*/
 
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
 * Internal functions
 */


/**
* @brief Setup and activate Internal core's Timer
*
* @param - uiTimer           - indicates which timer to setup and run
*
* @param - udPeriodCycles  - defines the timer's period in cycles 
*
*/
D_PSP_TEXT_SECTION void pspMachineInternalTimerCounterSetupAndRun(u32_t uiTimer, u64_t udPeriodCycles)
{
  u32_t uiNow, uiThen;
  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer));
  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
     /* Read Timer0 counter */
      uiNow = M_PSP_READ_CSR(D_PSP_MITCNT0_NUM);
      uiThen = uiNow + (u32_t)udPeriodCycles;
      /* Set Timer0 bound */
      M_PSP_WRITE_CSR(D_PSP_MITBND0_NUM, uiThen);
      /* Enable Timer0 counting */
      M_PSP_SET_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_EN_MASK);
      break;
    case D_PSP_INTERNAL_TIMER1:
      /* Read Timer1 counter */
      uiNow = M_PSP_READ_CSR(D_PSP_MITCNT1_NUM);
      uiThen = uiNow + (u32_t)udPeriodCycles;
      /* Set Timer1 bound */
      M_PSP_WRITE_CSR(D_PSP_MITBND1_NUM, uiThen);
      /* Enable Timer1 counting */
      M_PSP_SET_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_EN_MASK);
      break;
    default:
      break;
  }
}




/**
* @brief Get Core Internal Timer counter value
*
* @param - uitimer - indicates from which timer to get the counter value
*
* @return u64_t      - Timer counter value
*
*/
D_PSP_TEXT_SECTION u64_t pspMachineInternalTimerCounterGet(u32_t uiTimer)
{
  u64_t udCounter = 0;

  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
     
    case D_PSP_INTERNAL_TIMER0:
      udCounter = (u64_t)M_PSP_READ_CSR(D_PSP_MITCNT0_NUM);
      break;
    case D_PSP_INTERNAL_TIMER1:
      udCounter = (u64_t)M_PSP_READ_CSR(D_PSP_MITCNT1_NUM);
      break;
    default:
      break;
  }

  return (udCounter);
}


/**
* @brief Get Core Internal Timer compare counter value
*
* @param - uitimer - indicates from which timer to get the compare-counter value
*
* @return u64_t      – Time compare counter value
*
*/
D_PSP_TEXT_SECTION u64_t pspMachineInternalTimerCompareCounterGet(u32_t uiTimer)
{
  u64_t udCounterCompare = 0;

  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {

    case D_PSP_INTERNAL_TIMER0:
      udCounterCompare = (u64_t)M_PSP_READ_CSR(D_PSP_MITBND0_NUM);
      break;
    case D_PSP_INTERNAL_TIMER1:
      udCounterCompare = (u64_t)M_PSP_READ_CSR(D_PSP_MITBND1_NUM);
      break;
    default:
      break;
  }

  return (udCounterCompare);
}

/**
* @brief Enable Core Internal timer counting when core in sleep mode
*
* @param - uiTimer  - indicates which timer to setup
*
*/
D_PSP_TEXT_SECTION void pspMachineInternalTimerEnableCountInSleepMode(u32_t uiTimer)
{
  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      M_PSP_SET_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_HALT_EN_MASK);
      break;
    case D_PSP_INTERNAL_TIMER1:
      M_PSP_SET_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_HALT_EN_MASK);
      break;
    default:
      break;
  }
}

/**
* @brief Disable Core Internal timer counting when core in sleep mode
*
* @param - uiTimer  - indicates which timer to setup
*
*/
D_PSP_TEXT_SECTION void pspMachineInternalTimerDisableCountInSleepMode(u32_t uiTimer)
{
  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_HALT_EN_MASK);
      break;
    case D_PSP_INTERNAL_TIMER1:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_HALT_EN_MASK);
      break;
    default:
      break;
  }
}

/**
* @brief Enable Core Internal timer counting when core in in stall
*
* @param - uiTimer  - indicates which timer to setup
*
*/
D_PSP_TEXT_SECTION void pspMachineInternalTimerEnableCountInStallMode(u32_t uiTimer)
{
  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      M_PSP_SET_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    case D_PSP_INTERNAL_TIMER1:
      M_PSP_SET_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    default:
      break;
  }
}

/**
* @brief Disable Core Internal timer counting when core in in stall
*
* @param - uiTimer  - indicates which timer to setup
*
*/
D_PSP_TEXT_SECTION void pspMachineInternalTimerDisableCountInStallMode(u32_t uiTimer)
{
  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    case D_PSP_INTERNAL_TIMER1:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    default:
      break;
  }
}

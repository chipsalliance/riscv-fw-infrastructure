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
* @file   psp_timers_el2.c
* @author Nati Rapaport
* @date   19.08.2020
* @brief  This file implements EL2 timers service functions
*
*/
/**
* include files
*/
#include "psp_api.h"



 
/**
* definitions
*/
#if defined(D_MTIME_ADDRESS) && defined(D_MTIMECMP_ADDRESS)
  #define D_PSP_MTIME_ADDRESS  D_MTIME_ADDRESS
  #define D_PSP_MTIMECMP_ADDRESS D_MTIMECMP_ADDRESS
#else
  #error "D_MTIME_ADDRESS or D_MTIMECMP_ADDRESS is not defined"
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
 * Internal functions
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
D_PSP_TEXT_SECTION void pspMachineInternalTimerSetup64bitTimer(u64_t udPeriodCycles)
{
  u32_t uiNow, uiThen;

  /* Read Timer0 counter */
  uiNow = M_PSP_READ_CSR(D_PSP_MITCNT0_NUM);
  /* Add the lower 32bit of the input 'udPeriodCycles' parameter */
  uiThen = uiNow + (u32_t)udPeriodCycles;
  /* Set Timer0 bound */
  M_PSP_WRITE_CSR(D_PSP_MITBND0_NUM, uiThen);
  /* Read Timer1 counter */
  uiNow = M_PSP_READ_CSR(D_PSP_MITCNT1_NUM);
  /* Add the upper 32bit of the input 'udPeriodCycles' parameter */
  uiThen = uiNow + (u32_t)(udPeriodCycles >> D_PSP_SHIFT_32);
  /* Set Timer0 bound */
  M_PSP_WRITE_CSR(D_PSP_MITBND1_NUM, uiThen);

  /* In cascade mode, the HALT-EN, and PAUSE-EN bits must be the set identically for both timers - so disable all of them them now */
  M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
  M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
  M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_HALT_EN_MASK);
  M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_HALT_EN_MASK);

  /* Enable Timer0 and Timer1 counting */
  M_PSP_SET_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_EN_MASK);
  M_PSP_SET_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_EN_MASK);
}

/**
* @brief Setup and activate  core Machine Timer
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

  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) ||(D_PSP_INTERNAL_TIMER1 == uiTimer) || (D_PSP_INTERNAL_64BIT_TIMER == uiTimer));

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
    case D_PSP_INTERNAL_64BIT_TIMER:
      pspMachineInternalTimerSetup64bitTimer(udPeriodCycles);
      break;
    default:
      break;
  }
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
  u64_t udCounterTemp = 0;

  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer) || (D_PSP_INTERNAL_64BIT_TIMER == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      udCounter = (u64_t)M_PSP_READ_CSR(D_PSP_MITCNT0_NUM);
      break;
    case D_PSP_INTERNAL_TIMER1:
      udCounter = (u64_t)M_PSP_READ_CSR(D_PSP_MITCNT1_NUM);
      break;
    case D_PSP_INTERNAL_64BIT_TIMER:
    udCounterTemp = M_PSP_READ_CSR(D_PSP_MITCNT1_NUM);
    udCounter = udCounterTemp << D_PSP_SHIFT_32;
    udCounter |= M_PSP_READ_CSR(D_PSP_MITCNT0_NUM);
      break;
    default:
      break;
  }

  return (udCounter);
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
  u64_t udCounterCompareTemp = 0;

  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer) || (D_PSP_INTERNAL_64BIT_TIMER == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      udCounterCompare = (u64_t)M_PSP_READ_CSR(D_PSP_MITBND0_NUM);
      break;
    case D_PSP_INTERNAL_TIMER1:
      udCounterCompare = (u64_t)M_PSP_READ_CSR(D_PSP_MITBND1_NUM);
      break;
    case D_PSP_INTERNAL_64BIT_TIMER:
      udCounterCompareTemp = M_PSP_READ_CSR(D_PSP_MITBND1_NUM);
      udCounterCompare = udCounterCompareTemp << D_PSP_SHIFT_32;
      udCounterCompare |= M_PSP_READ_CSR(D_PSP_MITBND0_NUM);
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
  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer) || (D_PSP_INTERNAL_64BIT_TIMER == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      M_PSP_SET_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_HALT_EN_MASK);
      break;
    case D_PSP_INTERNAL_TIMER1:
      M_PSP_SET_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_HALT_EN_MASK);
      break;
    case D_PSP_INTERNAL_64BIT_TIMER:
      M_PSP_SET_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_HALT_EN_MASK);
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
  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer) || (D_PSP_INTERNAL_64BIT_TIMER == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_HALT_EN_MASK);
      break;
    case D_PSP_INTERNAL_TIMER1:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_HALT_EN_MASK);
      break;
    case D_PSP_INTERNAL_64BIT_TIMER:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_HALT_EN_MASK);
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
  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer) || (D_PSP_INTERNAL_64BIT_TIMER == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      M_PSP_SET_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    case D_PSP_INTERNAL_TIMER1:
      M_PSP_SET_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    case D_PSP_INTERNAL_64BIT_TIMER:
      M_PSP_SET_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
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
   M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer) || (D_PSP_INTERNAL_64BIT_TIMER == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    case D_PSP_INTERNAL_TIMER1:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    case D_PSP_INTERNAL_64BIT_TIMER:
        M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
        M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    default:
      break;
  }
}

 



/* add user code begin Header */
/**
  **************************************************************************
  * @file     wk_system.c
  * @brief    work bench config program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */
/* add user code end Header */

#include "wk_system.h"

#define STEP_DELAY_MS                    (uint32_t)(50)
#define TICK_COUNT_MAX                   (uint32_t)(0xFFFFFF)
#define TICK_COUNT_VALUE                 (SysTick->VAL)

/* global variable */
volatile uint32_t ticks_count_us;

/**
  * @brief  this function provides minimum delay (in microsecond).
  * @param  delay: specifies the delay time length, in microsecond.
  * @retval none
  */
__WEAK void wk_delay_us(uint32_t delay)
{
  uint32_t delay_ticks, pre_ticks, cur_ticks, delta;
  delay_ticks = delay * ticks_count_us;

  pre_ticks = TICK_COUNT_VALUE;
  do
  {
    cur_ticks = TICK_COUNT_VALUE;
    /* count down */
    delta = (cur_ticks <= pre_ticks) ? (pre_ticks - cur_ticks) : ((TICK_COUNT_MAX - cur_ticks) + pre_ticks + 1);
  } while(delta < delay_ticks);
}

/**
  * @brief  this function provides minimum delay (in milliseconds).
  * @param  delay variable specifies the delay time length, in milliseconds.
  * @retval none
  */
__WEAK void wk_delay_ms(uint32_t delay)
{
  while(delay)
  {
    if(delay > STEP_DELAY_MS)
    {
      wk_delay_us(STEP_DELAY_MS * 1000);
      delay -= STEP_DELAY_MS;
    }
    else
    {
      wk_delay_us(delay * 1000);
      delay = 0;
    }
  }
}

/**
  * @brief  this function configures the source of the time base.
  * @param  none
  * @retval none
  */
__WEAK void wk_timebase_init(void)
{
  crm_clocks_freq_type crm_clocks;
  uint32_t frequency = 0;

  /* get crm_clocks */
  crm_clocks_freq_get(&crm_clocks);

  frequency = crm_clocks.ahb_freq;

  /* config systick clock source */
  systick_clock_source_config(SYSTICK_CLOCK_SOURCE_AHBCLK_NODIV);
  ticks_count_us = (frequency / 1000000U);
  /* system tick config */
  TICK_COUNT_VALUE = 0UL;
  SysTick->LOAD = TICK_COUNT_MAX;
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}


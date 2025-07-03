/* add user code begin Header */
/**
  **************************************************************************
  * @file     wk_acc.c
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

/* Includes ------------------------------------------------------------------*/
#include "wk_acc.h"

/* add user code begin 0 */

/* add user code end 0 */

/**
  * @brief  init acc function
  * @param  none
  * @retval none
  */
void wk_acc_init(void)
{
  /* add user code begin acc_init 0 */

  /* add user code end acc_init 0 */

  /* update the c1\c2\c3 value */
  acc_write_c1(7980);
  acc_write_c2(8000);
  acc_write_c3(8020);

  /* add user code begin acc_init 1 */

  /* add user code end acc_init 1 */

  /* open acc calibration */
  acc_calibration_mode_enable(ACC_CAL_HICKTRIM, TRUE);

  /* add user code begin acc_init 2 */

  /* add user code end acc_init 2 */
}

/* add user code begin 1 */

/* add user code end 1 */

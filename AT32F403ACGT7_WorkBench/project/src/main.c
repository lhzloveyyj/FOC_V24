/* add user code begin Header */
/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
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
#include "at32f403a_407_wk_config.h"
#include "wk_acc.h"
#include "wk_debug.h"
#include "wk_spi.h"
#include "wk_tmr.h"
#include "wk_usbfs.h"
#include "wk_gpio.h"
#include "usb_app.h"
#include "wk_system.h"

/* private includes ----------------------------------------------------------*/
/* add user code begin private includes */
#include "mt6701.h"
#include "delay.h"
#include "log.h"
/* add user code end private includes */

/* private typedef -----------------------------------------------------------*/
/* add user code begin private typedef */

/* add user code end private typedef */

/* private define ------------------------------------------------------------*/
/* add user code begin private define */

/* add user code end private define */

/* private macro -------------------------------------------------------------*/
/* add user code begin private macro */

/* add user code end private macro */

/* private variables ---------------------------------------------------------*/
/* add user code begin private variables */

/* add user code end private variables */

/* private function prototypes --------------------------------------------*/
/* add user code begin function prototypes */

/* add user code end function prototypes */

/* private user code ---------------------------------------------------------*/
/* add user code begin 0 */

/* add user code end 0 */

/**
  * @brief main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  /* add user code begin 1 */

  /* add user code end 1 */

  /* system clock config. */
  wk_system_clock_config();

  /* config periph clock. */
  wk_periph_clock_config();

  /* init debug function. */
  wk_debug_config();

  /* nvic config. */
  wk_nvic_config();

  /* timebase config. */
  wk_timebase_init();

  /* init gpio function. */
  wk_gpio_config();

  /* init spi1 function. */
  wk_spi1_init();

  /* init tmr1 function. */
  wk_tmr1_init();

  /* init tmr2 function. */
  wk_tmr2_init();

  /* init acc function. */
  wk_acc_init();

  /* init usbfs function. */
  wk_usbfs_init();

  /* init usb app function. */
  wk_usb_app_init();

  /* add user code begin 2 */
  delay_init();
  
  delay_ms(2000);
  
  angle_init(MT6701_GetAngleWrapper);
  
   float angle = 0;
    float Ud = 1.0f;  // 合适电压

  
  tmr_interrupt_enable(TMR2,TMR_OVF_INT,TRUE);
  /* add user code end 2 */

  while(1)
  {
     wk_usb_app_task();

    /* add user code begin 3 */
//	  usb_printf("i 呜呜呜 lhz\r\n");
    /* add user code end 3 */
  }
}

  /* add user code begin 4 */

  /* add user code end 4 */

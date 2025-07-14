/* add user code begin Header */
/**
  **************************************************************************
  * @file     usbd_app.c
  * @brief    usb device app
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

#include "usb_conf.h"
#include "wk_system.h"

#include "usbd_int.h"
#include "cdc_class.h"
#include "cdc_desc.h"

/* private includes ----------------------------------------------------------*/
/* add user code begin private includes */
#include "LOG.h"
#include "MT6701.h"
#include "FOC.h"
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

usbd_core_type usb_core_dev;

uint32_t usbd_app_buffer_fs1[128];

/* private user code ---------------------------------------------------------*/
/* add user code begin 0 */

/* add user code end 0 */

/**
  * @brief  usb application initialization
  * @param  none
  * @retval none
  */
void wk_usb_app_init(void)
{
  /* add user code begin usb_app_init 0 */
	
  /* add user code end usb_app_init 0 */

  /*fs1 device cdc*/
  usbd_core_init(&usb_core_dev, USB, &cdc_class_handler, &cdc_desc_handler, 0);

  /* enable usb pull-up */
  usbd_connect(&usb_core_dev);

  /* add user code begin usb_app_init 1 */

  /* add user code end usb_app_init 1 */
}

/**
  * @brief  usb application task
  * @param  none
  * @retval none
  */
void wk_usb_app_task(void)
{
  /* add user code begin usb_app_task 0 */

  /* add user code end usb_app_task 0 */

  uint32_t length = 0;

  uint32_t timeout = 5000000;
  static uint8_t send_zero_packet = 0;

  /* add user code begin usb_app_task 1 */
	uint8_t buffer[128];
	//float data[1] = {mt6701.angle};
	//float data[3] = {Ua, Ub, Uc};
	//uint16_t len = PackFloatArrayToBytes(data, 3, buffer);
	
	
	//uint8_t A  = usb_vcp_send_data(&usb_core_dev, (uint8_t *)buffer, len);
	
  /* add user code end usb_app_task 1 */

  /* fs1 device cdc */
  length = usb_vcp_get_rxdata(&usb_core_dev, (uint8_t *)usbd_app_buffer_fs1);
  if(length > 0 || send_zero_packet == 1)
  {
    if(length > 0)
      send_zero_packet = 1;

    if(length == 0)
      send_zero_packet = 0;
    
    do
    {
      /* send data to host */
      if(usb_vcp_send_data(&usb_core_dev, (uint8_t *)usbd_app_buffer_fs1, length) == SUCCESS)
      {
        break;
      }
    }while(timeout --);
  }

  /* add user code begin usb_app_task 2 */

  /* add user code end usb_app_task 2 */
}

/**
  * @brief  usb interrupt handler
  * @param  none
  * @retval none
  */
void wk_usbfs_irq_handler(void)
{
  /* add user code begin otgfs1_irq_handler 0 */

  /* add user code end otgfs1_irq_handler 0 */

  usbd_irq_handler(&usb_core_dev);

  /* add user code begin otgfs1_irq_handler 1 */

  /* add user code end otgfs1_irq_handler 1 */
}

/**
  * @brief  usb delay function
  * @param  ms: delay number of milliseconds.
  * @retval none
  */
void usb_delay_ms(uint32_t ms)
{
  /* add user code begin delay_ms 0 */

  /* add user code end delay_ms 0 */

  wk_delay_ms(ms);

  /* add user code begin delay_ms 1 */

  /* add user code end delay_ms 1*/
}

/* add user code begin 1 */

/* add user code end 1 */

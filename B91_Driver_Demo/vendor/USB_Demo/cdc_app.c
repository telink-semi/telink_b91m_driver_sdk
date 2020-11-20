/********************************************************************************************************
 * @file	cdc_app.c
 *
 * @brief	This is the source file for B91
 *
 * @author	Driver Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/
#include "app_config.h"
#if(USB_DEMO_TYPE==USB_CDC)
#include "application/usb_app/usbcdc.h"
#include "application/usbstd/usb.h"

extern unsigned char usb_cdc_data[CDC_TXRX_EPSIZE];
extern unsigned short usb_cdc_data_len;
void user_init(void)
{
	//1.enable USB DP pull up 1.5k
	usb_set_pin_en();
	//2.enable USB manual interrupt(in auto interrupt mode,USB device would be USB printer device)
	usb_init_interrupt();
	//3.set ep5 BUSY(BUSY same as ACK) bit as 1 means output endpoint buffer can receive data from USB host
	usbhw_data_ep_ack(USB_EDP_CDC_OUT);
	//4.enable global interrupt
	core_interrupt_enable();
	usbhw_set_irq_mask(USB_IRQ_RESET_MASK|USB_IRQ_SUSPEND_MASK);
}

void main_loop (void)
{
	usb_handle_irq();

	if(usb_cdc_data_len!=0)
			{
			usb_cdc_tx_data_to_host(usb_cdc_data,usb_cdc_data_len);
			usb_cdc_data_len = 0;
		}

}

#endif


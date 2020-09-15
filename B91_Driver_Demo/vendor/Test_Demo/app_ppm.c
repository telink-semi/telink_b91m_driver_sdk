/********************************************************************************************************
 * @file	app_ppm.c
 *
 * @brief	This is the source file for B91
 *
 * @author	Z.W.H
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

#if(TEST_MODE == TEST_PPM_MODE)


#define PWM_CLK_SPEED	24000000 //pwm clock .

void rc_24m_cal_pwm (void)
{
	pwm_set_pin(PWM_PWM1_PE1);	//pe_io[1] = 0,PE1
	pwm_set_clk((unsigned char) (sys_clk.pclk*1000*1000/PWM_CLK_SPEED-1));
	pwm_set_tcmp(PWM1_ID, 0x0001);		//pwm1_cmp
	pwm_set_tmax(PWM1_ID, 0x0002); 		//pwm1_max
	pwm_start(PWM1_ID); 				//pwm1_en = 1
	clock_cal_24m_rc();
}
void rc_32k_cal_pwm (void)
{
	clock_32k_init(CLK_32K_RC);
	pwm_set_pin(PWM_PWM1_PE1);	//pe_io[1] = 0,PE1
	pwm_32k_chn_en(PWM_CLOCK_32K_CHN_PWM1);
	pwm_set_tcmp(PWM1_ID, 0x0001);		//pwm1_cmp
	pwm_set_tmax(PWM1_ID, 0x0002); 		//pwm1_max
	pwm_start(PWM1_ID); 				//pwm1_en = 1
	clock_cal_32k_rc();
}

void user_init(void)
{
	gpio_function_en(LED1);
	gpio_output_en(LED1);

//	rc_24m_cal_pwm();
	rc_32k_cal_pwm();
}


void main_loop(void)
{
    delay_ms(500);
	gpio_toggle(LED1);
}

#endif
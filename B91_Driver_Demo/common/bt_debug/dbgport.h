/********************************************************************************************************
 * @file	dbgport.h
 *
 * @brief	This is the header file for B91
 *
 * @author	L.J.W
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
#ifndef DBGPORT_H_
#define DBGPORT_H_
#include "../../drivers/B91/gpio.h"
#include "../assert.h"


/**
 * Debug Port Usage:
 * 1. set DIAGCNTL-DIAGEN0/1/2/3 as 1 to enable DIAG0/1/2/3
 * 2. set DIAGCNTL-DIAG0/1/2/3<6:0> as target value ,refer to Table 2-46 in RW-BT-CORE-FS/2.19 Validation and test
 *    DIAG0/1/2/3STAT<0:7>  <->  DiagOut<0:7>  <-> bt_dbg0/1/2/3<0:7>
 * 3. set dbg_sel_bt0/1/2/3 <-> bt_dbg0/1/2/3<0:7> <-> DBG_BB<0:31> <-> PA<0:4> ,PB<0:7>,PC<0:7>,PD<0:7>,PE<0:2>
 *	  set dbg_sel_bt0(GPIO_BASE+0x55)<12> -> DiagOut<0:7> <-> bt_dbg0<0:7> <-> DBG_BB<0:7>
 *	  set dbg_sel_bt1(GPIO_BASE+0x55)<13> -> DiagOut<0:7> <-> bt_dbg1<0:7> <-> DBG_BB<8:15>
 *	  set dbg_sel_bt2(GPIO_BASE+0x55)<14> -> DiagOut<0:7> <-> bt_dbg2<0:7> <-> DBG_BB<16:23>
 *	  set dbg_sel_bt3(GPIO_BASE+0x55)<15> -> DiagOut<0:7> <-> bt_dbg3<0:7> <-> DBG_BB<24:31>
 * 4. set gpio mux function as DBG_BBx
 *
 * Take "radcntl_txen <-> GPIO_PA0" signal for example:
 * 1. DIAGCNTL-DIAGEN0=1 (enable DIAG0)
 * 2. DIAG0<6:0> = 0x03  (OUTPUT "radcntl_txen" signal)
 * 3. (GPIO_BASE+0x55)<12> =1 (DiagOut<0:7> <-> bt_dbg0<0:7> <-> DBG_BB<0:7> <-> PA<0:4> ,PB<0:2>)
 *    So, radcntl_txen <-> DiagOut<0> <-> bt_dbg0<0> <-> DBG_BB<0> <-> PA<0>
 * 4. 0x140300<1:0>=3 to enable GPIO MUX function as DBG_BB0
 * */

#define REG_BTCORE_BASE_ADDR  	(0x160400)
#define BT_DIAGCNTL_ADDR   		(REG_BTCORE_BASE_ADDR+0x50)

typedef enum{
	BT_DBG0_BB0_A0  = GPIO_PA0,
	BT_DBG0_BB1_A1  = GPIO_PA1,
	BT_DBG0_BB2_A2  = GPIO_PA2,
	BT_DBG0_BB3_A3  = GPIO_PA3,
	BT_DBG0_BB4_A4  = GPIO_PA4,
	BT_DBG0_BB5_B0  = GPIO_PB0,
	BT_DBG0_BB6_B1  = GPIO_PB1,
	BT_DBG0_BB7_B2  = GPIO_PB2,

	BT_DBG1_BB8_B3  = GPIO_PB3,
	BT_DBG1_BB9_B4  = GPIO_PB4,
	BT_DBG1_BB10_B5 = GPIO_PB5,
	BT_DBG1_BB11_B6 = GPIO_PB6,
	BT_DBG1_BB12_B7 = GPIO_PB7,
	BT_DBG1_BB13_C0 = GPIO_PC0,
	BT_DBG1_BB14_C1 = GPIO_PC1,
	BT_DBG1_BB15_C2 = GPIO_PC2,

	BT_DBG2_BB16_C3 = GPIO_PC3,
	BT_DBG2_BB17_C4 = GPIO_PC4,
	BT_DBG2_BB18_C5 = GPIO_PC5,
	BT_DBG2_BB19_C6 = GPIO_PC6,
	BT_DBG2_BB20_C7 = GPIO_PC7,
	BT_DBG2_BB21_D0 = GPIO_PD0,
	BT_DBG2_BB22_D1 = GPIO_PD1,
	BT_DBG2_BB23_D2 = GPIO_PD2,

	BT_DBG3_BB24_D3 = GPIO_PD3,
	BT_DBG3_BB25_D4 = GPIO_PD4,
	BT_DBG3_BB26_D5 = GPIO_PD5,
	BT_DBG3_BB27_D6 = GPIO_PD6,
	BT_DBG3_BB28_D7 = GPIO_PD7,
	BT_DBG3_BB29_E0 = GPIO_PE0,
	BT_DBG3_BB30_E1 = GPIO_PE1,
	BT_DBG3_BB31_E2 = GPIO_PE2,
}btdbg_pin_e;



#define reg_bt_dbg_sel      REG_ADDR16(0x140354)
typedef enum{
	BT_DBG_SEL_EN	= BIT(8),
	BT_DBG_SEL_DBG0 = BIT(12),
	BT_DBG_SEL_DBG1 = BIT(13),
	BT_DBG_SEL_DBG2 = BIT(14),
	BT_DBG_SEL_DBG3 = BIT(15),
}bt_signal_group_e;

void bt_dbg_set_pin(btdbg_pin_e pin);
void bt_dbg_set_signal(bt_signal_group_e signal_group,u8 signal_byte);

static inline void bt_diagcntl_diag0_setf(unsigned char diag0)
{
    assert((((unsigned int)diag0 << 0) & ~((unsigned int)0x0000007F)) == 0);
    write_reg32(BT_DIAGCNTL_ADDR, (read_reg32(BT_DIAGCNTL_ADDR) & ~((unsigned int)0x0000007F)) | ((unsigned int)diag0 << 0));
}

static inline void bt_diagcntl_diag0_en_setf(unsigned char diag0en)
{
    assert((((unsigned int)diag0en << 7) & ~((unsigned int)0x00000080)) == 0);
    write_reg32(BT_DIAGCNTL_ADDR, (read_reg32(BT_DIAGCNTL_ADDR) & ~((unsigned int)0x00000080)) | ((unsigned int)diag0en << 7));
}

static inline void bt_diagcntl_diag1_setf(unsigned char diag1)
{
    assert((((unsigned int)diag1 << 8) & ~((unsigned int)0x00007F00)) == 0);
    write_reg32(BT_DIAGCNTL_ADDR, (read_reg32(BT_DIAGCNTL_ADDR) & ~((unsigned int)0x00007F00)) | ((unsigned int)diag1 << 8));
}

static inline void bt_diagcntl_diag1_en_setf(unsigned char diag1en)
{
    assert((((unsigned int)diag1en << 15) & ~((unsigned int)0x00008000)) == 0);
    write_reg32(BT_DIAGCNTL_ADDR, (read_reg32(BT_DIAGCNTL_ADDR) & ~((unsigned int)0x00008000)) | ((unsigned int)diag1en << 15));
}

static inline void bt_diagcntl_diag2_setf(unsigned char diag2)
{
    assert((((unsigned int)diag2 << 16) & ~((unsigned int)0x007F0000)) == 0);
    write_reg32(BT_DIAGCNTL_ADDR, (read_reg32(BT_DIAGCNTL_ADDR) & ~((unsigned int)0x007F0000)) | ((unsigned int)diag2 << 16));
}

static inline void bt_diagcntl_diag2_en_setf(unsigned char diag2en)
{
    assert((((unsigned int)diag2en << 23) & ~((unsigned int)0x00800000)) == 0);
    write_reg32(BT_DIAGCNTL_ADDR, (read_reg32(BT_DIAGCNTL_ADDR) & ~((unsigned int)0x00800000)) | ((unsigned int)diag2en << 23));
}

static inline void bt_diagcntl_diag3_setf(unsigned char diag3)
{
    assert((((unsigned int)diag3 << 24) & ~((unsigned int)0x7F000000)) == 0);
    write_reg32(BT_DIAGCNTL_ADDR, (read_reg32(BT_DIAGCNTL_ADDR) & ~((unsigned int)0x7F000000)) | ((unsigned int)diag3 << 24));
}

static inline void bt_diagcntl_diag3_en_setf(unsigned char diag3en)
{
    assert((((unsigned int)diag3en << 31) & ~((unsigned int)0x80000000)) == 0);
    write_reg32(BT_DIAGCNTL_ADDR, (read_reg32(BT_DIAGCNTL_ADDR) & ~((unsigned int)0x80000000)) | ((unsigned int)diag3en << 31));
}

#endif /* DBGPORT_H_ */

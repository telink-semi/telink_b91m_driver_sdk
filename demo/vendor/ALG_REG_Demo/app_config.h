/********************************************************************************************************
 * @file	app_config.h
 *
 * @brief	This is the header file for B91m
 *
 * @author	Driver Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#pragma once

#include "driver.h"
/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif
#if(MCU_CORE_B91)
	
#define LED1            GPIO_PB4
#define LED2            GPIO_PB5
#define LED3            GPIO_PB6
#define LED4            GPIO_PB7

#elif(MCU_CORE_B92 || MCU_CORE_B93)
	
#define LED1            GPIO_PD0
#define LED2            GPIO_PD1
#define LED3            GPIO_PD2
#define LED4            GPIO_PD3

#endif







#define ALG_BYTE_MODE	        0
#define ALG_HWORD_MODE          1
#define ALG_WORD_MODE 	        2
#define ALG_BUFF_MODE 	        3
#define ALG_DMA_WORD_MODE 	    4
#define ALG_DMA_BUFF_MODE 		5
#define ALG_DMA_ADDR_DATA_MODE  6

#define ALG_MODE				ALG_BYTE_MODE

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
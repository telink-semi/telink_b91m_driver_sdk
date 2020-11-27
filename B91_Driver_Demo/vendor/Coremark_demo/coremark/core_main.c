/********************************************************************************************************
 * @file	core_main.c
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
/* File: core_main.c
	This file contains the framework to acquire a block of memory, seed initial parameters, tun t he benchmark and report the results.
*/
#include "../../../drivers.h"
#include "coremark.h"
#include "core_portme.h"


/* Function: iterate
	Run the benchmark for a specified number of iterations.

	Operation:
	For each type of benchmarked algorithm:
		a - Initialize the data block for the algorithm.
		b - Execute the algorithm N times.

	Returns:
	NULL.
*/
static ee_u16 list_known_crc[]   =      {(ee_u16)0xd4b0,(ee_u16)0x3340,(ee_u16)0x6a79,(ee_u16)0xe714,(ee_u16)0xe3c1};
static ee_u16 matrix_known_crc[] =      {(ee_u16)0xbe52,(ee_u16)0x1199,(ee_u16)0x5608,(ee_u16)0x1fd7,(ee_u16)0x0747};
static ee_u16 state_known_crc[]  =      {(ee_u16)0x5e47,(ee_u16)0x39bf,(ee_u16)0xe5a4,(ee_u16)0x8e3a,(ee_u16)0x8d84};
void *iterate(void *pres) {
	ee_u32 i;
	ee_u16 crc;
	core_results *res=(core_results *)pres;
	ee_u32 iterations=res->iterations;
	res->crc=0;
	res->crclist=0;
	res->crcmatrix=0;
	res->crcstate=0;

	for (i=0; i<iterations; i++) {
		crc=core_bench_list(res,1);
		res->crc=crcu16(crc,res->crc);
		crc=core_bench_list(res,-1);
		res->crc=crcu16(crc,res->crc);
		if (i==0) res->crclist=res->crc;
	}
	return NULL;
}

#if (SEED_METHOD==SEED_ARG)
ee_s32 get_seed_args(int i, int argc, char *argv[]);
#define get_seed(x) (ee_s16)get_seed_args(x,argc,argv)
#define get_seed_32(x) get_seed_args(x,argc,argv)
#else /* via function or volatile */
ee_s32 get_seed_32(int i);
#define get_seed(x) (ee_s16)get_seed_32(x)
#endif

#if (MEM_METHOD==MEM_STATIC)
ee_u8 static_memblk[TOTAL_DATA_SIZE];
#endif
char *mem_name[3] = {"Static","Heap","Stack"};



/* Function: main
	Main entry routine for the benchmark.
	This function is responsible for the following steps:

	1 - Initialize input seeds from a source that cannot be determined at compile time.
	2 - Initialize memory block for use.
	3 - Run and time the benchmark.
	4 - Report results, testing the validity of the output if the seeds are known.

	Arguments:
	1 - first seed  : Any value
	2 - second seed : Must be identical to first for iterations to be identical
	3 - third seed  : Any value, should be at least an order of magnitude less then the input size, but bigger then 32.
	4 - Iterations  : Special, if set to 0, iterations will be automatically determined such that the benchmark will run between 10 to 100 secs

*/
#define LED_BLUE        GPIO_PB4
#define LED_GREEN       GPIO_PB5
#define LED_WHITE       GPIO_PB6
#define LED_RED         GPIO_PB7

#if MAIN_HAS_NOARGC

float  coremark_result;

MAIN_RETURN_TYPE main_coremark(void) {
	int argc=0;
	char *argv[1];
#else
MAIN_RETURN_TYPE main(int argc, char *argv[]) {
#endif

#if 0
	sys_init(LDO_1P4_LDO_1P8, VBAT_MAX_VALUE_GREATER_THAN_3V6);
	//clock_init(PLL_CLK_192M, PAD_PLL_DIV, PLL_DIV6_TO_CCLK,	CCLK_DIV2_TO_HCLK, HCLK_DIV2_TO_PCLK, CCLK_TO_MSPI_CLK);
	clock_init(PLL_CLK_96M, PAD_PLL_DIV, PLL_DIV3_TO_CCLK,	CCLK_DIV2_TO_HCLK, HCLK_DIV2_TO_PCLK, CCLK_TO_MSPI_CLK);
	gpio_function_en	(LED_BLUE|LED_RED|LED_GREEN|LED_WHITE);
	gpio_output_en	(LED_BLUE|LED_RED|LED_GREEN|LED_WHITE);
	reg_usb_ep8_send_thre = 64;

	//myudb_set_txfifo_local ();
	// initial USB & pull up DP
	//myudb_usb_init (0x120, 0);
	usb_set_pin_en ();
#endif
	while ((read_reg8(0x10080b) & 0x7f) == 0);
	gpio_set_high_level (LED_BLUE);
	gpio_set_high_level (LED_RED);

	ee_u16 i,j=0,num_algorithms=0;
	ee_s16 known_id=-1,total_errors=0;
	ee_u16 seedcrc=0;
	CORE_TICKS total_time;
	core_results results[MULTITHREAD];
#if (MEM_METHOD==MEM_STACK)
	ee_u8 stack_memblock[TOTAL_DATA_SIZE*MULTITHREAD];
#endif
	/* first call any initializations needed */
	portable_init(&(results[0].port), &argc, argv);



	/* First some checks to make sure benchmark will run ok */
	if (sizeof(struct list_head_s)>128) {
		printf("list_head structure too big for comparable data!\r\n");
		return MAIN_RETURN_VAL;
	}
	results[0].seed1=get_seed(1);
	results[0].seed2=get_seed(2);
	results[0].seed3=get_seed(3);
	results[0].iterations=get_seed_32(4);
#if CORE_DEBUG
	results[0].iterations=1;
#endif
	results[0].execs=get_seed_32(5);
	if (results[0].execs==0) { /* if not supplied, execute all algorithms */
		results[0].execs=ALL_ALGORITHMS_MASK;
	}
		/* put in some default values based on one seed only for easy testing */
	if ((results[0].seed1==0) && (results[0].seed2==0) && (results[0].seed3==0)) { /* validation run */
		results[0].seed1=0;
		results[0].seed2=0;
		results[0].seed3=0x66;
	}
	if ((results[0].seed1==1) && (results[0].seed2==0) && (results[0].seed3==0)) { /* perfromance run */
		results[0].seed1=0x3415;
		results[0].seed2=0x3415;
		results[0].seed3=0x66;
	}
#if (MEM_METHOD==MEM_STATIC)
	results[0].memblock[0]=(void *)static_memblk;
	results[0].size=TOTAL_DATA_SIZE;
	results[0].err=0;
	#if (MULTITHREAD>1)
	#error "Cannot use a static data area with multiple contexts!"
	#endif
#elif (MEM_METHOD==MEM_MALLOC)
	for (i=0 ; i<MULTITHREAD; i++) {
		ee_s32 malloc_override=get_seed(7);
		if (malloc_override != 0)
			results[i].size=malloc_override;
		else
			results[i].size=TOTAL_DATA_SIZE;
		results[i].memblock[0]=portable_malloc(results[i].size);
		results[i].seed1=results[0].seed1;
		results[i].seed2=results[0].seed2;
		results[i].seed3=results[0].seed3;
		results[i].err=0;
		results[i].execs=results[0].execs;
	}
#elif (MEM_METHOD==MEM_STACK)
	for (i=0 ; i<MULTITHREAD; i++) {
		results[i].memblock[0]=stack_memblock+i*TOTAL_DATA_SIZE;
		results[i].size=TOTAL_DATA_SIZE;
		results[i].seed1=results[0].seed1;
		results[i].seed2=results[0].seed2;
		results[i].seed3=results[0].seed3;
		results[i].err=0;
		results[i].execs=results[0].execs;
	}
#else
#error "Please define a way to initialize a memory block."
#endif
	/* Data init */
	/* Find out how space much we have based on number of algorithms */
	for (i=0; i<NUM_ALGORITHMS; i++) {
		if ((1<<(ee_u32)i) & results[0].execs)
			num_algorithms++;
	}
	for (i=0 ; i<MULTITHREAD; i++)
		results[i].size=results[i].size/num_algorithms;
	/* Assign pointers */
	for (i=0; i<NUM_ALGORITHMS; i++) {
		ee_u32 ctx;
		if ((1<<(ee_u32)i) & results[0].execs) {
			for (ctx=0 ; ctx<MULTITHREAD; ctx++)
				results[ctx].memblock[i+1]=(char *)(results[ctx].memblock[0])+results[0].size*j;
			j++;
		}
	}
	/* call inits */
	for (i=0 ; i<MULTITHREAD; i++) {
		if (results[i].execs & ID_LIST) {
			results[i].list=core_list_init(results[0].size,results[i].memblock[1],results[i].seed1);
		}
		if (results[i].execs & ID_MATRIX) {
			core_init_matrix(results[0].size, results[i].memblock[2], (ee_s32)results[i].seed1 | (((ee_s32)results[i].seed2) << 16), &(results[i].mat) );
		}
		if (results[i].execs & ID_STATE) {
			core_init_state(results[0].size,results[i].seed1,results[i].memblock[3]);
		}
	}

	/* automatically determine number of iterations if not set */
	if (results[0].iterations==0) {
		secs_ret secs_passed=0;
		ee_u32 divisor;
		results[0].iterations=1;
		while (secs_passed < (secs_ret)1) {
			results[0].iterations*=10;
			start_time();
			iterate(&results[0]);
			stop_time();
			secs_passed=time_in_secs(get_time());
		}
		/* now we know it executes for at least 1 sec, set actual run time at about 10 secs */
		divisor=(ee_u32)secs_passed;
		if (divisor==0) /* some machines cast float to int as 0 since this conversion is not defined by ANSI, but we know at least one second passed */
			divisor=1;
		results[0].iterations*=1+10/divisor;
	}
	/* perform actual benchmark */
	start_time();
#if (MULTITHREAD>1)
	if (default_num_contexts>MULTITHREAD) {
		default_num_contexts=MULTITHREAD;
	}
	for (i=0 ; i<default_num_contexts; i++) {
		results[i].iterations=results[0].iterations;
		results[i].execs=results[0].execs;
		core_start_parallel(&results[i]);
	}
	for (i=0 ; i<default_num_contexts; i++) {
		core_stop_parallel(&results[i]);
	}
#else
	iterate(&results[0]);
#endif
	stop_time();
	total_time=get_time();
	/* get a function of the input to report */
	seedcrc=crc16(results[0].seed1,seedcrc);
	seedcrc=crc16(results[0].seed2,seedcrc);
	seedcrc=crc16(results[0].seed3,seedcrc);
	seedcrc=crc16(results[0].size,seedcrc);

	switch (seedcrc) { /* test known output for common seeds */
		case 0x8a02: /* seed1=0, seed2=0, seed3=0x66, size 2000 per algorithm */
			known_id=0;
			printf("6k performance run parameters for coremark.\r\n");
			break;
		case 0x7b05: /*  seed1=0x3415, seed2=0x3415, seed3=0x66, size 2000 per algorithm */
			known_id=1;
			printf("6k validation run parameters for coremark.\r\n");
			break;
		case 0x4eaf: /* seed1=0x8, seed2=0x8, seed3=0x8, size 400 per algorithm */
			known_id=2;
			printf("Profile generation run parameters for coremark.\r\n");
			break;
		case 0xe9f5: /* seed1=0, seed2=0, seed3=0x66, size 666 per algorithm */
			known_id=3;
			printf("2K performance run parameters for coremark.\r\n");
			break;
		case 0x18f2: /*  seed1=0x3415, seed2=0x3415, seed3=0x66, size 666 per algorithm */
			known_id=4;
			printf("2K validation run parameters for coremark.\r\n");
			break;
		default:
			total_errors=-1;
			break;
	}
	if (known_id>=0) {
		for (i=0 ; i<default_num_contexts; i++) {
			results[i].err=0;
			if ((results[i].execs & ID_LIST) &&
				(results[i].crclist!=list_known_crc[known_id])) {
				printf("[%u]ERROR! list crc 0x%04x - should be 0x%04x\r\n",i,results[i].crclist,list_known_crc[known_id]);
				results[i].err++;
			}
			if ((results[i].execs & ID_MATRIX) &&
				(results[i].crcmatrix!=matrix_known_crc[known_id])) {
				printf("[%u]ERROR! matrix crc 0x%04x - should be 0x%04x\r\n",i,results[i].crcmatrix,matrix_known_crc[known_id]);
				results[i].err++;
			}
			if ((results[i].execs & ID_STATE) &&
				(results[i].crcstate!=state_known_crc[known_id])) {
				printf("[%u]ERROR! state crc 0x%04x - should be 0x%04x\r\n",i,results[i].crcstate,state_known_crc[known_id]);
				results[i].err++;
			}
			total_errors+=results[i].err;
		}
	}
	total_errors+=check_data_types();
	gpio_set_high_level (LED_GREEN);
	/* and report results */
	printf("CoreMark Size    : %d\r\n",(ee_u32)results[0].size);
	printf("Total ticks      : %d\r\n",(ee_u32)total_time);
#if HAS_FLOAT
	printf("Total time (secs): %f\r\n",time_in_secs(total_time));
	if (time_in_secs(total_time) > 0)
		printf("Iterations/Sec   : %f\r\n",default_num_contexts*results[0].iterations/time_in_secs(total_time));
#else
	printf("Total time (secs): %d\r\n",time_in_secs(total_time));
	if (time_in_secs(total_time) > 0)
		printf("Iterations/Sec   : %d\r\n",default_num_contexts*results[0].iterations/time_in_secs(total_time));
#endif
	if (time_in_secs(total_time) < 10) {
		printf("ERROR! Must execute for at least 10 secs for a valid result!\r\n");
		total_errors++;
	}

	printf("Iterations       : %d\r\n",(ee_u32)default_num_contexts*results[0].iterations);
	printf("Compiler version : %s\r\n",COMPILER_VERSION);
	printf("Compiler flags   : %s\r\n",COMPILER_FLAGS);
#if (MULTITHREAD>1)
	printf("Parallel %s : %d\r\n",PARALLEL_METHOD,default_num_contexts);
#endif
	printf("Memory location  : %s\r\n",MEM_LOCATION);
	/* output for verification */
	printf("seedcrc          : 0x%04x\r\n",seedcrc);
	if (results[0].execs & ID_LIST)
		for (i=0 ; i<default_num_contexts; i++)
			printf("[%d]crclist       : 0x%04x\r\n",i,results[i].crclist);
	if (results[0].execs & ID_MATRIX)
		for (i=0 ; i<default_num_contexts; i++)
			printf("[%d]crcmatrix     : 0x%04x\r\n",i,results[i].crcmatrix);
	if (results[0].execs & ID_STATE)
		for (i=0 ; i<default_num_contexts; i++)
			printf("[%d]crcstate      : 0x%04x\r\n",i,results[i].crcstate);
	for (i=0 ; i<default_num_contexts; i++)
		printf("[%d]crcfinal      : 0x%04x\r\n",i,results[i].crc);
	if (total_errors==0) {
		printf("Correct operation validated. See readme.txt for run and reporting rules.\r\n");
#if HAS_FLOAT
		if (known_id==3) {
			printf("CoreMark 1.0 : %f / %s %s",default_num_contexts*results[0].iterations/time_in_secs(total_time),COMPILER_VERSION,COMPILER_FLAGS);

			coremark_result = default_num_contexts*results[0].iterations/time_in_secs(total_time);
#if defined(MEM_LOCATION) && !defined(MEM_LOCATION_UNSPEC)
			printf(" / %s",MEM_LOCATION);
#else
			printf(" / %s",mem_name[MEM_METHOD]);
#endif

#if (MULTITHREAD>1)
			printf(" / %d:%s",default_num_contexts,PARALLEL_METHOD);
#endif
			printf("\r\n");
		}
#endif
	}
	if (total_errors>0)
		printf("Errors detected\r\n");
	if (total_errors<0)
		printf("Cannot validate operation for these seed values, please compare with results on a known platform.\r\n");

#if (MEM_METHOD==MEM_MALLOC)
	for (i=0 ; i<MULTITHREAD; i++)
		portable_free(results[i].memblock[0]);
#endif
	/* And last call any target specific code for finalizing */
	portable_fini(&(results[0].port));
	reg_usb_ep8_send_thre = 1;
	gpio_set_high_level (LED_WHITE);

	return MAIN_RETURN_VAL;
}



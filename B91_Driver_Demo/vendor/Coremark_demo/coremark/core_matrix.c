/********************************************************************************************************
 * @file	core_matrix.c
 *
 * @brief	This is the source file for B91
 *
 * @author	Z.X.D / D.M.H
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
 
#include "coremark.h"
/*
Topic: Description
	Matrix manipulation benchmark
	
	This very simple algorithm forms the basis of many more complex algorithms. 
	
	The tight inner loop is the focus of many optimizations (compiler as well as hardware based) 
	and is thus relevant for embedded processing. 
	
	The total available data space will be divided to 3 parts:
	NxN Matrix A - initialized with small values (upper 3/4 of the bits all zero).
	NxN Matrix B - initialized with medium values (upper half of the bits all zero).
	NxN Matrix C - used for the result.

	The actual values for A and B must be derived based on input that is not available at compile time.
*/
ee_s16 matrix_test(ee_u32 N, MATRES *C, MATDAT *A, MATDAT *B, MATDAT val);
ee_s16 matrix_sum(ee_u32 N, MATRES *C, MATDAT clipval);
void matrix_mul_const(ee_u32 N, MATRES *C, MATDAT *A, MATDAT val);
void matrix_mul_vect(ee_u32 N, MATRES *C, MATDAT *A, MATDAT *B);
void matrix_mul_matrix(ee_u32 N, MATRES *C, MATDAT *A, MATDAT *B);
void matrix_mul_matrix_bitextract(ee_u32 N, MATRES *C, MATDAT *A, MATDAT *B);
void matrix_add_const(ee_u32 N, MATDAT *A, MATDAT val);

#define matrix_test_next(x) (x+1)
#define matrix_clip(x,y) ((y) ? (x) & 0x0ff : (x) & 0x0ffff)
#define matrix_big(x) (0xf000 | (x))
#define bit_extract(x,from,to) (((x)>>(from)) & (~(0xffffffff << (to))))

#if CORE_DEBUG
void printmat(MATDAT *A, ee_u32 N, char *name) {
	ee_u32 i,j;
	ee_printf("Matrix %s [%dx%d]:\n",name,N,N);
	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {
			if (j!=0)
				ee_printf(",");
			ee_printf("%d",A[i*N+j]);
		}
		ee_printf("\n");
	}
}
void printmatC(MATRES *C, ee_u32 N, char *name) {
	ee_u32 i,j;
	ee_printf("Matrix %s [%dx%d]:\n",name,N,N);
	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {
			if (j!=0)
				ee_printf(",");
			ee_printf("%d",C[i*N+j]);
		}
		ee_printf("\n");
	}
}
#endif
/* Function: core_bench_matrix
	Benchmark function

	Iterate <matrix_test> N times, 
	changing the matrix values slightly by a constant amount each time.
*/
ee_u16 core_bench_matrix(mat_params *p, ee_s16 seed, ee_u16 crc) {
	ee_u32 N=p->N;
	MATRES *C=p->C;
	MATDAT *A=p->A;
	MATDAT *B=p->B;
	MATDAT val=(MATDAT)seed;

	crc=crc16(matrix_test(N,C,A,B,val),crc);

	return crc;
}

/* Function: matrix_test
	Perform matrix manipulation.

	Parameters:
	N - Dimensions of the matrix.
	C - memory for result matrix.
	A - input matrix
	B - operator matrix (not changed during operations)

	Returns:
	A CRC value that captures all results calculated in the function.
	In particular, crc of the value calculated on the result matrix 
	after each step by <matrix_sum>.

	Operation:
	
	1 - Add a constant value to all elements of a matrix.
	2 - Multiply a matrix by a constant.
	3 - Multiply a matrix by a vector.
	4 - Multiply a matrix by a matrix.
	5 - Add a constant value to all elements of a matrix.

	After the last step, matrix A is back to original contents.
*/
ee_s16 matrix_test(ee_u32 N, MATRES *C, MATDAT *A, MATDAT *B, MATDAT val) {
	ee_u16 crc=0;
	MATDAT clipval=matrix_big(val);

	matrix_add_const(N,A,val); /* make sure data changes  */
#if CORE_DEBUG
	printmat(A,N,"matrix_add_const");
#endif
	matrix_mul_const(N,C,A,val);
	crc=crc16(matrix_sum(N,C,clipval),crc);
#if CORE_DEBUG
	printmatC(C,N,"matrix_mul_const");
#endif
	matrix_mul_vect(N,C,A,B);
	crc=crc16(matrix_sum(N,C,clipval),crc);
#if CORE_DEBUG
	printmatC(C,N,"matrix_mul_vect");
#endif
	matrix_mul_matrix(N,C,A,B);
	crc=crc16(matrix_sum(N,C,clipval),crc);
#if CORE_DEBUG
	printmatC(C,N,"matrix_mul_matrix");
#endif
	matrix_mul_matrix_bitextract(N,C,A,B);
	crc=crc16(matrix_sum(N,C,clipval),crc);
#if CORE_DEBUG
	printmatC(C,N,"matrix_mul_matrix_bitextract");
#endif
	
	matrix_add_const(N,A,-val); /* return matrix to initial value */
	return crc;
}

/* Function : matrix_init
	Initialize the memory block for matrix benchmarking.

	Parameters:
	blksize - Size of memory to be initialized.
	memblk - Pointer to memory block.
	seed - Actual values chosen depend on the seed parameter.
	p - pointers to <mat_params> containing initialized matrixes.

	Returns:
	Matrix dimensions.
	
	Note:
	The seed parameter MUST be supplied from a source that cannot be determined at compile time
*/
ee_u32 core_init_matrix(ee_u32 blksize, void *memblk, ee_s32 seed, mat_params *p) {
	ee_u32 N=0;
	MATDAT *A;
	MATDAT *B;
	ee_s32 order=1;
	MATDAT val;
	ee_u32 i=0,j=0;
	if (seed==0)
		seed=1;
	while (j<blksize) {
		i++;
		j=i*i*2*4;		
	}
	N=i-1;
	A=(MATDAT *)align_mem(memblk);
	B=A+N*N;

	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {
			seed = ( ( order * seed ) % 65536 );
			val = (seed + order);
			val=matrix_clip(val,0);
			B[i*N+j] = val;
			val =  (val + order);
			val=matrix_clip(val,1);
			A[i*N+j] = val;
			order++;
		}
	}

	p->A=A;
	p->B=B;
	p->C=(MATRES *)align_mem(B+N*N);
	p->N=N;
#if CORE_DEBUG
	printmat(A,N,"A");
	printmat(B,N,"B");
#endif
	return N;
}

/* Function: matrix_sum
	Calculate a function that depends on the values of elements in the matrix.

	For each element, accumulate into a temporary variable.
	
	As long as this value is under the parameter clipval, 
	add 1 to the result if the element is bigger then the previous.
	
	Otherwise, reset the accumulator and add 10 to the result.
*/
ee_s16 matrix_sum(ee_u32 N, MATRES *C, MATDAT clipval) {
	MATRES tmp=0,prev=0,cur=0;
	ee_s16 ret=0;
	ee_u32 i,j;
	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {
			cur=C[i*N+j];
			tmp+=cur;
			if (tmp>clipval) {
				ret+=10;
				tmp=0;
			} else {
				ret += (cur>prev) ? 1 : 0;
			}
			prev=cur;
		}
	}
	return ret;
}

/* Function: matrix_mul_const
	Multiply a matrix by a constant.
	This could be used as a scaler for instance.
*/
void matrix_mul_const(ee_u32 N, MATRES *C, MATDAT *A, MATDAT val) {
	ee_u32 i,j;
	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {
			C[i*N+j]=(MATRES)A[i*N+j] * (MATRES)val;
		}
	}
}

/* Function: matrix_add_const
	Add a constant value to all elements of a matrix.
*/
void matrix_add_const(ee_u32 N, MATDAT *A, MATDAT val) {
	ee_u32 i,j;
	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {
			A[i*N+j] += val;
		}
	}
}

/* Function: matrix_mul_vect
	Multiply a matrix by a vector.
	This is common in many simple filters (e.g. fir where a vector of coefficients is applied to the matrix.)
*/
void matrix_mul_vect(ee_u32 N, MATRES *C, MATDAT *A, MATDAT *B) {
	ee_u32 i,j;
	for (i=0; i<N; i++) {
		C[i]=0;
		for (j=0; j<N; j++) {
			C[i]+=(MATRES)A[i*N+j] * (MATRES)B[j];
		}
	}
}

/* Function: matrix_mul_matrix
	Multiply a matrix by a matrix.
	Basic code is used in many algorithms, mostly with minor changes such as scaling.
*/
void matrix_mul_matrix(ee_u32 N, MATRES *C, MATDAT *A, MATDAT *B) {
	ee_u32 i,j,k;
	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {
			C[i*N+j]=0;
			for(k=0;k<N;k++)
			{
				C[i*N+j]+=(MATRES)A[i*N+k] * (MATRES)B[k*N+j];
			}
		}
	}
}

/* Function: matrix_mul_matrix_bitextract
	Multiply a matrix by a matrix, and extract some bits from the result.
	Basic code is used in many algorithms, mostly with minor changes such as scaling.
*/
void matrix_mul_matrix_bitextract(ee_u32 N, MATRES *C, MATDAT *A, MATDAT *B) {
	ee_u32 i,j,k;
	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {
			C[i*N+j]=0;
			for(k=0;k<N;k++)
			{
				MATRES tmp=(MATRES)A[i*N+k] * (MATRES)B[k*N+j];
				C[i*N+j]+=bit_extract(tmp,2,4)*bit_extract(tmp,5,7);
			}
		}
	}
}
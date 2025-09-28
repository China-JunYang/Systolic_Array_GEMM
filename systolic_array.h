#ifndef __SYSTOLIC_ARRAY_H
#define __SYSTOLIC_ARRAY_H

#include <ap_int.h>

// A（左）矩阵的行大小
#define M 14
// A（左）矩阵的列大小 = B（上）矩阵的行大小
#define K 9
// B（上）矩阵的列大小
#define N 14


typedef ap_int<8> data_t; //8位有符号定点数

// typedef int data_t; //debug方便看到数值


void PE_Array(data_t A[M][K], data_t B[K][N], ap_int<32> C[M][N], bool reset_C);

#endif

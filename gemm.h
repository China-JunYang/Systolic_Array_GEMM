#ifndef __GEMM_H
#define __GEMM_H

#include "systolic_array.h"

void GEMM_Tiling(data_t A[], data_t B[], ap_int<16> C[], int fm_row, int fm_col, int krnl_col);

#endif

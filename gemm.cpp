#include "systolic_array.h"

void Read_A_2_A_block(data_t* A, data_t A_block[M][K], int fm_row, int fm_col, int tiling_row, int tiling_col)
{
#pragma HLS array_partition variable=A_block complete dim=2
    Read_A_LOOP:
    for(int itr = 0, i = 0, j = 0; itr < M * K; itr++, j++){
#pragma HLS PIPELINE
        if (j == K){
            j = 0;
            i++;    
        }
        if(tiling_row + i < fm_row && tiling_col + j < fm_col){
            A_block[i][j] = A[(tiling_row + i) * fm_col + (tiling_col + j)];            
        }else{
            A_block[i][j] = (data_t)(0);            
        }
                     
    }
}

void Read_B_2_B_block(data_t* B, data_t B_block[K][N], int fm_col, int krnl_col, int tiling_row, int tiling_col)
{
#pragma HLS array_partition variable=B_block complete dim=2
    Read_B_LOOP:
    for(int itr = 0, i = 0, j = 0; itr < K * N; itr++, j++){
#pragma HLS PIPELINE
        if(j == N){
            j = 0;
            i++;    
        }
        if(tiling_row + i < fm_col && tiling_col + j < krnl_col){
            B_block[i][j] = B[(tiling_row + i) * krnl_col + (tiling_col + j)];            
        }else{
            B_block[i][j] = (data_t)(0);            
        }             
    }
}

void Write_C_block_2_C(ap_int<16>* C, ap_int<16> C_block[M][N], int fm_row, int krnl_col, int tiling_row, int tiling_col)
{
#pragma HLS array_partition variable=C_block complete dim=2
    Write_C_LOOP:
    for(int itr = 0, i = 0, j = 0; itr < M * N; itr++, j++){
#pragma HLS PIPELINE
        if(j == N){
            j = 0;
            i++;    
        }
        if(tiling_row + i < fm_row && tiling_col + j < krnl_col){
            C[(tiling_row + i) * krnl_col + (tiling_col + j)] = C_block[i][j];            
        }            
    }
}

void GEMM_Tiling(data_t* A, data_t* B, ap_int<16>* C, int fm_row, int fm_col, int krnl_col)
{
#pragma HLS INTERFACE m_axi depth=65535 port=A offset=slave 
#pragma HLS INTERFACE m_axi depth=65535 port=B offset=slave 
#pragma HLS INTERFACE m_axi depth=65535 port=C offset=slave 
#pragma HLS INTERFACE s_axilite port=fm_row   
#pragma HLS INTERFACE s_axilite port=fm_col   
#pragma HLS INTERFACE s_axilite port=krnl_col 
#pragma HLS INTERFACE s_axilite port=return 

	 data_t A_block[M][K];
#pragma HLS array_partition variable=A_block complete dim=1
	 data_t B_block[K][N];
#pragma HLS array_partition variable=B_block complete dim=2
	 ap_int<16> C_block[M][N];
#pragma HLS array_partition variable=C_block complete dim=0

	bool reset_C = false;
#pragma HLS DATAFLOW
    FM_ROW_LOOP:    
	for(int i=0; i<fm_row; i+=M){
        KRNL_LOOP:
		for(int j=0; j<krnl_col; j+=N){
            FM_COL_LOOP:
			// 从DRAM的A和B-->A[fm_row, fm_col]和B[fm_col, krnl_col]中按照[M * K]和[K * N]的大小读取分块矩阵到A_block和B_block
			for(int k=0; k<fm_col; k+=K){
				reset_C = (k==0) ? true : false;
				// read A to A_block
				Read_A_2_A_block(A, A_block, fm_row, fm_col, i, k);// read A
				// read B to B_block
				Read_B_2_B_block(B, B_block, fm_col, krnl_col, k, j);// read B 
				// DSP48 Array Calculation - Systolic Array Implement GEMM
                PE_Array(A_block, B_block, C_block, reset_C);// Calculation C
			}
			// write C_block to C
			Write_C_block_2_C(C, C_block, fm_row, krnl_col, i, j);// write C
		}
	}
}


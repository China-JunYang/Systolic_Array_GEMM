#include <hls_stream.h>
#include "systolic_array.h"

// 每个PE的内部执行结构
void PE(hls::stream<data_t> &A_pre, hls::stream<data_t> &B_pre, hls::stream<data_t> &A_nxt, hls::stream<data_t> &B_nxt, ap_int<16> &C, bool reset_C)
{
	data_t A_tmp, B_tmp;
	if(reset_C){
		C = 0;
	}
	PE_LOOP:
	for(int k=0; k<K; k++){
#pragma HLS PIPELINE
		// shift-in A & B
		A_pre >> A_tmp;
		B_pre >> B_tmp;
		// MAC
		C += A_tmp * B_tmp;
		// shift-out A & B
		A_nxt << A_tmp;
		B_nxt << B_tmp;
	}

}

// 按行读取A矩阵的Buffer
void Load_A(int m, data_t A[M][K], hls::stream<data_t> &A_nxt)
{
	for(int k=0; k<K; k++){
#pragma HLS PIPELINE
		A_nxt << A[m][k];
	}

}

// 按列读取A矩阵的Buffer
void Load_B(int n, data_t B[K][N], hls::stream<data_t> &B_nxt)
{
    Load_B_LOOP:
	for(int k=0; k<K; k++){
#pragma HLS PIPELINE
		B_nxt << B[k][n];
	}

}

void Drain_A(hls::stream<data_t> &A_pre)
{
	for(int k=0; k<K; k++){
#pragma HLS PIPELINE
		data_t drain;
		A_pre >> drain;
	}

}

void Drain_B(hls::stream<data_t> &B_pre)
{
	for(int k=0; k<K; k++){
#pragma HLS PIPELINE
		data_t drain;
		B_pre >> drain;
	}

}

void PE_Array(data_t A[M][K], data_t B[K][N], ap_int<32> C[M][N], bool reset_C)
{
	hls::stream<data_t> A_inter[M][N+1];
#pragma HLS STREAM variable=A_inter
	hls::stream<data_t> B_inter[M+1][N];
#pragma HLS STREAM variable=B_inter

#pragma HLS DATAFLOW

    Load_A_LOOP:
	for(int m=0; m<M; m++){
#pragma HLS UNROLL
		Load_A(m, A, A_inter[m][0]);
	}
    Load_B_LOOP:
	for(int n=0; n<N; n++){
#pragma HLS UNROLL
		Load_B(n, B, B_inter[0][n]);
	}

	PE_ROW:
	for(int m=0; m<M; m++){
#pragma HLS UNROLL
		PE_COL:
		for(int n=0; n<N; n++){
#pragma HLS UNROLL
			PE(A_inter[m][n], B_inter[m][n], A_inter[m][n+1], B_inter[m+1][n], C[m][n], reset_C);
		}
	}

	// Drain A
    Drain_A_LOOP:
	for(int m=0; m<M; m++){
#pragma HLS UNROLL
		Drain_A(A_inter[m][N]);
	}

	// Drain B
    Drain_B_LOOP:
	for(int n=0; n<N; n++){
#pragma HLS UNROLL
		Drain_B(B_inter[M][n]);
	}

}

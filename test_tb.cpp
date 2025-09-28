#include "gemm.h"
#include <iostream>

using namespace std;

#define ROW_A  40
#define COL_A  9
#define COL_B  20

#define DIN_A_SIZE  (ROW_A*COL_A)
#define DIN_B_SIZE  (COL_A*COL_B)
#define DOUT_SIZE   (ROW_A*COL_B)

data_t din_a[DIN_A_SIZE], din_b[DIN_B_SIZE];
ap_int<16> dout[DOUT_SIZE];

// GEMM_Tiling_Test
void GEMM_Tiling_test()
{
  for(int i = 0; i < DIN_A_SIZE; i++){
    din_a[i] = (data_t)(1);
  }

  cout << "din_a:\n";
  for(int r = 0; r < ROW_A; r++)
  {
    for(int c = 0; c < COL_A; c++)
      cout << din_a[r * COL_A + c] << "\t";
    cout << "\n";
  }

  for(int i = 0; i < DIN_B_SIZE; i++){
    din_b[i] = (data_t)(1);
  }

  cout << "din_b:\n";
  for(int r = 0; r < COL_A; r++)
  {
    for(int c = 0; c < COL_B; c++)
      cout << din_b[r * COL_B + c] << "\t";
    cout << "\n";
  }

  ap_int<16> ref[DOUT_SIZE];

    for(int r1 = 0; r1 < ROW_A; r1++)
      for (int c2 = 0; c2 < COL_B; c2++)
      {
    	int temp = 0;

        for(int r2 = 0; r2 < COL_A; r2++)
          temp += din_a[r1 * COL_A + r2] * din_b[r2 * COL_B + c2];

        ref[r1 * COL_B + c2] = temp;
      }

    GEMM_Tiling(din_a, din_b, dout, ROW_A, COL_A, COL_B);

  cout << "Reference:\n";
  for(int r = 0; r < ROW_A; r++)
  {
    for(int c = 0; c < COL_B; c++)
      cout << ref[r * COL_B + c] << "\t";
    cout << "\n";
  }
  
   cout << "\nDout:\n";
   for(int r = 0; r < ROW_A; r++)
   {
     for(int c = 0; c < COL_B; c++)
       cout << dout[r * COL_B + c] << "\t";
     cout << "\n";
   }

  int num_errors = 0;

  for(int i = 0; i < DOUT_SIZE; i++)
  {
    if(dout[i] != ref[i])
      num_errors++;
  }

  cout << "Number of errors: " << num_errors << "\n";
}


int main(){
	
  GEMM_Tiling_test();

  return 0;
}

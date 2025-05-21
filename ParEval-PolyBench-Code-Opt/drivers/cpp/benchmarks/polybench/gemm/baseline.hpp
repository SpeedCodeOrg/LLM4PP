#include <vector>

void correct_kernel_gemm(int ni, int nj, int nk,
		 	 double alpha,
		 	 double beta,
		 	 std::vector<std::vector<double>>& A,
		 	 std::vector<std::vector<double>>& B,
		 	 std::vector<std::vector<double>>& C) {

    // => Form C := alpha*A*B + beta*C,
    //A is NIxNK
    //B is NKxNJ
    //C is NIxNJ
    for (int i = 0; i < ni; i++) {
	for (int j = 0; j < nj; j++) {
	    C[i][j] *= beta;
	}
	for (int k = 0; k < nk; k++) {
	    for (int j = 0; j < nj; j++) {
		C[i][j] += alpha * A[i][k] * B[k][j];
	    }
	}
    }
}


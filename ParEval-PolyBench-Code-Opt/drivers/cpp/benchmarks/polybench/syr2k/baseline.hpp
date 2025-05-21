#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_syr2k(int n, int m,
		  	  double alpha,
		  	  double beta,
		  	  std::vector<std::vector<double>>& C,
		  	  std::vector<std::vector<double>>& A,
		  	  std::vector<std::vector<double>>& B) {
    //BLAS PARAMS
    //UPLO  = 'L'
    //TRANS = 'N'
    //A is NxM
    //B is NxM
    //C is NxN
    for (int i = 0; i < n; i++) {
	for (int j = 0; j <= i; j++) {
	    C[i][j] *= beta;
	}

	for (int k = 0; k < m; k++) {
	    for (int j = 0; j <= i; j++) {
		C[i][j] += A[j][k]*alpha*B[i][k] + B[j][k]*alpha*A[i][k];
	    }
	}
    }
}


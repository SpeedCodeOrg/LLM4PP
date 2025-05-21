#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_syrk(int n, int m,
		 	 double alpha,
		 	 double beta,
		 	 std::vector<std::vector<double>>& C,
		 	 std::vector<std::vector<double>>& A) {

    // =>  Form  C := alpha*A*A**T + beta*C.
    //A is NxM
    //C is NxN
    for (int i = 0; i < n; i++) {
	for (int j = 0; j <= i; j++) {
	    C[i][j] *= beta;
	}
	for (int k = 0; k < m; k++) {
	    for (int j = 0; j <= i; j++) {
		C[i][j] += alpha * A[i][k] * A[j][k];
	    }
	}
    }
}


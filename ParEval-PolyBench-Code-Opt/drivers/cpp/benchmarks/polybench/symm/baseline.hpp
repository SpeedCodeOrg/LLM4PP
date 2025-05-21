
#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_symm(int m, int n,
		 	 double alpha,
		 	 double beta,
		 	 std::vector<std::vector<double>>& C,
		 	 std::vector<std::vector<double>>& A,
		 	 std::vector<std::vector<double>>& B) {

    // =>  Form  C := alpha*A*B + beta*C
    // A is MxM
    // B is MxN
    // C is MxN
    //note that due to Fortran array layout, the code below more closely resembles upper triangular case in BLAS
    for (int i = 0; i < m; i++) {
	for (int j = 0; j < n; j++) {
	    double temp2 = 0;
	    for (int k = 0; k < i; k++) {
		C[k][j] += alpha * B[i][j] * A[i][k];
		temp2 += B[k][j] * A[i][k];
	    }
	    C[i][j] = beta * C[i][j] + alpha*B[i][j] * A[i][i] + alpha * temp2;
	}
    }
}


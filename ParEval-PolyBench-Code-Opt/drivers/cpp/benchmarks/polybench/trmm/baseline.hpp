#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_trmm(int m, int n,
		 	 double alpha,
		 	 std::vector<std::vector<double>>& A,
		 	 std::vector<std::vector<double>>& B) {

    // => Form  B := alpha*A**T*B.
    // A is MxM
    // B is MxN

    for (int i = 0; i < m; i++) {
	for (int j = 0; j < n; j++) {
	    for (int k = i + 1; k < m; k++) {
		B[i][j] += A[k][i] * B[k][j];
	    }
	    B[i][j] = alpha * B[i][j];
	}
    }
}


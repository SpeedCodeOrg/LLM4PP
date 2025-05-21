#include <cmath>
#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_cholesky(int n,
		     	     std::vector<std::vector<double>>& A) {
    for (int i = 0; i < n; i++) {
	for (int j = 0; j < i; j++) {
	    for (int k = 0; k < j; k++) {
		A[i][j] -= A[i][k] * A[j][k];
	    }
	    A[i][j] /= A[j][j];
	}
	// i==j case
	for (int k = 0; k < i; k++) {
	    A[i][i] -= A[i][k] * A[i][k];
	}

	A[i][i] = std::sqrt(A[i][i]);
    }
}


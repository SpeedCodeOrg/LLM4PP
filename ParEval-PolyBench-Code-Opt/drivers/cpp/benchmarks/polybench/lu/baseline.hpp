#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_lu(int n,
	       	       std::vector<std::vector<double>>& A) {
    for (int i = 0; i < n; i++) {
	for (int j = 0; j < i; j++) {
	    for(int k = 0; k < j; k++) {
		A[i][j] -= A[i][k] * A[k][j];
	    }
	    A[i][j] /= A[j][j];
	}

	for (int j = i; j < n; j++) {
	    for (int k = 0; k < i; k++) {
		A[i][j] -= A[i][k] * A[k][j];
	    }
	}
    }
}


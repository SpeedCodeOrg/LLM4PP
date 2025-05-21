#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_jacobi_2d(int tsteps,
			      int n,
		              std::vector<std::vector<double>>& A,
		      	      std::vector<std::vector<double>>& B) {

    for (int t = 0; t < tsteps; t++) {
	for (int i = 1; i < n - 1; i++) {
	    for (int j = 1; j < n - 1; j++) {
		B[i][j] = 0.2 * (A[i][j] + A[i][j-1] + A[i][1+j] + A[1+i][j] + A[i-1][j]);
	    }
	}
	for (int i = 1; i < n - 1; i++) {
	    for (int j = 1; j < n - 1; j++) {
		A[i][j] = 0.2 * (B[i][j] + B[i][j-1] + B[i][1+j] + B[1+i][j] + B[i-1][j]);
	    }
	}
    }
}


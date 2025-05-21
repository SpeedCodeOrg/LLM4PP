#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_jacobi_1d(int tsteps,
		      	      int n,
			      std::vector<double>& A,
			      std::vector<double>& B) {

    for (int t = 0; t < tsteps; t++) {
	for (int i = 1; i < n - 1; i++) {
	    B[i] = 0.33333 * (A[i-1] + A[i] + A[i + 1]);
	}
	for (int i = 1; i < n - 1; i++) {
	    A[i] = 0.33333 * (B[i-1] + B[i] + B[i + 1]);
	}
    }
}


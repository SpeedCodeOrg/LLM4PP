#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_seidel_2d(int tsteps,
		      	      int n,
		       	      std::vector<std::vector<double>>& A) {
    for (int t = 0; t <= tsteps - 1; t++) {
	for (int i = 1; i<= n - 2; i++) {
	    for (int j = 1; j <= n - 2; j++) {
		A[i][j] = (A[i-1][j-1] + A[i-1][j] + A[i-1][j+1] + A[i][j-1] + A[i][j] + A[i][j+1] + A[i+1][j-1] + A[i+1][j] + A[i+1][j+1]) / 9.0;
	    }
	 }
    }
}


#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_atax(int m, int n,
		 	 std::vector<std::vector<double>>& A,
		 	 std::vector<double>& x,
		 	 std::vector<double>& y,
			 std::vector<double>& tmp) {
    for (int i = 0; i < n; i++) {
	y[i] = 0.0;
    }

    for (int i = 0; i < m; i++) {
	tmp[i] = 0.0;
	for (int j = 0; j < n; j++) {
	    tmp[i] = tmp[i] + A[i][j] * x[j];
	}

	for (int j = 0; j < n; j++) {
	    y[j] = y[j] + A[i][j] * tmp[i];
	}
    }
}


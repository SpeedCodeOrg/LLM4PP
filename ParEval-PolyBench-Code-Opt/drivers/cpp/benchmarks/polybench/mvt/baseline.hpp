#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_mvt(int n,
			std::vector<double>& x1,
			std::vector<double>& x2,
			std::vector<double>& y_1,
			std::vector<double>& y_2,
			std::vector<std::vector<double>>& A) {

    for (int i = 0; i < n; i++) {
	for (int j = 0; j < n; j++) {
	    x1[i] = x1[i] + A[i][j] * y_1[j];
	}
    }
    for (int i = 0; i < n; i++) {
	for (int j = 0; j < n; j++) {
	    x2[i] = x2[i] + A[j][i] * y_2[j];
	}
    }
}


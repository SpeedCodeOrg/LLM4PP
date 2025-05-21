#include <cmath>
#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
/* QR Decomposition with Modified Gram Schmidt:
 http://www.inf.ethz.ch/personal/gander/ */
void correct_kernel_gramschmidt(int m, int n,
				std::vector<std::vector<double>>& A,
				std::vector<std::vector<double>>& R,
				std::vector<std::vector<double>>& Q) {
    for (int k = 0; k < n; k++) {
	double nrm = 0.0;
	for (int i = 0; i < m; i++) {
	    nrm += A[i][k] * A[i][k];
	}
	R[k][k] = std::sqrt(nrm);
	for (int i = 0; i < m; i++) {
	    Q[i][k] = A[i][k] / R[k][k];
	}
	for (int j = k + 1; j < n; j++) {
	    R[k][j] = 0.0;
	    for (int i = 0; i < m; i++) {
		R[k][j] += Q[i][k] * A[i][j];
	    }
	    for (int i = 0; i < m; i++) {
		A[i][j] = A[i][j] - Q[i][k] * R[k][j];
	    }
	}
    }
}

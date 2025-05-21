#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_2mm(int ni, int nj, int nk, int nl,
			double alpha,
			double beta,
			std::vector<std::vector<double>>& tmp,
			std::vector<std::vector<double>>& A,
			std::vector<std::vector<double>>& B,
			std::vector<std::vector<double>>& C,
			std::vector<std::vector<double>>& D) {

    for (int i = 0; i < ni; i++) {
	for (int j = 0; j < nj; j++) {
	    tmp[i][j] = 0.0;
	    for (int k = 0; k < nk; k++) {
		tmp[i][j] += alpha * A[i][k] * B[k][j];
	    }
	}
    }

    for (int i = 0; i < ni; i++) {
	for (int j = 0; j < nl; j++) {
	    D[i][j] *= beta;
	    for (int k = 0; k < nj; k++) {
		D[i][j] += tmp[i][k] * C[k][j];
	    }
	}
    }
}


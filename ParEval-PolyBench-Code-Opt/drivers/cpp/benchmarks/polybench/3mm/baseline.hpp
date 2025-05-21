#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_3mm(int ni, int nj, int nk, int nl, int nm,
			std::vector<std::vector<double>>& E,
			std::vector<std::vector<double>>& A,
			std::vector<std::vector<double>>& B,
			std::vector<std::vector<double>>& F,
			std::vector<std::vector<double>>& C,
			std::vector<std::vector<double>>& D,
			std::vector<std::vector<double>>& G) {

    for (int i = 0; i < ni; i++) {
	for (int j = 0; j < nj; j++) {
	    E[i][j] = 0.0;
	    for (int k = 0; k < nk; k++) {
		E[i][j] += A[i][k] * B[k][j];
	    }
	}
    }

    for (int i = 0; i < nj; i++) {
	for (int j = 0; j < nl; j++) {
	    F[i][j] = 0.0;
	    for (int k = 0; k < nm; k++) {
		F[i][j] += C[i][k]  * D[k][j];
	    }
	}
    }

    for (int i = 0; i < ni; i++) {
	for (int j = 0; j < nl; j++) {
	    G[i][j] = 0.0;
	    for (int k = 0; k < nj; k++) {
		G[i][j] += E[i][k] * F[k][j];
	    }
	}
    }
}


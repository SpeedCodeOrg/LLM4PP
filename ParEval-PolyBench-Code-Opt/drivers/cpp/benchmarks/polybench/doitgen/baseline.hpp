#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_doitgen(int nr, int nq, int np,
		            std::vector<std::vector<std::vector<double>>>& A,
		            std::vector<std::vector<double>>& C4,
			    std::vector<double>& sum) {

    for (int r = 0; r < nr; r++) {
	for (int q = 0; q < nq; q++) {
	    for (int p = 0; p < np; p++) {
		sum[p] = 0.0;
		for (int s = 0; s < np; s++) {
		    sum[p] += A[r][q][s] * C4[s][p];
		}
	    }

	    for (int p = 0; p < np; p++) {
		A[r][q][p] = sum[p];
	    }
	}
    }
}


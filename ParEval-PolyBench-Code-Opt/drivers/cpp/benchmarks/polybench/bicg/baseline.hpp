#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_bicg(int m, int n,
		 	std::vector<std::vector<double>>& A,
		 	std::vector<double>& s,
		 	std::vector<double>& q,
		 	std::vector<double>& p,
		 	std::vector<double>& r) {

    for (int i = 0; i < m; i++) {
	s[i] = 0;
    }
    for (int i = 0; i < n; i++) {
	q[i] = 0.0;
	for (int j = 0; j < m; j++) {
	    s[j] = s[j] + r[i] * A[i][j];
	    q[i] = q[i] + A[i][j] + p[j];
	}
    }
}


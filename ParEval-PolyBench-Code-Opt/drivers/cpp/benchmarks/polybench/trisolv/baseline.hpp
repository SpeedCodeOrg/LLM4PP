#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_trisolv(int n,
		    	    std::vector<std::vector<double>>& L,
		    	    std::vector<double>& x,
		    	    std::vector<double>& b) {

    for (int i = 0; i < n; i++) {
	x[i] = b[i];
	for (int j = 0; j < i; j++) {
	    x[i] -= L[i][j] * x[j];
	}
	x[i] = x[i] / L[i][i];
    }
}


#include <vector>
    

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_gesummv(int n,
		    	    double alpha,
		    	    double beta,
		    	    std::vector<std::vector<double>>& A,
		    	    std::vector<std::vector<double>>& B,
		    	    std::vector<double>& tmp,
		    	    std::vector<double>& x,
		    	    std::vector<double>& y) {

    for (int i = 0; i < n; i++) {
	tmp[i] = 0.0;
	y[i] = 0.0;
	for (int j = 0; j < n; j++) {
	    tmp[i] = A[i][j] * x[j] + tmp[i];
	    y[i] = B[i][j] * x[j] + y[i];
	}
	
	y[i] = alpha * tmp[i] + beta * y[i];
    }
}


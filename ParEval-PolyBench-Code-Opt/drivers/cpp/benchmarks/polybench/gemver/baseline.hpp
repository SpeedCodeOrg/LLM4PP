#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_gemver(int n,
		   	   double alpha,
		   	   double beta,
		   	   std::vector<std::vector<double>>& A,
		   	   std::vector<double>& u1,
		   	   std::vector<double>& v1,
		   	   std::vector<double>& u2,
		   	   std::vector<double>& v2,
		   	   std::vector<double>& w,
		   	   std::vector<double>& x,
		   	   std::vector<double>& y,
		   	   std::vector<double>& z) {
	
    for (int i = 0; i < n; i++) {
	for (int j = 0; j < n; j++) {
	    A[i][j] = A[i][j] + u1[i] * v1[j] + u2[i] * v2[j];
	}
    }

    for (int i = 0; i < n; i++) {
	for (int j = 0; j < n; j++) {
	    x[i] = x[i] + beta * A[j][i] * y[j];
	}
    }

    for (int i = 0; i < n; i++) {
	x[i] = x[i] + z[i];
    }

    for (int i = 0; i < n; i++) {
	for (int j = 0; j < n; j++) {
	    w[i] = w[i] +  alpha * A[i][j] * x[j];
	}
    }
}


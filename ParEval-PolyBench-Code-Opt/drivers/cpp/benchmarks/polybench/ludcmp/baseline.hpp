#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_ludcmp(int n,
		   	   std::vector<std::vector<double>>& A,
		   	   std::vector<double>& b,
		   	   std::vector<double>& x,
		   	   std::vector<double>& y) {

    for (int i = 0; i < n; i++) {
	for (int j = 0; j < i; j++) {
	    double w = A[i][j];
	    for (int k = 0; k < j; k++) {
		w -= A[i][k] * A[k][j];
	    }
	    A[i][j] = w / A[j][j];
	}

	for (int j = i; j < n; j++) {
	    double w = A[i][j];
	    for (int k = 0; k < i; k++) {
		w -= A[i][k] * A[k][j];
	    }

	    A[i][j] = w;
	}
    }

    for (int i = 0; i < n; i++) {
	double w = b[i];
	for (int j = 0; j < i; j++) {
	    w -= A[i][j] * y[j];
	}
	y[i] = w;
    }

    for (int i = n - 1; i >= 0; i--) {
	double w = y[i];
	for (int j = i + 1; j < n; j++) {
	    w -= A[i][j] * x[j];
	}
	x[i] = w / A[i][i];
    }
}


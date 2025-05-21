#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_durbin(int n,
		   	   std::vector<double>& r,
		   	   std::vector<double>& y) {
    std::vector<double> z(n, 0.0);
    y[0] = -r[0];
    double beta = 1.0;
    double alpha = -r[0];

    for (int k = 1; k < n; k++) {
	beta = (1 - alpha * alpha) * beta;
	double sum = 0.0;
	for (int i = 0; i < k; i++) {
	    sum += r[k - i - 1] * y[i];
	}

	alpha = -(r[k] + sum) / beta;

	for (int i = 0; i < k; i++) {
	    z[i] = y[i] + alpha * y[k - i - 1];
	}

	for (int i = 0; i < k; i++) {
	    y[i] = z[i];
	}

	y[k] = alpha;
    }
}


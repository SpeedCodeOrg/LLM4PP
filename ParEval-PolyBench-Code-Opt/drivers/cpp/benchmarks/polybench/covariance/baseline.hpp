#include <vector>

void correct_kernel_covariance(int m, int n, 
			       std::vector<std::vector<double>>& data,
		       	       std::vector<std::vector<double>>& cov,
			       std::vector<double>& mean) {

    for (int j = 0; j < m; j++) {
	mean[j] = 0.0;
        for (int i = 0; i < n; i++) {
            mean[j] += data[i][j];
        }
        mean[j] /= n;
    }

    for (int i = 0; i < n; i++) {
	for (int j = 0; j < m; j++) {
	    data[i][j] -= mean[j];
	}
    }

    for (int i = 0; i < m; i++) {
	for (int j = i; j < m; j++) {
	    cov[i][j] = 0.0;
	    for (int k = 0; k < n; k++) {
		cov[i][j] += data[k][i] * data[k][j];
	    }
	    cov[i][j] /= (n - 1.0);
	    cov[j][i] = cov[i][j];
	}
    }
}


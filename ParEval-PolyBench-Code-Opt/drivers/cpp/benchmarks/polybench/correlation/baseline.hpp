#include <numeric>
#include <vector>

void correct_kernel_correlation(int m, int n,
				std::vector<std::vector<double>>& data,
				std::vector<std::vector<double>>& corr,
				std::vector<double>& mean,
				std::vector<double>& stddev) {
    double eps = 0.1;

    for (int j = 0; j < m; j++) {
        mean[j] = 0.0;
        for (int i = 0; i < n; i++) {
            mean[j] += data[i][j];
        }
        mean[j] /= n;
    }

    for (int j = 0; j < m; j++) {
        stddev[j] = 0.0;
        for (int i = 0; i < n; i++) {
            stddev[j] += (data[i][j] - mean[j]) * (data[i][j] - mean[j]);
        }
        stddev[j] /= n;
        stddev[j] = std::sqrt(stddev[j]);
        /* The following in an inelegant but usual way to handle
        near-zero std. dev. values, which below would cause a zero-
        divide. */
        stddev[j] = stddev[j] <= eps ? 1.0 : stddev[j];
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            data[i][j] -= mean[j];
            data[i][j] /= std::sqrt(n) * stddev[j];
        }
    }

    for (int i = 0; i < m - 1; i++) {
        corr[i][i] = 1.0;
        for (int j = i + 1; j < m; j++) {
            corr[i][j] = 0.0;
            for (int k = 0; k < n; k++) {
                corr[i][j] += (data[k][i] * data[k][j]);
            }
            corr[j][i] = corr[i][j];
        }
    }

    corr[m-1][m-1] = 1.0;
}


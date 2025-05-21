#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
/* Based on a Fortran code fragment from Figure 5 of
 * "Automatic Data and Computation Decomposition on Distributed Memory Parallel Computers"
 * by Peizong Lee and Zvi Meir Kedem, TOPLAS, 2002
 */
void correct_kernel_adi(int tsteps, int n,
			std::vector<std::vector<double>>& u,
			std::vector<std::vector<double>>& v,
			std::vector<std::vector<double>>& p,
			std::vector<std::vector<double>>& q) {
    double DX = 1.0 / n;
    double DY = 1.0 / n;
    double DT = 1.0 / tsteps;
    double B1 = 2.0;
    double B2 = 1.0;
    double mul1 = B1 * DT / (DX * DX);
    double mul2 = B2 * DT / (DY * DY);
    double a = -mul1 / 2.0;
    double b = 1.0 + mul1;
    double c = a;
    double d = -mul2 / 2.0;
    double e = 1.0 + mul2;
    double f = d;

    for (int t = 1; t <= tsteps; t++) {
	for (int i = 1; i < n - 1; i++) {
	    v[0][i] = 1.0;
	    p[i][0] = 0.0;
	    q[i][0] = v[0][i];

	    for (int j = 1; j < n - 1; j++) {
		p[i][j] = -c / (a * p[i][j-1] + b);
		q[i][j] = (-d * u[j][i - 1] + (1.0 + 2.0 * d) * u[j][i] - f * u[j][i + 1] - a * q[i][j - 1]) / (a * p[i][j - 1] + b);
	    }

	    v[n - 1][i] = 1.0;

	    for (int j = n - 2; j >= 1; j--) {
		v[j][i] = p[i][j] * v[j + 1][i] + q[i][j];
	    }
	}

	for (int i = 1; i < n - 1; i++) {
	    u[i][0] = 1.0;
	    p[i][0] = 0.0;
	    q[i][0] = u[i][0];

	    for (int j = 1; j < n - 1; j++) {
		p[i][j] = -f / (d * p[i][j - 1] + e);
		q[i][j] = (-a * v[i-1][j] + (1.0 + 2.0 * a) * v[i][j] - c * v[i + 1][j] - d * q[i][j - 1]) / (d * p[i][j - 1] + e);
	    }

	    u[i][n - 1] = 1.0;
	    for (int j = n - 2; j >= 1; j--) {
		u[i][j] = p[i][j] * u[i][j + 1] + q[i][j];
	    }
	}
    }
}


#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
void correct_kernel_heat_3d(int tsteps,
		    	    int n,
		    	    std::vector<std::vector<std::vector<double>>>& A,
		    	    std::vector<std::vector<std::vector<double>>>& B) {

    for (int t = 1; t <= tsteps; t++) {
        for (int i = 1; i < n - 1; i++) {
            for (int j = 1; j < n - 1; j++) {
                for (int k = 1; k < n - 1; k++) {
                    B[i][j][k] = 0.125 * (A[i+1][j][k] - 2.0 * A[i][j][k] + A[i-1][j][k]) + 0.125 * (A[i][j+1][k] - 2.0 * A[i][j][k] + A[i][j-1][k]) + 0.125 * (A[i][j][k+1] - 2.0 * A[i][j][k] + A[i][j][k-1]) + A[i][j][k];
                }
            }
        }

        for (int i = 1; i < n - 1; i++) {
           for (int j = 1; j < n - 1; j++) {
               for (int k = 1; k < n - 1; k++) {
                   A[i][j][k] = 0.125 * (B[i+1][j][k] - 2.0 * B[i][j][k] + B[i-1][j][k]) + 0.125 * (B[i][j+1][k] - 2.0 * B[i][j][k] + B[i][j-1][k]) + 0.125 * (B[i][j][k+1] - 2.0 * B[i][j][k] + B[i][j][k-1]) + B[i][j][k];
               }
           }
       }
    }
}


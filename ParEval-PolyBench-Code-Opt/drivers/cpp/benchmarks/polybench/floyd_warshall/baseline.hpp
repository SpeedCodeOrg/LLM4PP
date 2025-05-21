#include <vector>

void correct_kernel_floyd_warshall(int n, std::vector<std::vector<int>>& path) {
    for (int k = 0; k < n; k++) {
	for (int i = 0; i < n; i++) {
	    for (int j = 0; j < n; j++) {
		path[i][j] = std::min<int>(path[i][j], path[i][k] + path[k][j]);
	    }
	}
    }
}


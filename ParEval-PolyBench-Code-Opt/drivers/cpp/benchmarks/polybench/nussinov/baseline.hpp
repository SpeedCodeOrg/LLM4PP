#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
/*
  Original version by Dave Wonnacott at Haverford College <davew@cs.haverford.edu>,
  with help from Allison Lake, Ting Zhou, and Tian Jin,
  based on algorithm by Nussinov, described in Allison Lake's senior thesis.
*/

void correct_kernel_nussinov(int n, std::vector<int>& seq,
		     	     std::vector<std::vector<int>>& table) {

    auto match = [](int b1, int b2) {
	return b1 + b2 == 3 ? 1 : 0;
    };

    for (int i = n - 1; i >= 0; i--) {
	for (int j = i + 1; j < n; j++) {
	    if (j - 1 >= 0) {
		table[i][j] = std::max<int>(table[i][j], table[i][j-1]);
	    }
	    if (i + 1 < n) {
	      table[i][j] = std::max<int>(table[i][j], table[i+1][j]);
	    }

	    if (j - 1 >= 0 && i + 1 < n) {
		if (i<j-1) {
		    table[i][j] = std::max<int>(table[i][j], table[i+1][j-1] + match(seq[i], seq[j]));
		} else {
		    table[i][j] = std::max<int>(table[i][j], table[i+1][j-1]);
		}
	    }

	    for (int k = i + 1; k < j; k++) {
		table[i][j] = std::max<int>(table[i][j], table[i][k] + table[k+1][j]);
	    }
	}
    }
}


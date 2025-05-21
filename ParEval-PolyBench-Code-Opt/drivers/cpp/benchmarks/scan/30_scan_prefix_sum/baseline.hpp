#pragma once

#include <numeric>
#include <vector>

/* Compute the prefix sum of the vector x into output.
   Example:

   input: [1, 7, 4, 6, 6, 2]
   output: [1, 8, 12, 18, 24, 26]
*/
void NO_INLINE correctPrefixSum(std::vector<int64_t> const& x, std::vector<int64_t> &output) {
    std::inclusive_scan(x.begin(), x.end(), output.begin());
}

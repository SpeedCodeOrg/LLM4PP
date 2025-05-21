#pragma once
#include <vector>
#include <algorithm>

/* Find the set of points that defined the smallest convex polygon that contains all the points in the vector points. Store the result in `hull`.
   Example:

   input: [{0, 3}, {1, 1}, {2, 2}, {4, 4}, {0, 0}, {1, 2}, {3, 1}, {3, 3}]
   output: [{0, 3}, {4, 4}, {3, 1}, {0, 0}]
*/
void NO_INLINE correctConvexHull(std::vector<Point> const& points, std::vector<Point> &hull) {
    // The polygon needs to have at least three points
    if (points.size() < 3)   {
        hull = points;
        return;
    }

    std::vector<Point> pointsSorted = points;

    std::sort(pointsSorted.begin(), pointsSorted.end(), [](Point const& a, Point const& b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    });

    /*
    auto CrossProduct = [](Point const& a, Point const& b, Point const& c) {
        return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x) > 0;
    };
    */

    auto CrossProduct = [](Point const& a, Point const& b, Point const& c) {
        return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    };

    std::vector<Point> ans(2 * pointsSorted.size());

    int n = points.size();

    int k = 0;
    // Build lower hull
    for (int i = 0; i < n; ++i) {
        // If the point at K-1 position is not a part
        // of hull as vector from ans[k-2] to ans[k-1]
        // and ans[k-2] to pointsSorted[i] has a clockwise turn
        while (k >= 2 && CrossProduct(ans[k - 2], ans[k - 1], pointsSorted[i]) <= 0) {
            k--;
	}
        ans[k++] = pointsSorted[i];
    }

    // Build upper hull
    for (int i = n - 1, t = k + 1; i > 0; --i) {
        // If the point at K-1 position is not a part
        // of hull as vector from ans[k-2] to ans[k-1]
        // and ans[k-2] to pointsSorted[i] has a clockwise turn
        while (k >= t && CrossProduct(ans[k - 2], ans[k - 1], pointsSorted[i - 1]) <= 0) {
            k--;
	}
        ans[k++] = pointsSorted[i - 1];
    }

    // Resize the array to desired size
    ans.resize(k - 1);

    hull = ans;
    return;

    /*
    std::vector<Point> upperHull;
    std::vector<Point> lowerHull;
    upperHull.push_back(pointsSorted[0]);
    upperHull.push_back(pointsSorted[1]);

    for (size_t i = 2; i < pointsSorted.size(); i++) {
        while (upperHull.size() > 1
               && !CrossProduct(upperHull[upperHull.size() - 2],
                                upperHull[upperHull.size() - 1],
                                pointsSorted[i])) {
            upperHull.pop_back();
        }
        upperHull.push_back(pointsSorted[i]);

        while (lowerHull.size() > 1
               && !CrossProduct(lowerHull[lowerHull.size() - 2],
                                lowerHull[lowerHull.size() - 1],
                                pointsSorted[pointsSorted.size() - i - 1])) {
            lowerHull.pop_back();
        }
        lowerHull.push_back(pointsSorted[pointsSorted.size() - i - 1]);
    }
    upperHull.insert(upperHull.end(), lowerHull.begin(), lowerHull.end());

    hull = upperHull;
    return;
    */
}

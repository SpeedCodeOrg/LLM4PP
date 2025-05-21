#pragma once
#include <vector>
#include <algorithm>
#include <cmath>

/* Return the perimeter of the smallest convex polygon that contains all the points in the vector points.
   Example:

   input: [{0, 3}, {1, 1}, {2, 2}, {4, 4}, {0, 0}, {1, 2}, {3, 1}, {3, 3}]
   output: 13.4477
*/
double NO_INLINE correctConvexHullPerimeter(std::vector<Point> const& points) {
    // The polygon needs to have at least three points
    if (points.size() < 3)   {
        return 0;
    }

    std::vector<Point> pointsSorted = points;

    std::sort(pointsSorted.begin(), pointsSorted.end(), [](Point const& a, Point const& b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    });

    /*
    auto CrossProduct = [](Point const& a, Point const& b, Point const& c) {
        return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
    };
    */

    auto CrossProduct = [](Point const& a, Point const& b, Point const& c) {
        return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    };

    auto dist = [](Point const& p1, Point const& p2) {
        return sqrt(pow(p2.x-p1.x, 2) + pow(p2.y-p1.y, 2));
    };

    // Implementation taken from https://www.geeksforgeeks.org/perimeter-of-convex-hull-for-a-given-set-of-points/
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

    double perimeter = 0.0;

    // Find the distance between adjacent points
    for (int i = 0; i < ans.size() - 1; i++) {
        perimeter += dist(ans[i], ans[i + 1]);
    }

    // Add the distance between first and last point
    perimeter += dist(ans[0], ans[ans.size() - 1]);

    return perimeter;

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

    double perimeter = 0;
    for (size_t i = 0; i < upperHull.size() - 1; i++) {
        perimeter += dist(upperHull[i], upperHull[i+1]);
    }
    perimeter += dist(upperHull[0], upperHull[upperHull.size() - 1]);

    return perimeter;
    */
}

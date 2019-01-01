#include <vector>

using std::vector;

extern "C" {
    long long calculatesum(const vector<double>& vec) {
        long long sum = 0;
        for (auto x : vec) {
            sum += x;
        }
        return sum;
    }
}

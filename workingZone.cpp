#include <algorithm>
#include <vector>
#include <iterator>
using namespace std;
// ✅ 高效清晰的top-k API
vector<int> top_k_sorted(const vector<int> &input, size_t k)
{
    if (input.empty() || k == 0)
        return {};

    k = min(k, input.size());
    vector<int> result;
    result.reserve(k);

    // 使用部分排序拷贝：O(n log k) 时间复杂度，只需一次拷贝
    partial_sort_copy(input.begin(), input.end(),
                      result.begin(), result.end(),
                      greater<int>());

    return result;
}
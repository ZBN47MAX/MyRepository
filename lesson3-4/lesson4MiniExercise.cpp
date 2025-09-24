///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A OG code
#include <iostream>
#include <vector>
using namespace std;

int main()
{
    vector<int> maths = {70, 60, 80};
    vector<int> physics = {10, -2, 7};

    int sum1 = 0;
    for (int s : maths)
        sum1 += s;
    double avg1 = sum1 / 3.0;
    cout << "Average: " << avg1 << (avg1 >= 50 ? " PASS" : " FAIL") << "\n";

    int sum2 = 0;
    for (int s : physics)
        sum2 += s;
    double avg2 = sum2 / 3.0;
    cout << "Average: " << avg2 << (avg2 >= 50 ? " PASS" : " FAIL") << "\n";
}
// A1 The code repeats the same idea twice. Refactor into compute_average and print_report with clear names and types.
#include <iostream>
#include <vector>
using namespace std;

int compute_average(double &avg1, double &avg2)
{
    vector<int> maths = {70, 60, 80};
    vector<int> physics = {10, -2, 7};

    int sum1 = 0;
    for (int s : maths)
        sum1 += s;
    avg1 = sum1 / 3.0;

    int sum2 = 0;
    for (int s : physics)
        sum2 += s;
    avg2 = sum2 / 3.0;

    return 0;
}

int print_report(double avg1, double avg2)
{
    cout << "Average: " << avg1 << (avg1 >= 50 ? " PASS" : " FAIL") << "\n";
    cout << "Average: " << avg2 << (avg2 >= 50 ? " PASS" : " FAIL") << "\n";
    return 0;
}
int main()
{
    double a1, a2;
    compute_average(a1, a2);
    print_report(a1, a2);
    return 0;
}
// A2 Propose function signatures (parameters as const & where appropriate; return a value, not print).
//????得问嗷
#include <iostream>
#include <vector>
#include <string>
using namespace std;

string compute_average(const vector<int> &scores)
{

    int sum = 0;
    for (int score : scores)
    {
        sum += score;
    }
    double average = static_cast<double>(sum) / scores.size();
    return "Average: " + to_string(average) + (average >= 50 ? " PASS" : " FAIL");
}

int main()
{
    vector<int> maths = {70, 60, 80};
    vector<int> physics = {10, -2, 7};

    string mathsReport = compute_average(maths);
    string physicsReport = compute_average(physics);

    cout << mathsReport << endl;
    cout << physicsReport << endl;

    return 0;
}

// A3 Ensure functions do one task each (compute vs print).
#include <iostream>
#include <vector>
#include <string>
using namespace std;

// 计算平均分
double compute_average(const vector<int> &scores)
{

    int sum = 0;
    for (int score : scores)
    {
        sum += score;
    }
    return static_cast<double>(sum) / scores.size();
}

// 是否通过
bool isPassed(double average, double passingScore = 50.0)
{
    return average >= passingScore;
}

// 创建报告
string make_report(double average, bool passed)
{
    return "Average:" + to_string(average) + (passed ? "PASS" : "FAIL");
}

// 输出报告
void output_report(const string &report)
{
    cout << report << endl;
}

// 主接入口
int main()
{
    // 两套参数
    vector<int> maths = {70, 60, 80};
    vector<int> physics = {10, -2, 7};

    // 双精度浮点数变量mathAvg被赋值为 将向量maths通过compute-avg函数之后得出的参数
    double mathsAvg = compute_average(maths);
    // 布尔变量mathPassed被赋值为 mathsAvg通过isPassed判断后的参数
    bool mathPassed = isPassed(mathsAvg);
    // 字符串mathReport被赋值为 manthAvg和mathPassed两个变量进入make-report函数后得出的字符串数据
    string mathReport = make_report(mathsAvg, mathPassed);
    // 将字符串mathReport进入到output-report函数中
    output_report(mathReport);

    double physicsAvg = compute_average(physics);
    bool physicsPassed = isPassed(physicsAvg);
    string physicsReport = make_report(physicsAvg, physicsPassed);
    output_report(physicsReport);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// B OG Code
#include <algorithm>
#include <vector>
// ❌ Copies input twice and sorts whole vector unnecessarily
std::vector<int> top_k_sorted(std::vector<int> data, std::size_t k)
{
    std::sort(data.begin(), data.end()); // O(n log n)
    if (k > data.size())
        k = data.size();
    return std::vector<int>(data.end() - k, data.end());
}

// B1 Choose an API for “top-k sorted” that is clear and efficient. Fix the version below.
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
// B2 Refactor to take input by const std::vector<int>& and avoid full sort (hint: std::partial_sort_copy or nth_element).
// B3 Explain your parameter/return choices in one sentence.
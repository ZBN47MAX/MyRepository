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

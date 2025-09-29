///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A OG code
//   Compute average and pass/fail (threshold 50)
#include <iostream>
using namespace std;

int main()
{
    int a = 78;  // exam 1
    int b = 85;  // exam 2
    int c = 92;  // exam 3
    int tmp = a; // <-- suspicious
    int total = 0;
    total = a + b + c;
    double avg; // <-- declared early, uninitialised
    avg = total / 3.0;
    bool t = true; // <-- defaulted, then overwritten
    if (avg < 50.0)
    {
        t = false;
    }
    cout << "Average: " << avg << "  passed? " << (t ? "yes" : "no") << endl;
    return 0;
}

// A1 Rewrite it with fewer, clearer variables.
//  Compute average and pass/fail (threshold 50)
#include <iostream>
using namespace std;

int main()
{
    int a = 78; // exam 1
    int b = 85; // exam 2
    int c = 92; // exam 3
                // tmp = a;       // <-- suspicious
    int total = 0;
    total = a + b + c;
    double avg; // <-- declared early, uninitialised
    avg = total / 3.0;
    bool t = true; // <-- defaulted, then overwritten
    if (avg < 50.0)
    {
        t = false;
    }
    cout << "Average: " << avg << "  passed? " << (t ? "yes" : "no") << endl;
    return 0;
}

// A2q List the variables to remove or rename and explain why.
// A2a tmp要删除因为没有使用，abct都应该被重新命名为更有意义的标识符

// A3q Refactor so each variable has a clear purpose and is used > 1 time (or make it const if it shouldn’t change).
// A3a1 abc total 还有avg都可以换成const
// A3a2 最终修改代码
#include <iostream>
using namespace std;

int main()
{
    const int a = 78; // exam 1
    const int b = 85; // exam 2
    const int c = 92; // exam 3
    // tmp = a;       // <-- suspicious
    const int total = a + b + c;
    const double avg = total / 3.0;
    bool t = true; // <-- defaulted, then overwritten
    if (avg < 50.0)
    {
        t = false;
    }
    cout << "Average: " << avg << "  passed? " << (t ? "yes" : "no") << endl;
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// B OG Code
int x = 40;
int y = 60;
int z = x + y;
double a1 = z / 2.0; // average?
bool f = a1 > 50.0;  // pass flag?

// B1 Improve the identifiers so the code reads like its intention (no comments needed).
// improved code
int num1 = 40;
int num2 = 60;
int total = x + y;
double avg = total / 2.0; // average?
bool pass = avg > 50.0;   // pass flag?

// B2 Rename variables to communicate meaning (e.g., leftScore, rightScore, sum, average, passed).
int leftscore = 40;
int rightscore = 60;
int sum = x + y;
double avg = sum / 2.0;   // average?
bool passed = avg > 50.0; // pass flag?

// B3 Mark values that shouldn’t change as const.
const int leftscore = 40;
const int rightscore = 60;
const int sum = x + y;
const double avg = sum / 2.0;   // average?
const bool passed = avg > 50.0; // pass flag?
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//C OG code
double average;
int n; cin >> n;
int sum = 0;
for (int i = 0; i < n; ++i) {
    int v; cin >> v;
    sum += v;
}
average = sum / static_cast<double>(n);
cout << "Average: " << average << "\n";

//C1 Move declarations as close as possible to first use, and mark read-only variables as const.
  int n;
    cin >> n;
    int sum = 0;
    for (int i = 0; i < n; ++i)
    {
        int v;
        cin >> v;
        sum += v;
    }
    const double average = sum / static_cast<double>(n);
    cout << "Average: " << average << "\n";


//C2 Which variables can be const?
// average因为在优化后声明赋值完成后没有涉及计算的部分

//C3 Where can you narrow scope without hurting readability?
// average因为原版的变量声明后未赋值且直到最后才使用

//C4 What edge case must you guard (hint: value of n)?
//必须是大于0的整数

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//D OG Code
#include <iostream>
using namespace std;

int main() {
    int maxVal;            // <-- uninitialised
    int count; cin >> count;
    for (int i = 0; i < count; ++i) {
        int val; cin >> val;
        if (val > maxVal)  // compare against garbage on first iteration
            maxVal = val;
    }
    cout << "Max: " << maxVal << "\n";
}

//D1 The following program occasionally prints the wrong “max”. Identify the bug and the exact variable transition where it happens. Fix it.
//happens at "if (val > maxVal)",发生在前述位置，因为最开始maxVal没有赋值，因此val会被拿去与原本内存地址中的数据进行对比，导致产出垃圾数据
//fixed
#include <iostream>
using namespace std;

int main() {
    int maxVal = 0;            // <-- initialised
    int count; cin >> count;
    for (int i = 0; i < count; ++i) {
        int val; cin >> val;
        if (val > maxVal)  
            maxVal = val;
    }
    cout << "Max: " << maxVal << "\n";
}

//D2 Propose two safe initialisation strategies for maxVal (think: first element vs sentinel).
//use first element
#include <iostream>
using namespace std;

int main()
{
    int count;
    cin >> count;
    if (count > 0)
    {
        int maxVal;//用户输入第一个参数
        cin >> maxVal; 
        for (int i = 1; i < count; ++i)//因为用户已经给出一个参数了，所以循环次数减少一次
        {
            int val;
            cin >> val;
            if (val > maxVal) 
                maxVal = val;
        }
        cout << "Max: " << maxVal << "\n";
    }
    else
    {
        cout << "please enter a integer thats larger than 0" << endl;
    }
}

//use sentinel
#include <iostream>
#include <climits>  // 用于 INT_MIN
using namespace std;

int main()
{
    int count;
    cin >> count;
    if (count > 0)
    {
        int maxVal = INT_MIN;
        for (int i = 0; i < count; ++i)
        {
            int val;
            cin >> val;
            if (val > maxVal) 
                maxVal = val;
        }
        cout << "Max: " << maxVal << "\n";
    }
    else
    {
        cout << "please enter a integer thats larger than 0" << endl;
    }
}

//D3 List a minimal “watch list” of variables for the debugger, with why each matters.
//debugger watch list
/*
变量	调试时观察点	 预期变化
count	输入后	        反映用户输入的正整数
maxVal	每次比较后	    只在找到更大值时更新
i	    每次循环后	    0 → 1 → 2 ... → count-1
val	    每次输入后	    显示当前读取的数值
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

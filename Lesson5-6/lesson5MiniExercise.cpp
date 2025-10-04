///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A OG code
int main();
// 标准版，可用
int main(int argc, char *argv[]);
// 标准命令行参数，可用，基于数组
int main(int argc, char **argv);
// 标准命令行参数，可用，基于指针
int main(char **argv, int argc); // ?
// 参数顺序错误
int main(const int argc, const char **); // ?
// 不能对main使用const
auto main() -> int; // (C++ trailing return)
// C++11的新写法，把返回值类型摆到后面了，实际功能和标准版内容无异
// A1 Which of these are valid and portable forms of main? Explain why for each.
// 如上

// A2 Choose one signature you’ll use in this course and justify (readability + portability).
// 标准版，方便修改，基于传统便于理解
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// B OG Code
#include <iostream>
int main(int argc, char *argv[])
{
    std::cout << "Start\n";
    if (argc < 2)
    {
        std::cout << "Retry\n";
        return main(argc + 1, argv);
    } // ❌
    std::cout << "Done\n";
    return 0;
}
// B1 Identify the problem and refactor so the program has a testable entry function you call from main.
// 在主函数中调用主函数，导致循环
// 重构
#include <iostream>
using namespace std;

// 可测试的业务逻辑函数
bool run(int argCount, char *argValues[])
{
    cout << "Start\n";
    if (argCount < 2)
    {
        cout << "Retry\n";
        return false; // 表示需要重试
    }
    cout << "Done\n";
    return true; // 表示完成
}

int main(int argc, char *argv[])
{
    // 模拟重试逻辑
    int attempts = 0;
    const int maxAttempts = 3;

    while (attempts < maxAttempts)
    {
        if (run(argc + attempts, argv))
        {
            break; // 成功完成，退出循环
        }
        attempts++;
        cout << "Attempt " << attempts << " failed, retrying...\n";
    }

    if (attempts == maxAttempts)
    {
        cout << "All attempts failed!\n";
        return 1;
    }

    return 0;
}
// B2 Create int run(int argc, char* argv[]) and have main call run once.
// see above

// B3 Explain why calling main is undefined/forbidden.
// causing recursion

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// C OG Code
#include <iostream>
int main(int argc, char *argv[])
{
    for (int i = 0; i <= argc; ++i)
    { // ❌ out-of-bounds
        std::cout << "arg[" << i << "] = " << argv[i] << "\n";
    }
    return 0;
}
// C1 The program intends to echo user args, but it’s wrong. Fix it and convert to modern C++ strings.
// refactor
#include <iostream>
#include <vector>  // 必须包含这个头文件
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    // 1. 创建一个空的字符串vector
    vector<string> args;
    
    // 2. 把命令行参数逐个添加到vector中
    for (int i = 1; i < argc; ++i)
    {
        args.push_back(argv[i]);  // push_back：在末尾添加元素
    }
    
    // 3. 打印vector中的参数
    // 方法1：像数组一样用下标访问
    for (int i = 0; i < args.size(); ++i)  // size() 获取元素个数
    {
        cout << "arg[" << i + 1 << "] = " << args[i] << "\n";
    }
    
    return 0;
}
// C2 Print only user-provided arguments (skip program name).
#include <iostream>
using namespace std;
int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        cout << "arg[" << i << "] = " << argv[i] << "\n";
    }
    return 0;
}
// C3 Build std::vector<std::string> from argv and print that instead.
#include <iostream>
#include <vector>  // 必须包含这个头文件
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    // 1. 创建一个空的字符串vector
    vector<string> args;
    
    // 2. 把命令行参数逐个添加到vector中
    for (int i = 1; i < argc; ++i)
    {
        args.push_back(argv[i]);  // push_back：在末尾添加元素
    }
    
    // 3. 打印vector中的参数
    // 方法1：像数组一样用下标访问
    for (int i = 0; i < args.size(); ++i)  // size() 获取元素个数
    {
        cout << "arg[" << i + 1 << "] = " << args[i] << "\n";
    }
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// C OG Code

//Implement a program that sums integer arguments: on success prints the sum and returns 0; on the first non-integer, prints an error and returns a nonzero code.
//Use std::from_chars (no exceptions, no locales) to parse each arg.
//Choose and document your exit codes (e.g., ParseError when an arg isn’t an integer).
#include <iostream>
#include <string>
#include <vector>
#include <charconv>   // from_chars

enum ExitCode { Ok = 0, BadUsage = 64, ParseError = 65 };

int main(int argc, char* argv[]) {
      for (int i = 0; i < argc; ++i)  // size() 获取元素个数
    {
        cout << "arg[" << i + 1 << "] = " << argc << "\n";
    }
}
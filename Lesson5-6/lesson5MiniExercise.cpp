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

// 可测试的业务逻辑函数
bool processArguments(int argCount, char* argValues[]) {
    std::cout << "Start\n";
    if (argCount < 2) {
        std::cout << "Retry\n";
        return false;  // 表示需要重试
    }
    std::cout << "Done\n";
    return true;  // 表示完成
}

int main(int argc, char *argv[]) {
    // 模拟重试逻辑
    int attempts = 0;
    const int maxAttempts = 3;
    
    while (attempts < maxAttempts) {
        if (processArguments(argc + attempts, argv)) {
            break;  // 成功完成，退出循环
        }
        attempts++;
        std::cout << "Attempt " << attempts << " failed, retrying...\n";
    }
    
    if (attempts == maxAttempts) {
        std::cout << "All attempts failed!\n";
        return 1;
    }
    
    return 0;
}
// B2 Create int run(int argc, char* argv[]) and have main call run once.
// B3 Explain why calling main is undefined/forbidden.

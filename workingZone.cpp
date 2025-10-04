#include <iostream>
#include <string>
#include <cstdlib>  // 使用 strtol 代替 std::from_chars

using namespace std;

enum ExitCode { 
    Ok = 0, 
    BadUsage = 64, 
    ParseError = 65 
};

int main(int argc, char* argv[]) {
    // 检查是否有参数
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <number1> <number2> ...\n";
        return BadUsage;
    }
    
    long total = 0;  // 用于存储总和
    
    // 遍历所有参数（从第一个参数开始，跳过程序名）
    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];
        char* endptr;
        
        // 使用 strtol 解析整数
        long value = strtol(arg, &endptr, 10);
        
        // 检查解析是否成功
        if (endptr == arg) {
            cerr << "Error: '" << arg << "' is not a valid integer\n";
            return ParseError;
        }
        
        // 检查是否整个字符串都被解析了
        if (*endptr != '\0') {
            cerr << "Error: '" << arg << "' contains non-integer characters\n";
            return ParseError;
        }
        
        // 累加到总和
        total += value;
    }
    
    // 输出结果
    cout << "Sum: " << total << endl;
    return Ok;
}
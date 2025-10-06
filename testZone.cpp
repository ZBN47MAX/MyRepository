#include "head.h"
#include <cstdlib> // 用于 atoi

// 定义全局变量
int player1TotalScore = 0;
int player2TotalScore = 0;
bool player1LastRoundChoice = false;
bool player2LastRoundChoice = false;
int totalRounds = 3;
bool strategyValue = false;
bool TFTvalueP1 = false;
int currentRound = 0;
bool p1DefactTrigger = false;
bool p2DefactTrigger = false;

// 策略函数：永远合作
bool ALLC()
{
    return true; // true 代表合作
}

// 策略函数：永远背叛
bool ALLD()
{
    return false; // false 代表背叛
}

// 以牙还牙策略 - 修正版
bool TFTp1()
{
    // 第一回合总是合作
    if (currentRound == 0)
    {
        p1DefactTrigger = false; // 重置触发标志
        return true;
    }

    // 如果对手上回合背叛，触发永久背叛
    if (player2LastRoundChoice == false)
    {
        p1DefactTrigger = true;
    }

    // 如果已经触发背叛，就继续背叛
    if (p1DefactTrigger == true)
    {
        return false;
    }

    // 否则合作
    return true;
}

bool TFTp2()
{
    // 第一回合总是合作
    if (currentRound == 0)
    {
        p1DefactTrigger = false; // 重置触发标志
        return true;
    }

    // 如果对手上回合背叛，触发永久背叛
    if (player1LastRoundChoice == false)
    {
        p1DefactTrigger = true;
    }

    // 如果已经触发背叛，就继续背叛
    if (p2DefactTrigger == true)
    {
        return false;
    }

    // 否则合作
    return true;
}

// 自定义策略：第二局开始背叛 - 修正版
bool custom()
{
    if (currentRound == 0)
    {                // 第一回合
        return true; // 合作
    }
    else
    {
        return false; // 第二回合开始背叛
    }
}

bool bullshits()
{
    return (rand() % 2) == 0;
}

// 根据策略名称返回对应的策略函数
bool (*getStrategy(const string &strategyName))()
{
    if (strategyName == "ALLC" || strategyName == "allc")
    {
        return ALLC;
    }
    else if (strategyName == "ALLD" || strategyName == "alld")
    {
        return ALLD;
    }
    else if (strategyName == "TFTp1" || strategyName == "tftp1")
    {
        return TFTp1;
    }
    else if (strategyName == "BULLSHITS" || strategyName == "bullshits")
    { // 新增随机策略
        return bullshits;
    }
    else if (strategyName == "CUSTOM" || strategyName == "custom")
    {
        return custom;
    }
    else
    {
        // 默认使用永远合作
        cout << "unknow tatic '" << strategyName << "'use default(ALLC) " << endl;
        return ALLC;
    }
}

// 对战场地函数 - 修正版
int arena(bool (*player1Strategy)(), bool (*player2Strategy)(), int rounds)
{
    totalRounds = rounds; // 设置回合数

    // 重置总分和状态
    player1TotalScore = 0;
    player2TotalScore = 0;
    TFTvalueP1 = false;
    currentRound = 0;
    p1DefactTrigger = false;

    cout << "Game Start! : " << endl;
    cout << "Total Rounds: " << totalRounds << endl;
    cout << "------------------------" << endl;

    for (int i = 0; i < totalRounds; i++)
    {
        currentRound = i; // 更新当前回合数

        // 获取双方本回合策略选择
        bool player1Choice = player1Strategy();
        bool player2Choice = player2Strategy();

        int player1RoundScore = 0;
        int player2RoundScore = 0;

        // 根据策略选择计算得分
        if (player1Choice == true && player2Choice == true)
        {
            // 双方合作
            player1RoundScore = 3;
            player2RoundScore = 3;
        }
        else if (player1Choice == true && player2Choice == false)
        {
            // 玩家1合作，玩家2背叛
            player1RoundScore = 0;
            player2RoundScore = 5;
        }
        else if (player1Choice == false && player2Choice == true)
        {
            // 玩家1背叛，玩家2合作
            player1RoundScore = 5;
            player2RoundScore = 0;
        }
        else if (player1Choice == false && player2Choice == false)
        {
            // 双方背叛
            player1RoundScore = 1;
            player2RoundScore = 1;
        }

        // 记录本回合选择（用于下回合参考）
        player1LastRoundChoice = player1Choice;
        player2LastRoundChoice = player2Choice;

        // 更新总分
        player1TotalScore += player1RoundScore;
        player2TotalScore += player2RoundScore;

        // 输出本回合结果
        cout << "Round: " << (i + 1) << endl;
        cout << "Player 1: " << (player1Choice ? "Cooperate" : "Defect")
             << " | Scored: " << player1RoundScore << endl;
        cout << "Player 2: " << (player2Choice ? "Cooperate" : "Defect")
             << " | Scored: " << player2RoundScore << endl;
        cout << "Current Total - P1: " << player1TotalScore
             << " | P2: " << player2TotalScore << endl;
        cout << "==========================" << endl;
    }

    // 输出最终结果
    cout << "=========Summary==========" << endl;
    cout << "Player 1 Total Scored: " << player1TotalScore << endl;
    cout << "Player 1 Average/Round: " << (player1TotalScore / totalRounds) << endl;
    cout << "Player 2 Total Scored: " << player2TotalScore << endl;
    cout << "Player 2 Average/Round: " << (player2TotalScore / totalRounds) << endl;

    return 0;
}

// 显示帮助信息
// 显示帮助信息
void showHelp(const string &programName)
{
    cout << "How to use: " << programName << " [options]" << endl;
    cout << "options:" << endl;
    cout << "  --p1 <strategy>        Player 1 strategy (ALLC, ALLD, TFT, Bullshits)" << endl;
    cout << "  --p2 <strategy>        Player 2 strategy (ALLC, ALLD, CUSTOM, Bullshits)" << endl;
    cout << "  --rounds <number>      Number of rounds per match (default: 5)" << endl;
    cout << "  --repeats <number> Number of repetitions (default: 1)" << endl;
    cout << "  --help                 Show this help information" << endl;
    cout << endl;
    cout << "Examples:" << endl;
    cout << "  " << programName << " --p1 TFT --p2 CUSTOM --rounds 10" << endl;
    cout << "  " << programName << " --p1 ALLC --p2 ALLD --rounds 5 --repetitions 3" << endl;
}

int main(int argc, char *argv[])
{

    // 默认参数
    string player1Strategy = "TFT";
    string player2Strategy = "CUSTOM";
    int rounds = 5;
    int repeats = 1; // 新增默认重复次数

    // 解析命令行参数
    for (int i = 1; i < argc; i++)
    {
        string arg = argv[i];

        if (arg == "--p1" && i + 1 < argc)
        {
            player1Strategy = argv[i + 1];
            i++; // 跳过下一个参数
        }
        else if (arg == "--p2" && i + 1 < argc)
        {
            player2Strategy = argv[i + 1];
            i++;
        }
        else if (arg == "--rounds" && i + 1 < argc)
        {
            rounds = atoi(argv[i + 1]); // 字符串转整数
            i++;
        }
        else if (arg == "--repeats" && i + 1 < argc)
        { // 新增重复次数参数
            repeats = atoi(argv[i + 1]);
            if (repeats <= 0)
            {
                cout << "Error: repetitions must be positive" << endl;
                return 1;
            }
            i++;
        }
        else if (arg == "--help")
        {
            showHelp(argv[0]);
            return 0;
        }
        else
        {
            cout << "unknow: " << arg << endl;
            cout << "use --help to get info" << endl;
            return 1;
        }
    }

    cout << "=== IPD! START!!! ===" << endl;
    cout << "P1 Tatic: " << player1Strategy << endl;
    cout << "P2 Tatic: " << player2Strategy << endl;
    cout << "Rounds per match: " << rounds << endl;
    cout << "repeats: " << repeats << " times" << endl;
    cout << "=====================" << endl;

    // 获取策略函数
    bool (*strategy1)() = getStrategy(player1Strategy);
    bool (*strategy2)() = getStrategy(player2Strategy);

    // 进行多次重复实验
    for (int rep = 0; rep < repeats; rep++)
    {
        if (repeats > 1)
        {
            cout << "\n=== Repetition " << (rep + 1) << " ===" << endl;
        }

        // 进行对战
        arena(strategy1, strategy2, rounds);
    }
    return 0;
}
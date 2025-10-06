#include "head.h"
#include "strategies.h"

// 对战场地函数 - 修正版
int arena(bool (*player1Strategy)(), bool (*player2Strategy)())  // 改为函数指针
{
    // 重置总分和状态
    player1TotalScore = 0;
    player2TotalScore = 0;
    TFTvalueP1 = false;
    currentRound = 0;

    cout << "Game Start! : " << endl;
    cout << "Total Rounds: " << totalRounds << endl;
    cout << "------------------------" << endl;

    for (int i = 0; i < totalRounds; i++)
    {
        currentRound = i;  // 更新当前回合数
        
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
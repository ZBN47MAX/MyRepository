#ifndef HEAD_H
#define HEAD_H


#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <cstring>
#include <stdexcept>
#include <fstream>
#include <random>
#include <chrono>
#include <sstream>
#include <sys/stat.h>  // 用于目录操作
#ifdef _WIN32
#include <direct.h>    // Windows 的 mkdir
#endif

using namespace std;



// 声明全局变量
extern int player1TotalScore;
extern int player2TotalScore;
extern bool player1LastRoundChoice;
extern bool player2LastRoundChoice;
extern int totalRounds;
extern bool strategyValue;
extern bool TFTvalueP1;
extern int currentRound;
extern bool p1DefactTrigger;
extern bool p2DefactTrigger;
extern int repeats;

// 声明函数
//bool ALLC();
//bool ALLD();
//bool TFTp1();
//bool custom();
//int arena(bool (*player1Strategy)(), bool (*player2Strategy)());

#endif
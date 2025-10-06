#include "head.h"

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
    if (currentRound == 0) {
        p1DefactTrigger = false;  // 重置触发标志
        return true;
    }
    
    // 如果对手上回合背叛，触发永久背叛
    if (player2LastRoundChoice == false) {
        p1DefactTrigger = true;
    }
    
    // 如果已经触发背叛，就继续背叛
    if (p1DefactTrigger == true) {
        return false;
    }
    
    // 否则合作
    return true;
}

// 自定义策略：第二局开始背叛 - 修正版
bool custom(){
    if(currentRound == 0){  // 第一回合
        return true;  // 合作
    } else {
        return false; // 第二回合开始背叛
    }
}
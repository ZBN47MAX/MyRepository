#ifndef STRATEGY_H
#define STRATEGY_H

#include <vector>
#include <string>
#include <random>

// Strategy base class
class Strategy {
public:
    virtual std::string get_name() = 0;
    virtual bool make_choice(const std::vector<bool>& my_history, 
                           const std::vector<bool>& opponent_history,
                           const std::vector<int>& my_scores, 
                           const std::vector<int>& opponent_scores) = 0;
    virtual void reset() {}
    virtual ~Strategy() {}
};

// 所有具体策略类的声明
class ALLC : public Strategy {
public:
    std::string get_name() override;
    bool make_choice(const std::vector<bool>& my_history, 
                    const std::vector<bool>& opponent_history,
                    const std::vector<int>& my_scores, 
                    const std::vector<int>& opponent_scores) override;
};

class ALLD : public Strategy {
    // ... 类似声明
};

class TFT : public Strategy {
    // ... 类似声明
};

// 其他策略类声明...
class GrimTrigger : public Strategy;
class Pavlov : public Strategy;
class Random : public Strategy;
class Probe : public Strategy;
class CTFT : public Strategy;
class custom1 : public Strategy;
class custom2 : public Strategy;

#endif

#include "Strategy.h"
#include <iostream>

// ALLC 实现
std::string ALLC::get_name() { return "ALLC"; }
bool ALLC::make_choice(const std::vector<bool>&, const std::vector<bool>&,
                      const std::vector<int>&, const std::vector<int>&) {
    return true;
}

// ALLD 实现
std::string ALLD::get_name() { return "ALLD"; }
bool ALLD::make_choice(const std::vector<bool>&, const std::vector<bool>&,
                      const std::vector<int>&, const std::vector<int>&) {
    return false;
}

// TFT 实现
std::string TFT::get_name() { return "TFT"; }
bool TFT::make_choice(const std::vector<bool>&, const std::vector<bool>& opponent_history,
                     const std::vector<int>&, const std::vector<int>&) {
    if (opponent_history.empty()) return true;
    return opponent_history.back();
}

// 其他策略类的实现...
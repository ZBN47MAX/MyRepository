#ifndef STRATEGY_H
#define STRATEGY_H

#include "pch.h"


class Strategy {
public:
    virtual ~Strategy() = default;
    virtual std::string get_name() = 0;
    virtual bool make_choice(const std::vector<bool>& my_history,
        const std::vector<bool>& opponent_history,
        const std::vector<int>& my_scores,
        const std::vector<int>& opponent_scores) = 0;
    virtual void reset() {}
};


class ALLC : public Strategy {
public:
    std::string get_name() override;
    bool make_choice(const std::vector<bool>& my_history,
        const std::vector<bool>& opponent_history,
        const std::vector<int>& my_scores,
        const std::vector<int>& opponent_scores) override;
};


class ALLD : public Strategy {
public:
    std::string get_name() override;
    bool make_choice(const std::vector<bool>& my_history,
        const std::vector<bool>& opponent_history,
        const std::vector<int>& my_scores,
        const std::vector<int>& opponent_scores) override;
};


class TFT : public Strategy {
public:
    std::string get_name() override;
    bool make_choice(const std::vector<bool>& my_history,
        const std::vector<bool>& opponent_history,
        const std::vector<int>& my_scores,
        const std::vector<int>& opponent_scores) override;
};


class GrimTrigger : public Strategy {
private:
    bool triggered = false;
public:
    std::string get_name() override;
    bool make_choice(const std::vector<bool>& my_history,
        const std::vector<bool>& opponent_history,
        const std::vector<int>& my_scores,
        const std::vector<int>& opponent_scores) override;
    void reset() override;
};


class Pavlov : public Strategy {
public:
    std::string get_name() override;
    bool make_choice(const std::vector<bool>& my_history,
        const std::vector<bool>& opponent_history,
        const std::vector<int>& my_scores,
        const std::vector<int>& opponent_scores) override;
};


class Random : public Strategy {
private:
    std::mt19937 gen;
    std::uniform_real_distribution<double> dis;
public:
    Random();
    std::string get_name() override;
    bool make_choice(const std::vector<bool>& my_history,
        const std::vector<bool>& opponent_history,
        const std::vector<int>& my_scores,
        const std::vector<int>& opponent_scores) override;
};


class Probe : public Strategy {
private:
    bool exploit_mode = false;
public:
    std::string get_name() override;
    bool make_choice(const std::vector<bool>& my_history,
        const std::vector<bool>& opponent_history,
        const std::vector<int>& my_scores,
        const std::vector<int>& opponent_scores) override;
    void reset() override;
};


class CTFT : public Strategy {
private:
    bool last_was_mistake = false;
public:
    std::string get_name() override;
    bool make_choice(const std::vector<bool>& my_history,
        const std::vector<bool>& opponent_history,
        const std::vector<int>& my_scores,
        const std::vector<int>& opponent_scores) override;
    void reset() override;
};


class Custom1 : public Strategy {
private:
    int opponent_defects = 0;
    std::mt19937 gen;
    std::uniform_real_distribution<double> dis;
public:
    Custom1();
    std::string get_name() override;
    bool make_choice(const std::vector<bool>& my_history,
        const std::vector<bool>& opponent_history,
        const std::vector<int>& my_scores,
        const std::vector<int>& opponent_scores) override;
    void reset() override;
};


class Custom2 : public Strategy {
private:
    std::mt19937 gen;
    std::uniform_real_distribution<double> dis;
public:
    Custom2();
    std::string get_name() override;
    bool make_choice(const std::vector<bool>& my_history,
        const std::vector<bool>& opponent_history,
        const std::vector<int>& my_scores,
        const std::vector<int>& opponent_scores) override;
    void reset() override;
};

#endif 
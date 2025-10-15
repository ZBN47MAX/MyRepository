#ifndef GAMECORE_H
#define GAMECORE_H

#include "pch.h"
#include "Strategy.h"
#include "Noise.h"


class GameManager;


class Player {
public:
    std::string id;
    Strategy* strategy;
    int last_score = 0;
    int total_score = 0;
    std::vector<bool> choice_history;
    std::vector<int> score_history;
    std::vector<bool> actual_choices; 

    Player(std::string player_id, Strategy* strat);
    bool make_choice(const std::vector<bool>& opponent_history);
    void add_score(int score);
    void reset();
    std::vector<bool> get_choice_history() const;
    std::vector<bool> get_actual_choices() const;
};


struct GameResult {
    double mean_score;
    double std_dev;
    double confidence_interval;
    std::vector<int> all_scores;

    GameResult();
};


class Arena {
private:
    int REWARD;     
    int SUCKER;     
    int TEMPTATION; 
    int PUNISHMENT; 

public:
    Arena(int T, int R, int P, int S);
    bool validate_scores();
    void print_scores();
    std::pair<int, int> play_round(Player& player1, Player& player2);
    std::vector<int> get_scores();
};


class NoiseSweep {
private:
    GameResult calculate_statistics(const std::vector<int>& scores);

public:
    void run_sweep(const std::vector<std::string>& strategies, int rounds, int repeats,
        const std::vector<double>& noise_levels, Arena& base_arena);
};

#endif 

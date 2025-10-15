#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "pch.h"
#include "Strategy.h"
#include "GameCore.h"


extern std::string OUTPUT_PATH;
extern std::string OUTPUT_FORMAT;

class GameManager {
private:
    std::vector<Strategy*> strategies;
    std::map<std::string, Strategy*> strategy_map;
    std::vector<std::vector<std::string>> game_logs;
    std::string current_session_id;

public:
    GameManager();
    ~GameManager();

    void set_session_id(const std::string& session_id);
    void add_strategy(Strategy* strategy);
    void log_game_result(const std::string& p1_strategy, const std::string& p2_strategy,
        int rounds, int p1_score, int p2_score,
        int T, int R, int P, int S, double noise_level);
    void write_csv_file();
    void write_txt_file();
    void write_output_file();
    void play_single_game(int rounds, Player& player1, Player& player2, Arena& arena);
    void tournament(int rounds, int repeats, std::string fixed_strategy_name, Arena& arena);
    void show_strategies();
    Strategy* get_strategy(const std::string& name);
    void run_q2_experiment(int rounds, int repeats, const std::vector<double>& noise_levels, Arena& arena);
};


bool create_directory(const std::string& path);
bool directory_exists(const std::string& path);
void create_output_directories();
std::string get_output_path(const std::string& format = "csv");
std::string generate_unique_id(const std::string& p1_strategy = "ALLC",
    const std::string& p2_strategy = "ALLD",
    int rounds = 3, int games = 5);
void show_help();
int safe_stoi(const std::string& str, const std::string& param_name);
double safe_stod(const std::string& str, const std::string& param_name);
std::vector<int> parse_scores(const std::string& score_str);
std::vector<double> parse_noise_levels(const std::string& noise_str);
bool validate_score_rules(int T, int R, int P, int S);
void parse_args(int argc, char* argv[], GameManager& manager);

#endif 
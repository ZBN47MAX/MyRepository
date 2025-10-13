//待办：
/*
增加种子生成器（能影响回合数，对局数，玩家1使用策略，玩家2使用策略，是否激活策略进化，噪音级别）保证相同种子能产出相同结果
增加对局噪音（完成
增加对局进化 给有进化的策略加个开关
增加保存或载入文件
增加对局变异 在策略中增加的有规律的特殊噪音
增加输出文件格式选择（完成，除了json
已有完全合作，完全背叛，标准以牙还牙。增加策略：冷酷触发，巴甫洛夫，随机，反悔以牙还牙，探针策略（完成
增加两个全新策略
*/
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

// 创建目录（跨平台）
bool create_directory(const string& path) {
#ifdef _WIN32
    return _mkdir(path.c_str()) == 0;
#else
    return mkdir(path.c_str(), 0755) == 0;
#endif
}

// 检查目录是否存在
bool directory_exists(const string& path) {
    struct stat info;
    return stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR);
}

// 创建所有必要的目录
void create_output_directories() {
    vector<string> dirs = {"Record", "Record/", "Record/TXT"};
    
    for (const auto& dir : dirs) {
        if (!directory_exists(dir)) {
            if (create_directory(dir)) {
                cout << ">>> Created directory: " << dir << endl;
            } else {
                cout << "!!! Warning: Could not create directory: " << dir << endl;
            }
        }
    }
}

// 噪音类 - 作为第三方模块影响策略输出结果
class Noise {
private:
    double noise_level;  // 噪音水平：0-1，0无噪音，1完全噪音
    mt19937 gen;
    uniform_real_distribution<double> dis;
    
public:
    Noise(double level = 0.0) : noise_level(max(0.0, min(1.0, level))), 
                               gen(random_device{}()), 
                               dis(0.0, 1.0) {}
    
    // 应用噪音到策略选择
    bool apply_noise(bool original_choice) {
        if (noise_level <= 0.0) {
            return original_choice;  // 无噪音，返回原选择
        }
        
        if (dis(gen) < noise_level) {
            return !original_choice;  // 应用噪音，反转选择
        }
        
        return original_choice;  // 保持原选择
    }
    
    double get_noise_level() const {
        return noise_level;
    }
    
    void set_noise_level(double level) {
        noise_level = max(0.0, min(1.0, level));
    }
};

// 全局噪音实例
Noise global_noise(0.0);

// 获取输出文件路径
string get_output_path(const string& format = "csv") {
    string base_dir = "Record";
    string format_upper = format;
    for (auto& c : format_upper) c = toupper(c);
    
    string output_dir = base_dir + "/" + format_upper;
    
    // 确保目录存在
    if (!directory_exists(output_dir)) {
        create_directory(output_dir);
    }
    
    // 生成文件名
    auto now = chrono::system_clock::now();
    time_t time = chrono::system_clock::to_time_t(now);
    tm* local_time = localtime(&time);
    
    stringstream filename;
    filename << "prisoners_dilemma_"
             << (local_time->tm_year + 1900) 
             << setw(2) << setfill('0') << (local_time->tm_mon + 1)
             << setw(2) << setfill('0') << local_time->tm_mday
             << "_"
             << setw(2) << setfill('0') << local_time->tm_hour
             << setw(2) << setfill('0') << local_time->tm_min
             << "." << format;
    
    return output_dir + "/" + filename.str();
}

// 全局文件路径
string OUTPUT_PATH = get_output_path();
string OUTPUT_FORMAT = "csv";

/// 生成新的识别编号格式：YYYYMMDDHHMMP1[策略]P2[策略]R[回合数]G[游戏数]
string generate_unique_id(const string& p1_strategy = "ALLC", 
                         const string& p2_strategy = "ALLD", 
                         int rounds = 3, int games = 5) {
    // 获取当前时间
    auto now = chrono::system_clock::now();
    time_t time = chrono::system_clock::to_time_t(now);
    tm* local_time = localtime(&time);
    
    // 格式化为 YYYYMMDDHHMM
    stringstream time_ss;
    time_ss << (local_time->tm_year + 1900) 
            << setw(2) << setfill('0') << (local_time->tm_mon + 1)
            << setw(2) << setfill('0') << local_time->tm_mday
            << setw(2) << setfill('0') << local_time->tm_hour
            << setw(2) << setfill('0') << local_time->tm_min;
    
    // 组合成完整识别码
    stringstream id_ss;
    id_ss << time_ss.str() 
          << "P1" << p1_strategy
          << "P2" << p2_strategy
          << "R" << rounds
          << "G" << games;
    
    return id_ss.str();
}

// Strategy base class
class Strategy {
public:
    virtual string get_name() = 0;
    virtual bool make_choice(const vector<bool>& my_history, const vector<bool>& opponent_history, 
                            const vector<int>& my_scores, const vector<int>& opponent_scores) = 0;
    virtual void reset() {}  // 重置策略状态
    virtual ~Strategy() {}
};

// 永远合作策略
class ALLC : public Strategy {
public:
    string get_name() override {
        return "ALLC";
    }
    bool make_choice(const vector<bool>& my_history, const vector<bool>& opponent_history,
                    const vector<int>& my_scores, const vector<int>& opponent_scores) override {
        return true;  // 永远合作
    }
};

// 永远背叛策略
class ALLD : public Strategy {
public:
    string get_name() override {
        return "ALLD";
    }
    bool make_choice(const vector<bool>& my_history, const vector<bool>& opponent_history,
                    const vector<int>& my_scores, const vector<int>& opponent_scores) override {
        return false;  // 永远背叛
    }
};

// 以牙还牙策略
class TFT : public Strategy {
public:
    string get_name() override {
        return "TFT";
    }
    bool make_choice(const vector<bool>& my_history, const vector<bool>& opponent_history,
                    const vector<int>& my_scores, const vector<int>& opponent_scores) override {
        if (opponent_history.empty()) {
            return true;  // 第一回合合作
        }
        return opponent_history.back();  // 模仿对手上一回合的行为
    }
};

// 冷酷触发策略
class GrimTrigger : public Strategy {
private:
    bool triggered = false;
    
public:
    string get_name() override {
        return "GrimTrigger";
    }
    
    bool make_choice(const vector<bool>& my_history, const vector<bool>& opponent_history,
                    const vector<int>& my_scores, const vector<int>& opponent_scores) override {
        if (triggered) {
            return false;  // 一旦触发，永远背叛
        }
        
        // 检查对手是否背叛过
        for (bool choice : opponent_history) {
            if (!choice) {
                triggered = true;
                return false;
            }
        }
        
        return true;  // 如果对手从未背叛，继续合作
    }
    
    void reset() override {
        triggered = false;
    }
};

// 巴甫洛夫策略
class Pavlov : public Strategy {
public:
    string get_name() override {
        return "Pavlov";
    }
    
    bool make_choice(const vector<bool>& my_history, const vector<bool>& opponent_history,
                    const vector<int>& my_scores, const vector<int>& opponent_scores) override {
        if (my_history.empty()) {
            return true;  // 第一回合合作
        }
        
        // 获取上一回合的得分
        int my_last_score = my_scores.back();
        int opp_last_score = opponent_scores.back();
        
        // 如果得分比对方高，保持决定；否则切换决定
        if (my_last_score > opp_last_score) {
            return my_history.back();  // 保持
        } else {
            return !my_history.back();  // 切换
        }
    }
};

// 随机策略
class Random : public Strategy {
private:
    mt19937 gen;
    uniform_real_distribution<double> dis;
    
public:
    Random() : gen(random_device{}()), dis(0.0, 1.0) {}
    
    string get_name() override {
        return "Random";
    }
    
    bool make_choice(const vector<bool>& my_history, const vector<bool>& opponent_history,
                    const vector<int>& my_scores, const vector<int>& opponent_scores) override {
        return dis(gen) > 0.5;  // 50%概率合作
    }
};

// 探针策略
class Probe : public Strategy {
private:
    bool exploit_mode = false;
    
public:
    string get_name() override {
        return "Probe";
    }
    
    bool make_choice(const vector<bool>& my_history, const vector<bool>& opponent_history,
                    const vector<int>& my_scores, const vector<int>& opponent_scores) override {
        // 使用历史长度而不是内部计数器，避免边界问题
        int current_round = my_history.size() + 1;
        
        // 前4回合固定模式：合作、背叛、合作、合作
        if (current_round <= 4) {
            switch (current_round) {
                case 1: return true;   // 合作
                case 2: return false;  // 背叛
                case 3: return true;   // 合作
                case 4: return true;   // 合作
            }
        }
        
        // 如果已经进入利用模式，持续背叛
        if (exploit_mode) {
            return false;
        }
        
        // 在第5回合评估对手
        if (current_round == 5) {
            // 检查对手在前4回合的反应
            // 如果对手在前4回合中有3次或以上合作，认为可以被利用
            int cooperate_count = 0;
            for (int i = 0; i < 4 && i < opponent_history.size(); i++) {
                if (opponent_history[i]) cooperate_count++;
            }
            
            cout << ">>> Probe evaluation: opponent cooperated " << cooperate_count 
                 << "/4 times in first 4 rounds" << endl;
            
            if (cooperate_count >= 3) {
                exploit_mode = true;
                cout << ">>> Probe: Switching to EXPLOIT mode (always defect)" << endl;
                return false;  // 开始利用
            } else {
                cout << ">>> Probe: Switching to TIT-FOR-TAT mode" << endl;
                // 切换到以牙还牙
                return opponent_history.back();
            }
        }
        
        // 第6回合及以后，如果在利用模式就继续背叛，否则使用以牙还牙
        if (exploit_mode) {
            return false;
        } else {
            // 确保有历史记录
            if (!opponent_history.empty()) {
                return opponent_history.back();
            }
            return true;  // 安全回退
        }
    }
    
    void reset() override {
        exploit_mode = false;
    }
};

// Player class
class Player {
public:
    string id;
    Strategy* strategy;
    int last_score = 0;
    int total_score = 0;
    vector<bool> choice_history;
    vector<int> score_history;
    vector<bool> actual_choices;  // 应用噪音前的实际选择
    
    Player(string player_id, Strategy* strat) : id(player_id), strategy(strat) {}
    
    bool make_choice(const vector<bool>& opponent_history) {
        bool actual_choice = strategy->make_choice(choice_history, opponent_history, score_history, vector<int>());
        bool final_choice = global_noise.apply_noise(actual_choice);
        
        actual_choices.push_back(actual_choice);
        choice_history.push_back(final_choice);
        return final_choice;
    }
    
    void add_score(int score) {
        last_score = score;
        total_score += score;
        score_history.push_back(score);
    }
    
    void reset() {
        last_score = 0;
        total_score = 0;
        choice_history.clear();
        score_history.clear();
        actual_choices.clear();
        strategy->reset();
    }
    
    vector<bool> get_choice_history() const {
        return choice_history;
    }
    
    vector<bool> get_actual_choices() const {
        return actual_choices;
    }
};

// Arena for single rounds
class Arena {
private:
    int REWARD;      // R - 双方合作
    int SUCKER;      // S - 合作但被背叛
    int TEMPTATION;  // T - 背叛而对方合作
    int PUNISHMENT;  // P - 双方背叛

public:
    Arena(int T, int R, int P, int S) : TEMPTATION(T), REWARD(R), PUNISHMENT(P), SUCKER(S) {}
    
    bool validate_scores() {
        // 规则1: T > R > P > S
        if (!(TEMPTATION > REWARD && REWARD > PUNISHMENT && PUNISHMENT > SUCKER)) {
            return false;
        }
        // 规则2: 2R > T + S
        if (!(2 * REWARD > TEMPTATION + SUCKER)) {
            return false;
        }
        return true;
    }
    
    void print_scores() {
        cout << ">>> Score Matrix: T=" << TEMPTATION << " R=" << REWARD 
             << " P=" << PUNISHMENT << " S=" << SUCKER << endl;
        cout << ">>> Noise Level: " << fixed << setprecision(2) << global_noise.get_noise_level() << endl;
    }
    
    pair<int, int> play_round(Player& player1, Player& player2) {
        bool choice1 = player1.make_choice(player2.get_choice_history());
        bool choice2 = player2.make_choice(player1.get_choice_history());
        
        int score1, score2;
        
        if (choice1 && choice2) {
            score1 = REWARD;
            score2 = REWARD;
        } else if (choice1 && !choice2) {
            score1 = SUCKER;
            score2 = TEMPTATION;
        } else if (!choice1 && choice2) {
            score1 = TEMPTATION;
            score2 = SUCKER;
        } else {
            score1 = PUNISHMENT;
            score2 = PUNISHMENT;
        }
        
        player1.add_score(score1);
        player2.add_score(score2);
        
        // 显示实际选择和最终选择（如果有噪音影响）
        string p1_choice_str = (choice1 ? "COOP" : "DEFECT");
        string p2_choice_str = (choice2 ? "COOP" : "DEFECT");
        
        if (global_noise.get_noise_level() > 0.0) {
            bool actual1 = player1.actual_choices.back();
            bool actual2 = player2.actual_choices.back();
            if (actual1 != choice1) p1_choice_str = (actual1 ? "COOP*" : "DEFECT*") + ("->" + p1_choice_str);
            if (actual2 != choice2) p2_choice_str = (actual2 ? "COOP*" : "DEFECT*") + ("->" + p2_choice_str);
        }
        
        cout << "| Round " << setw(2) << player1.choice_history.size() 
             << " | " << player1.id << ":" << p1_choice_str
             << " vs " << player2.id << ":" << p2_choice_str
             << " | Scores: " << setw(2) << score1 << "-" << setw(2) << score2 << " |" << endl;
        
        return make_pair(score1, score2);
    }
    
    // 获取当前分数配置
    vector<int> get_scores() {
        return {TEMPTATION, REWARD, PUNISHMENT, SUCKER};
    }
};

// Game manager for multiple rounds and games
class GameManager {
private:
    vector<Strategy*> strategies;
    map<string, Strategy*> strategy_map;
    vector<vector<string>> game_logs;
    string current_session_id;

public:
    GameManager() {
        // 注册所有策略
        add_strategy(new ALLC());
        add_strategy(new ALLD());
        add_strategy(new TFT());
        add_strategy(new GrimTrigger());
        add_strategy(new Pavlov());
        add_strategy(new Random());
        add_strategy(new Probe());
    }
    
    void set_session_id(const string& session_id) {
        current_session_id = session_id;
    }
    
    void add_strategy(Strategy* strategy) {
        strategies.push_back(strategy);
        strategy_map[strategy->get_name()] = strategy;
    }
    
    void log_game_result(const string& p1_strategy, const string& p2_strategy, 
                        int rounds, int p1_score, int p2_score, 
                        int T, int R, int P, int S, double noise_level) {
        vector<string> record = {
            current_session_id,
            p1_strategy,
            p2_strategy,
            to_string(rounds),
            to_string(p1_score),
            to_string(p2_score),
            to_string(T),
            to_string(R),
            to_string(P),
            to_string(S),
            to_string(noise_level)
        };
        game_logs.push_back(record);
    }
    
    void write_csv_file() {
        ofstream file(OUTPUT_PATH);
        if (!file.is_open()) {
            cout << "!!! Warning: Could not open CSV file for writing: " << OUTPUT_PATH << endl;
            return;
        }
        
        // 写入表头
        file << "SessionID,Player1Strategy,Player2Strategy,Rounds,Player1Score,Player2Score,T,R,P,S,NoiseLevel\n";
        
        for (const auto& record : game_logs) {
            for (size_t i = 0; i < record.size(); ++i) {
                file << record[i];
                if (i < record.size() - 1) file << ",";
            }
            file << "\n";
        }
        
        file.close();
        cout << ">>> CSV results saved to: " << OUTPUT_PATH << endl;
    }
    
    void write_txt_file() {
        ofstream file(OUTPUT_PATH);
        if (!file.is_open()) {
            cout << "!!! Warning: Could not open TXT file for writing: " << OUTPUT_PATH << endl;
            return;
        }
        
        file << "PRISONER'S DILEMMA SIMULATION RESULTS" << endl;
        file << "Session ID: " << current_session_id << endl;
        file << "Noise Level: " << fixed << setprecision(2) << global_noise.get_noise_level() << endl;
        file << string(50, '=') << endl;
        
        for (size_t i = 0; i < game_logs.size(); ++i) {
            const auto& record = game_logs[i];
            file << "\nGAME " << (i + 1) << ":" << endl;
            file << "  Strategies: " << record[1] << " vs " << record[2] << endl;
            file << "  Rounds: " << record[3] << endl;
            file << "  Scores: " << record[4] << " - " << record[5] << endl;
            file << "  Parameters: T=" << record[6] << " R=" << record[7] 
                 << " P=" << record[8] << " S=" << record[9] << endl;
            file << "  Noise Level: " << record[10] << endl;
        }
        
        file.close();
        cout << ">>> TXT results saved to: " << OUTPUT_PATH << endl;
    }
    
    void write_output_file() {
        if (OUTPUT_FORMAT == "csv") {
            write_csv_file();
        } else if (OUTPUT_FORMAT == "txt") {
            write_txt_file();
        }
    }
    
    void play_single_game(int rounds, Player& player1, Player& player2, Arena& arena) {
        cout << "\n" << string(60, '=') << endl;
        cout << ">>> GAME STARTED" << endl;
        cout << string(60, '=') << endl;
        cout << "| Player1: " << setw(12) << left << player1.strategy->get_name() 
             << " | Player2: " << setw(12) << left << player2.strategy->get_name() 
             << " | Rounds: " << setw(3) << rounds << " |" << endl;
        arena.print_scores();
        cout << string(60, '-') << endl;
        
        for (int round = 1; round <= rounds; round++) {
            arena.play_round(player1, player2);
        }
        
        cout << string(60, '-') << endl;
        cout << ">>> GAME STATISTICS" << endl;
        cout << string(60, '-') << endl;
        cout << "| " << player1.id << " | Total: " << setw(4) << player1.total_score 
             << " | Average: " << fixed << setprecision(2) << setw(5) 
             << (double)player1.total_score / rounds << " |" << endl;
        cout << "| " << player2.id << " | Total: " << setw(4) << player2.total_score 
             << " | Average: " << fixed << setprecision(2) << setw(5) 
             << (double)player2.total_score / rounds << " |" << endl;
        
        // 记录游戏结果
        auto scores = arena.get_scores();
        log_game_result(player1.strategy->get_name(), player2.strategy->get_name(),
                       rounds, player1.total_score, player2.total_score,
                       scores[0], scores[1], scores[2], scores[3],
                       global_noise.get_noise_level());
    }
    
    void tournament(int rounds, int games, string fixed_strategy_name, Arena& arena) {
        cout << "\n" << string(70, '*') << endl;
        cout << ">>> TOURNAMENT MODE STARTED" << endl;
        cout << string(70, '*') << endl;
        cout << "| Fixed Player: " << setw(12) << left << fixed_strategy_name 
             << " | Rounds/Match: " << setw(3) << rounds 
             << " | Games/Opponent: " << setw(2) << games << " |" << endl;
        arena.print_scores();
        
        Strategy* fixed_strategy = strategy_map[fixed_strategy_name];
        Player fixed_player("Player1", fixed_strategy);
        
        for (Strategy* opp_strategy : strategies) {
            if (opp_strategy->get_name() == fixed_strategy_name) {
                continue;
            }
            
            cout << "\n" << string(50, '~') << endl;
            cout << ">>> VS STRATEGY: " << opp_strategy->get_name() << endl;
            cout << string(50, '~') << endl;
            
            for (int game = 1; game <= games; game++) {
                Player opponent("Player2", opp_strategy);
                fixed_player.reset();
                
                cout << "\n--- Game " << game << " ---" << endl;
                play_single_game(rounds, fixed_player, opponent, arena);
            }
        }
    }
    
    void show_strategies() {
        cout << ">>> Available strategies: ";
        for (Strategy* strategy : strategies) {
            cout << strategy->get_name() << " ";
        }
        cout << endl;
    }
    
    Strategy* get_strategy(const string& name) {
        return strategy_map[name];
    }
    
    ~GameManager() {
        for (Strategy* strategy : strategies) {
            delete strategy;
        }
    }
};

// Function to show help information
void show_help() {
    cout << "\n" << string(70, '=') << endl;
    cout << ">>> PRISONER'S DILEMMA SIMULATOR - HELP MENU" << endl;
    cout << string(70, '=') << endl;
    cout << "COMMAND LINE USAGE:" << endl;
    cout << "  --help                    Show this help menu" << endl;
    cout << "  --p1 STRATEGY             Player 1 strategy (default: ALLC)" << endl;
    cout << "  --p2 STRATEGY             Player 2 strategy (default: ALLD)" << endl;
    cout << "  --rounds NUMBER           Number of rounds per game (default: 3)" << endl;
    cout << "  --games NUMBER            Number of games (default: 5)" << endl;
    cout << "  --tournament STRATEGY     Tournament mode with fixed strategy" << endl;
    cout << "  --list                    List available strategies" << endl;
    cout << "  --scores T,R,P,S          Set custom scores (default: 5,3,1,0)" << endl;
    cout << "  --noise LEVEL             Set noise level 0-1 (default: 0)" << endl;
    cout << "  --format FORMAT           Output format: csv, txt (default: csv)" << endl;
    cout << "                            Rules: T > R > P > S and 2R > T + S" << endl;
    cout << "\nAVAILABLE STRATEGIES:" << endl;
    cout << "  ALLC       - Always Cooperate" << endl;
    cout << "  ALLD       - Always Defect" << endl;
    cout << "  TFT        - Tit for Tat (copy opponent's last move)" << endl;
    cout << "  GrimTrigger- Cooperate until opponent defects, then always defect" << endl;
    cout << "  Pavlov     - Win-Stay Lose-Shift strategy" << endl;
    cout << "  Random     - Random choice (50% cooperate)" << endl;
    cout << "  Probe      - Test opponent then exploit or switch to TFT" << endl;
    cout << "\nEXAMPLES:" << endl;
    cout << "  ./game --p1 TFT --p2 ALLD --rounds 20" << endl;
    cout << "  ./game --tournament GrimTrigger --rounds 10 --games 3" << endl;
    cout << "  ./game --scores 4,3,2,1 --p1 Pavlov --p2 Random" << endl;
    cout << "  ./game --noise 0.2 --format txt" << endl;
    cout << "  ./game                    (Uses default settings)" << endl;
    cout << string(70, '=') << endl;
}

// Function to safely convert string to integer with error handling
int safe_stoi(const string& str, const string& param_name) {
    try {
        int value = stoi(str);
        if (value <= 0) {
            cout << "!!! Error: " << param_name << " must be a positive number" << endl;
            show_help();
            exit(1);
        }
        return value;
    } catch (const invalid_argument& e) {
        cout << "!!! Error: Invalid number for " << param_name << ": " << str << endl;
        show_help();
        exit(1);
    } catch (const out_of_range& e) {
        cout << "!!! Error: Number out of range for " << param_name << ": " << str << endl;
        show_help();
        exit(1);
    }
}

// Function to safely convert string to double with error handling
double safe_stod(const string& str, const string& param_name) {
    try {
        double value = stod(str);
        if (value < 0.0 || value > 1.0) {
            cout << "!!! Error: " << param_name << " must be between 0 and 1" << endl;
            show_help();
            exit(1);
        }
        return value;
    } catch (const invalid_argument& e) {
        cout << "!!! Error: Invalid number for " << param_name << ": " << str << endl;
        show_help();
        exit(1);
    } catch (const out_of_range& e) {
        cout << "!!! Error: Number out of range for " << param_name << ": " << str << endl;
        show_help();
        exit(1);
    }
}

// Function to parse scores from string
vector<int> parse_scores(const string& score_str) {
    vector<int> scores;
    stringstream ss(score_str);
    string item;
    
    while (getline(ss, item, ',')) {
        try {
            scores.push_back(stoi(item));
        } catch (const exception& e) {
            cout << "!!! Error: Invalid score value: " << item << endl;
            show_help();
            exit(1);
        }
    }
    
    if (scores.size() != 4) {
        cout << "!!! Error: Scores must be exactly 4 numbers separated by commas" << endl;
        show_help();
        exit(1);
    }
    
    return scores;
}

// Function to validate scores according to game rules
bool validate_score_rules(int T, int R, int P, int S) {
    if (!(T > R && R > P && P > S)) {
        cout << "!!! Error: Scores must satisfy T > R > P > S" << endl;
        cout << "!!! Your input: T=" << T << " R=" << R << " P=" << P << " S=" << S << endl;
        return false;
    }
    
    if (!(2 * R > T + S)) {
        cout << "!!! Error: Scores must satisfy 2R > T + S" << endl;
        cout << "!!! Your input: 2R=" << (2*R) << " T+S=" << (T+S) << endl;
        return false;
    }
    
    return true;
}

// Function to parse command line arguments
void parse_args(int argc, char* argv[], GameManager& manager) {
    // 设置默认值
    string p1_strategy = "ALLC";
    string p2_strategy = "ALLD";
    int rounds = 3;
    int games = 5;
    string tournament_strategy = "";
    vector<int> custom_scores = {5, 3, 1, 0}; // 默认 T,R,P,S
    double noise_level = 0.0;
    string output_format = "csv";
    
    // 解析命令行参数
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            show_help();
            exit(0);
        }
        else if (strcmp(argv[i], "--list") == 0) {
            manager.show_strategies();
            exit(0);
        }
        else if (strcmp(argv[i], "--p1") == 0) {
            if (i + 1 < argc) {
                p1_strategy = argv[++i];
                if (!manager.get_strategy(p1_strategy)) {
                    cout << "!!! Error: Unknown strategy '" << p1_strategy << "'" << endl;
                    show_help();
                    exit(1);
                }
            } else {
                cout << "!!! Error: --p1 requires a strategy name" << endl;
                show_help();
                exit(1);
            }
        }
        else if (strcmp(argv[i], "--p2") == 0) {
            if (i + 1 < argc) {
                p2_strategy = argv[++i];
                if (!manager.get_strategy(p2_strategy)) {
                    cout << "!!! Error: Unknown strategy '" << p2_strategy << "'" << endl;
                    show_help();
                    exit(1);
                }
            } else {
                cout << "!!! Error: --p2 requires a strategy name" << endl;
                show_help();
                exit(1);
            }
        }
        else if (strcmp(argv[i], "--rounds") == 0) {
            if (i + 1 < argc) {
                rounds = safe_stoi(argv[++i], "--rounds");
            } else {
                cout << "!!! Error: --rounds requires a number" << endl;
                show_help();
                exit(1);
            }
        }
        else if (strcmp(argv[i], "--games") == 0) {
            if (i + 1 < argc) {
                games = safe_stoi(argv[++i], "--games");
            } else {
                cout << "!!! Error: --games requires a number" << endl;
                show_help();
                exit(1);
            }
        }
        else if (strcmp(argv[i], "--tournament") == 0) {
            if (i + 1 < argc) {
                tournament_strategy = argv[++i];
                if (!manager.get_strategy(tournament_strategy)) {
                    cout << "!!! Error: Unknown strategy '" << tournament_strategy << "'" << endl;
                    show_help();
                    exit(1);
                }
            } else {
                cout << "!!! Error: --tournament requires a strategy name" << endl;
                show_help();
                exit(1);
            }
        }
        else if (strcmp(argv[i], "--scores") == 0) {
            if (i + 1 < argc) {
                custom_scores = parse_scores(argv[++i]);
                if (!validate_score_rules(custom_scores[0], custom_scores[1], 
                                        custom_scores[2], custom_scores[3])) {
                    show_help();
                    exit(1);
                }
            } else {
                cout << "!!! Error: --scores requires 4 numbers separated by commas" << endl;
                show_help();
                exit(1);
            }
        }
        else if (strcmp(argv[i], "--noise") == 0) {
            if (i + 1 < argc) {
                noise_level = safe_stod(argv[++i], "--noise");
                global_noise.set_noise_level(noise_level);
            } else {
                cout << "!!! Error: --noise requires a number between 0 and 1" << endl;
                show_help();
                exit(1);
            }
        }
        else if (strcmp(argv[i], "--format") == 0) {
            if (i + 1 < argc) {
                output_format = argv[++i];
                if (output_format != "csv" && output_format != "txt") {
                    cout << "!!! Error: --format must be csv or txt" << endl;
                    show_help();
                    exit(1);
                }
                OUTPUT_FORMAT = output_format;
                OUTPUT_PATH = get_output_path(output_format);
            } else {
                cout << "!!! Error: --format requires csv or txt" << endl;
                show_help();
                exit(1);
            }
        }
        else {
            cout << "!!! Error: Unknown argument: " << argv[i] << endl;
            show_help();
            exit(1);
        }
    }
    
    // 生成识别码（在解析完所有参数后）
    string session_id;
    if (!tournament_strategy.empty()) {
        // 车轮战模式：使用固定策略作为P1，其他策略轮流作为P2
        session_id = generate_unique_id(tournament_strategy, "TOURNAMENT", rounds, games);
    } else {
        // 普通对战模式
        session_id = generate_unique_id(p1_strategy, p2_strategy, rounds, games);
    }
    manager.set_session_id(session_id);
    
    // 无参数时使用默认设置
    if (argc == 1) {
        cout << ">>> No arguments provided. Using default settings:" << endl;
        cout << "| Player1: " << setw(8) << p1_strategy 
             << " | Player2: " << setw(8) << p2_strategy 
             << " | Rounds: " << setw(2) << rounds 
             << " | Games: " << setw(2) << games << " |" << endl;
        cout << "| Scores: T=" << custom_scores[0] << " R=" << custom_scores[1] 
             << " P=" << custom_scores[2] << " S=" << custom_scores[3] << " |" << endl;
        cout << "| Noise Level: " << fixed << setprecision(2) << noise_level << " |" << endl;
        cout << "| Output Format: " << output_format << " |" << endl;
        cout << ">>> Session ID: " << session_id << endl;
        cout << ">>> Output file: " << OUTPUT_PATH << endl;
        cout << string(60, '-') << endl;
    } else {
        cout << ">>> Session ID: " << session_id << endl;
        cout << ">>> Output file: " << OUTPUT_PATH << endl;
    }
    
    // 创建竞技场并验证分数
    Arena arena(custom_scores[0], custom_scores[1], custom_scores[2], custom_scores[3]);
    if (!arena.validate_scores()) {
        cout << "!!! Error: Invalid score configuration" << endl;
        show_help();
        exit(1);
    }
    
    // 执行游戏逻辑
    if (!tournament_strategy.empty()) {
        cout << ">>> Running tournament with provided settings..." << endl;
        manager.tournament(rounds, games, tournament_strategy, arena);
    }
    else {
        Strategy* strat1 = manager.get_strategy(p1_strategy);
        Strategy* strat2 = manager.get_strategy(p2_strategy);
        
        for (int game = 1; game <= games; game++) {
            Player player1("Player1", strat1);
            Player player2("Player2", strat2);
            cout << "\n=== Game " << game << " ===" << endl;
            manager.play_single_game(rounds, player1, player2, arena);
        }
    }
}

int main(int argc, char* argv[]) {
    // 创建输出目录
    create_output_directories();
    
    GameManager manager;
    
    cout << string(50, '=') << endl;
    cout << ">>> PRISONER'S DILEMMA SIMULATOR" << endl;
    cout << string(50, '=') << endl;
    
    try {
        // 解析参数并执行游戏
        parse_args(argc, argv, manager);
        
        // 写入输出文件
        manager.write_output_file();
        
    } catch (const exception& e) {
        cout << "!!! Unexpected error: " << e.what() << endl;
        // 即使出错也尝试写入已记录的数据
        manager.write_output_file();
        show_help();
        return 1;
    }
    
    // 程序结束时总是显示帮助信息
    cout << "\n";
    show_help();
    
    return 0;
}
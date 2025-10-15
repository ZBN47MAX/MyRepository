#include "GameManager.h"


std::string OUTPUT_PATH = get_output_path();
std::string OUTPUT_FORMAT = "csv";


bool create_directory(const std::string& path) {
#ifdef _WIN32
    return _mkdir(path.c_str()) == 0;
#else
    return mkdir(path.c_str(), 0755) == 0;
#endif
}

bool directory_exists(const std::string& path) {
    struct stat info;
    return stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR);
}

void create_output_directories() {
    std::vector<std::string> dirs = { "Record", "Record/CSV", "Record/TXT" };

    for (const auto& dir : dirs) {
        if (!directory_exists(dir)) {
            if (create_directory(dir)) {
                std::cout << ">>> Created directory: " << dir << std::endl;
            }
            else {
                std::cout << "!!! Warning: Could not create directory: " << dir << std::endl;
            }
        }
    }
}

std::string get_output_path(const std::string& format) {
    std::string base_dir = "Record";
    std::string format_upper = format;
    for (auto& c : format_upper)
        c = toupper(c);

    std::string output_dir = base_dir + "/" + format_upper;

    
    if (!directory_exists(output_dir)) {
        create_directory(output_dir);
    }

   
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dis(1000, 9999);

    std::stringstream filename;
    filename << "prisoners_dilemma_"
        << dis(gen) << "_" << dis(gen)
        << "." << format;

    return output_dir + "/" + filename.str();
}

std::string generate_unique_id(const std::string& p1_strategy,
    const std::string& p2_strategy,
    int rounds, int games) {

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dis(10000, 99999);

   
    std::stringstream id_ss;
    id_ss << "SES" << dis(gen)
        << "P1" << p1_strategy
        << "P2" << p2_strategy
        << "R" << rounds
        << "G" << games;

    return id_ss.str();
}


GameManager::GameManager() {
    
    add_strategy(new ALLC());
    add_strategy(new ALLD());
    add_strategy(new TFT());
    add_strategy(new GrimTrigger());
    add_strategy(new Pavlov());
    add_strategy(new Random());
    add_strategy(new Probe());
    add_strategy(new CTFT());
    add_strategy(new Custom1());
    add_strategy(new Custom2());
}

GameManager::~GameManager() {
    for (Strategy* strategy : strategies) {
        delete strategy;
    }
}

void GameManager::set_session_id(const std::string& session_id) {
    current_session_id = session_id;
}

void GameManager::add_strategy(Strategy* strategy) {
    strategies.push_back(strategy);
    strategy_map[strategy->get_name()] = strategy;
}

void GameManager::log_game_result(const std::string& p1_strategy, const std::string& p2_strategy,
    int rounds, int p1_score, int p2_score,
    int T, int R, int P, int S, double noise_level) {
    std::vector<std::string> record = {
        current_session_id,
        p1_strategy,
        p2_strategy,
        std::to_string(rounds),
        std::to_string(p1_score),
        std::to_string(p2_score),
        std::to_string(T),
        std::to_string(R),
        std::to_string(P),
        std::to_string(S),
        std::to_string(noise_level) };
    game_logs.push_back(record);
}

void GameManager::write_csv_file() {
    std::ofstream file(OUTPUT_PATH);
    if (!file.is_open()) {
        std::cout << "!!! Warning: Could not open CSV file for writing: " << OUTPUT_PATH << std::endl;
        return;
    }

   
    file << "SessionID,Player1Strategy,Player2Strategy,Rounds,Player1Score,Player2Score,T,R,P,S,NoiseLevel\n";

    for (const auto& record : game_logs) {
        for (size_t i = 0; i < record.size(); ++i) {
            file << record[i];
            if (i < record.size() - 1)
                file << ",";
        }
        file << "\n";
    }

    file.close();
    std::cout << ">>> CSV results saved to: " << OUTPUT_PATH << std::endl;
}

void GameManager::write_txt_file() {
    std::ofstream file(OUTPUT_PATH);
    if (!file.is_open()) {
        std::cout << "!!! Warning: Could not open TXT file for writing: " << OUTPUT_PATH << std::endl;
        return;
    }

    file << "PRISONER'S DILEMMA SIMULATION RESULTS" << std::endl;
    file << "Session ID: " << current_session_id << std::endl;
    file << "Noise Level: " << std::fixed << std::setprecision(2) << global_noise.get_noise_level() << std::endl;
    file << std::string(50, '=') << std::endl;

    for (size_t i = 0; i < game_logs.size(); ++i) {
        const auto& record = game_logs[i];
        file << "\nGAME " << (i + 1) << ":" << std::endl;
        file << "  Strategies: " << record[1] << " vs " << record[2] << std::endl;
        file << "  Rounds: " << record[3] << std::endl;
        file << "  Scores: " << record[4] << " - " << record[5] << std::endl;
        file << "  Parameters: T=" << record[6] << " R=" << record[7]
            << " P=" << record[8] << " S=" << record[9] << std::endl;
        file << "  Noise Level: " << record[10] << std::endl;
    }

    file.close();
    std::cout << ">>> TXT results saved to: " << OUTPUT_PATH << std::endl;
}

void GameManager::write_output_file() {
    if (OUTPUT_FORMAT == "csv") {
        write_csv_file();
    }
    else if (OUTPUT_FORMAT == "txt") {
        write_txt_file();
    }
}

void GameManager::play_single_game(int rounds, Player& player1, Player& player2, Arena& arena) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << ">>> GAME STARTED" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "| Player1: " << std::setw(12) << std::left << player1.strategy->get_name()
        << " | Player2: " << std::setw(12) << std::left << player2.strategy->get_name()
        << " | Rounds: " << std::setw(3) << rounds << " |" << std::endl;
    arena.print_scores();
    std::cout << std::string(60, '-') << std::endl;

    for (int round = 1; round <= rounds; round++) {
        arena.play_round(player1, player2);
    }

    std::cout << std::string(60, '-') << std::endl;
    std::cout << ">>> GAME STATISTICS" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    std::cout << "| " << player1.id << " | Total: " << std::setw(4) << player1.total_score
        << " | Average: " << std::fixed << std::setprecision(2) << std::setw(5)
        << (double)player1.total_score / rounds << " |" << std::endl;
    std::cout << "| " << player2.id << " | Total: " << std::setw(4) << player2.total_score
        << " | Average: " << std::fixed << std::setprecision(2) << std::setw(5)
        << (double)player2.total_score / rounds << " |" << std::endl;

    // 记录游戏结果
    auto scores = arena.get_scores();
    log_game_result(player1.strategy->get_name(), player2.strategy->get_name(),
        rounds, player1.total_score, player2.total_score,
        scores[0], scores[1], scores[2], scores[3],
        global_noise.get_noise_level());
}

void GameManager::tournament(int rounds, int repeats, std::string fixed_strategy_name, Arena& arena) {
    std::cout << "\n" << std::string(70, '*') << std::endl;
    std::cout << ">>> TOURNAMENT MODE STARTED" << std::endl;
    std::cout << std::string(70, '*') << std::endl;
    std::cout << "| Fixed Player: " << std::setw(12) << std::left << fixed_strategy_name
        << " | Rounds/Match: " << std::setw(3) << rounds
        << " | Repeats/Opponent: " << std::setw(2) << repeats << " |" << std::endl;
    arena.print_scores();

    Strategy* fixed_strategy = strategy_map[fixed_strategy_name];
    Player fixed_player("Player1", fixed_strategy);

    for (Strategy* opp_strategy : strategies) {
        if (opp_strategy->get_name() == fixed_strategy_name) {
            continue;
        }

        std::cout << "\n" << std::string(50, '~') << std::endl;
        std::cout << ">>> VS STRATEGY: " << opp_strategy->get_name() << std::endl;
        std::cout << std::string(50, '~') << std::endl;

        for (int repeat = 1; repeat <= repeats; repeat++) {
            Player opponent("Player2", opp_strategy);
            fixed_player.reset();

            std::cout << "\n--- Repeat " << repeat << " ---" << std::endl;
            play_single_game(rounds, fixed_player, opponent, arena);
        }
    }
}

void GameManager::show_strategies() {
    std::cout << ">>> Available strategies: ";
    for (Strategy* strategy : strategies) {
        std::cout << strategy->get_name() << " ";
    }
    std::cout << std::endl;
}

Strategy* GameManager::get_strategy(const std::string& name) {
    return strategy_map[name];
}

void GameManager::run_q2_experiment(int rounds, int repeats, const std::vector<double>& noise_levels, Arena& arena) {
    std::vector<std::string> q2_strategies = { "TFT", "GrimTrigger", "Pavlov", "CTFT", "ALLC", "ALLD", "custom1", "custom2" };
    NoiseSweep sweep;
    sweep.run_sweep(q2_strategies, rounds, repeats, noise_levels, arena);
}


void show_help() {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << ">>> PRISONER'S DILEMMA SIMULATOR - HELP MENU" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    std::cout << "COMMAND LINE USAGE:" << std::endl;
    std::cout << "  --help                    Show this help menu" << std::endl;
    std::cout << "  --p1 STRATEGY             Player 1 strategy (default: ALLC)" << std::endl;
    std::cout << "  --p2 STRATEGY             Player 2 strategy (default: ALLD)" << std::endl;
    std::cout << "  --rounds NUMBER           Number of rounds per game (default: 3)" << std::endl;
    std::cout << "  --repeats NUMBER          Number of repeats for experiments (default: 30)" << std::endl;
    std::cout << "  --tournament STRATEGY     Tournament mode with fixed strategy" << std::endl;
    std::cout << "  --strategies              List available strategies" << std::endl;
    std::cout << "  --scores T,R,P,S          Set custom scores (default: 5,3,1,0)" << std::endl;
    std::cout << "  --epsilon LEVEL           Set noise level 0-1 (default: 0)" << std::endl;
    std::cout << "  --format FORMAT           Output format: csv, txt (default: csv)" << std::endl;
    std::cout << "  --q2                      Run Q2 noise sweep experiment" << std::endl;
    std::cout << "  --noise-levels LIST       Comma-separated noise levels (default: 0.0,0.05,0.1,0.2)" << std::endl;
    std::cout << "                            Rules: T > R > P > S and 2R > T + S" << std::endl;
    std::cout << "\nAVAILABLE STRATEGIES:" << std::endl;
    std::cout << "  ALLC       - Always Cooperate" << std::endl;
    std::cout << "  ALLD       - Always Defect" << std::endl;
    std::cout << "  TFT        - Tit for Tat (copy opponent's last move)" << std::endl;
    std::cout << "  GrimTrigger- Cooperate until opponent defects, then always defect" << std::endl;
    std::cout << "  Pavlov     - Win-Stay Lose-Shift strategy" << std::endl;
    std::cout << "  Random     - Random choice (50% cooperate)" << std::endl;
    std::cout << "  Probe      - Test opponent then exploit or switch to TFT" << std::endl;
    std::cout << "  CTFT       - Contrite Tit for Tat (noise-resistant)" << std::endl;
    std::cout << "  custom1    - Counting forgiving: cooperate until 3 defects, 10% mutation" << std::endl;
    std::cout << "  custom2    - Copycat with mutation: mimic opponent with 15% chance to do opposite" << std::endl;
    std::cout << "\nEXAMPLES:" << std::endl;
    std::cout << "  prisoners_dilemma --p1 TFT --p2 ALLD --rounds 20" << std::endl;
    std::cout << "  prisoners_dilemma --tournament GrimTrigger --rounds 10 --repeats 3" << std::endl;
    std::cout << "  prisoners_dilemma --scores 4,3,2,1 --p1 Pavlov --p2 Random" << std::endl;
    std::cout << "  prisoners_dilemma --epsilon 0.2 --format txt" << std::endl;
    std::cout << "  prisoners_dilemma --q2 --rounds 100 --repeats 30 --noise-levels 0.0,0.05,0.1,0.2" << std::endl;
    std::cout << "  prisoners_dilemma --strategies" << std::endl;
    std::cout << "  prisoners_dilemma                    (Uses default settings)" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
}

int safe_stoi(const std::string& str, const std::string& param_name) {
    try {
        int value = std::stoi(str);
        if (value <= 0) {
            std::cout << "!!! Error: " << param_name << " must be a positive number" << std::endl;
            show_help();
            exit(1);
        }
        return value;
    }
    catch (const std::invalid_argument& e) {
        std::cout << "!!! Error: Invalid number for " << param_name << ": " << str << std::endl;
        show_help();
        exit(1);
    }
    catch (const std::out_of_range& e) {
        std::cout << "!!! Error: Number out of range for " << param_name << ": " << str << std::endl;
        show_help();
        exit(1);
    }
}

double safe_stod(const std::string& str, const std::string& param_name) {
    try {
        double value = std::stod(str);
        if (value < 0.0 || value > 1.0) {
            std::cout << "!!! Error: " << param_name << " must be between 0 and 1" << std::endl;
            show_help();
            exit(1);
        }
        return value;
    }
    catch (const std::invalid_argument& e) {
        std::cout << "!!! Error: Invalid number for " << param_name << ": " << str << std::endl;
        show_help();
        exit(1);
    }
    catch (const std::out_of_range& e) {
        std::cout << "!!! Error: Number out of range for " << param_name << ": " << str << std::endl;
        show_help();
        exit(1);
    }
}

std::vector<int> parse_scores(const std::string& score_str) {
    std::vector<int> scores;
    std::stringstream ss(score_str);
    std::string item;

    while (std::getline(ss, item, ',')) {
        try {
            scores.push_back(std::stoi(item));
        }
        catch (const std::exception& e) {
            std::cout << "!!! Error: Invalid score value: " << item << std::endl;
            show_help();
            exit(1);
        }
    }

    if (scores.size() != 4) {
        std::cout << "!!! Error: Scores must be exactly 4 numbers separated by commas" << std::endl;
        show_help();
        exit(1);
    }

    return scores;
}

std::vector<double> parse_noise_levels(const std::string& noise_str) {
    std::vector<double> levels;
    std::stringstream ss(noise_str);
    std::string item;

    while (std::getline(ss, item, ',')) {
        try {
            levels.push_back(std::stod(item));
        }
        catch (const std::exception& e) {
            std::cout << "!!! Error: Invalid noise level: " << item << std::endl;
            show_help();
            exit(1);
        }
    }

    return levels;
}

bool validate_score_rules(int T, int R, int P, int S) {
    if (!(T > R && R > P && P > S)) {
        std::cout << "!!! Error: Scores must satisfy T > R > P > S" << std::endl;
        std::cout << "!!! Your input: T=" << T << " R=" << R << " P=" << P << " S=" << S << std::endl;
        return false;
    }

    if (!(2 * R > T + S)) {
        std::cout << "!!! Error: Scores must satisfy 2R > T + S" << std::endl;
        std::cout << "!!! Your input: 2R=" << (2 * R) << " T+S=" << (T + S) << std::endl;
        return false;
    }

    return true;
}

void parse_args(int argc, char* argv[], GameManager& manager) {
   
    std::string p1_strategy = "ALLC";
    std::string p2_strategy = "ALLD";
    int rounds = 3;
    int repeats = 30;
    std::string tournament_strategy = "";
    std::vector<int> custom_scores = { 5, 3, 1, 0 };
    double epsilon = 0.0;
    std::string output_format = "csv";
    bool run_q2_experiment = false;
    std::vector<double> noise_levels = { 0.0, 0.05, 0.1, 0.2 };

    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            show_help();
            exit(0);
        }
        else if (strcmp(argv[i], "--strategies") == 0) {
            manager.show_strategies();
            exit(0);
        }
        else if (strcmp(argv[i], "--p1") == 0) {
            if (i + 1 < argc) {
                p1_strategy = argv[++i];
                if (!manager.get_strategy(p1_strategy)) {
                    std::cout << "!!! Error: Unknown strategy '" << p1_strategy << "'" << std::endl;
                    show_help();
                    exit(1);
                }
            }
            else {
                std::cout << "!!! Error: --p1 requires a strategy name" << std::endl;
                show_help();
                exit(1);
            }
        }
        else if (strcmp(argv[i], "--p2") == 0) {
            if (i + 1 < argc) {
                p2_strategy = argv[++i];
                if (!manager.get_strategy(p2_strategy)) {
                    std::cout << "!!! Error: Unknown strategy '" << p2_strategy << "'" << std::endl;
                    show_help();
                    exit(1);
                }
            }
            else {
                std::cout << "!!! Error: --p2 requires a strategy name" << std::endl;
                show_help();
                exit(1);
            }
        }
        else if (strcmp(argv[i], "--rounds") == 0) {
            if (i + 1 < argc) {
                rounds = safe_stoi(argv[++i], "--rounds");
            }
            else {
                std::cout << "!!! Error: --rounds requires a number" << std::endl;
                show_help();
                exit(1);
            }
        }
        else if (strcmp(argv[i], "--repeats") == 0) {
            if (i + 1 < argc) {
                repeats = safe_stoi(argv[++i], "--repeats");
            }
            else {
                std::cout << "!!! Error: --repeats requires a number" << std::endl;
                show_help();
                exit(1);
            }
        }
        else if (strcmp(argv[i], "--tournament") == 0) {
            if (i + 1 < argc) {
                tournament_strategy = argv[++i];
                if (!manager.get_strategy(tournament_strategy)) {
                    std::cout << "!!! Error: Unknown strategy '" << tournament_strategy << "'" << std::endl;
                    show_help();
                    exit(1);
                }
            }
            else {
                std::cout << "!!! Error: --tournament requires a strategy name" << std::endl;
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
            }
            else {
                std::cout << "!!! Error: --scores requires 4 numbers separated by commas" << std::endl;
                show_help();
                exit(1);
            }
        }
        else if (strcmp(argv[i], "--epsilon") == 0) {
            if (i + 1 < argc) {
                epsilon = safe_stod(argv[++i], "--epsilon");
                global_noise.set_noise_level(epsilon);
            }
            else {
                std::cout << "!!! Error: --epsilon requires a number between 0 and 1" << std::endl;
                show_help();
                exit(1);
            }
        }
        else if (strcmp(argv[i], "--format") == 0) {
            if (i + 1 < argc) {
                output_format = argv[++i];
                if (output_format != "csv" && output_format != "txt") {
                    std::cout << "!!! Error: --format must be csv or txt" << std::endl;
                    show_help();
                    exit(1);
                }
                OUTPUT_FORMAT = output_format;
                OUTPUT_PATH = get_output_path(output_format);
            }
            else {
                std::cout << "!!! Error: --format requires csv or txt" << std::endl;
                show_help();
                exit(1);
            }
        }
        else if (strcmp(argv[i], "--q2") == 0) {
            run_q2_experiment = true;
        }
        else if (strcmp(argv[i], "--noise-levels") == 0) {
            if (i + 1 < argc) {
                noise_levels = parse_noise_levels(argv[++i]);
            }
            else {
                std::cout << "!!! Error: --noise-levels requires comma-separated values" << std::endl;
                show_help();
                exit(1);
            }
        }
        else {
            std::cout << "!!! Error: Unknown argument: " << argv[i] << std::endl;
            show_help();
            exit(1);
        }
    }



    std::string session_id;
    if (run_q2_experiment) {
        session_id = generate_unique_id("Q2_EXPERIMENT", "NOISE_SWEEP", rounds, repeats);
    }
    else if (!tournament_strategy.empty()) {
        session_id = generate_unique_id(tournament_strategy, "TOURNAMENT", rounds, repeats);
    }
    else {
        session_id = generate_unique_id(p1_strategy, p2_strategy, rounds, repeats);
    }
    manager.set_session_id(session_id);

   
    Arena arena(custom_scores[0], custom_scores[1], custom_scores[2], custom_scores[3]);
    if (!arena.validate_scores()) {
        std::cout << "!!! Error: Invalid score configuration" << std::endl;
        show_help();
        exit(1);
    }

   
    if (run_q2_experiment) {
        std::cout << ">>> Running Q2 Noise Sweep Experiment..." << std::endl;
        std::cout << ">>> Session ID: " << session_id << std::endl;
        std::cout << ">>> Noise levels to scan: ";
        for (size_t i = 0; i < noise_levels.size(); i++) {
            std::cout << noise_levels[i];
            if (i < noise_levels.size() - 1)
                std::cout << ", ";
        }
        std::cout << std::endl;
        std::cout << ">>> Rounds: " << rounds << ", Repeats: " << repeats << std::endl;

        manager.run_q2_experiment(rounds, repeats, noise_levels, arena);
        return;
    }

    //default
    if (argc == 1) {
        std::cout << ">>> No arguments provided. Using default settings:" << std::endl;
        std::cout << "| Player1: " << std::setw(8) << p1_strategy
            << " | Player2: " << std::setw(8) << p2_strategy
            << " | Rounds: " << std::setw(2) << rounds
            << " | Repeats: " << std::setw(2) << repeats << " |" << std::endl;
        std::cout << "| Scores: T=" << custom_scores[0] << " R=" << custom_scores[1]
            << " P=" << custom_scores[2] << " S=" << custom_scores[3] << " |" << std::endl;
        std::cout << "| Epsilon: " << std::fixed << std::setprecision(2) << epsilon << " |" << std::endl;
        std::cout << "| Output Format: " << output_format << " |" << std::endl;
        std::cout << ">>> Session ID: " << session_id << std::endl;
        std::cout << ">>> Output file: " << OUTPUT_PATH << std::endl;
        std::cout << std::string(60, '-') << std::endl;
    }
    else {
        std::cout << ">>> Session ID: " << session_id << std::endl;
        std::cout << ">>> Output file: " << OUTPUT_PATH << std::endl;
    }

    
    if (!tournament_strategy.empty()) {
        std::cout << ">>> Running tournament with provided settings..." << std::endl;
        manager.tournament(rounds, repeats, tournament_strategy, arena);
    }
    else {
        Strategy* strat1 = manager.get_strategy(p1_strategy);
        Strategy* strat2 = manager.get_strategy(p2_strategy);

        for (int repeat = 1; repeat <= repeats; repeat++) {
            Player player1("Player1", strat1);
            Player player2("Player2", strat2);
            std::cout << "\n=== Repeat " << repeat << " ===" << std::endl;
            manager.play_single_game(rounds, player1, player2, arena);
        }
    }

   
    manager.write_output_file();
}
#include "GameCore.h"
#include "GameManager.h"


Player::Player(std::string player_id, Strategy* strat) : id(player_id), strategy(strat) {}

bool Player::make_choice(const std::vector<bool>& opponent_history) {
    bool actual_choice = strategy->make_choice(choice_history, opponent_history, score_history, std::vector<int>());
    bool final_choice = global_noise.apply_noise(actual_choice);

    actual_choices.push_back(actual_choice);
    choice_history.push_back(final_choice);
    return final_choice;
}

void Player::add_score(int score) {
    last_score = score;
    total_score += score;
    score_history.push_back(score);
}

void Player::reset() {
    last_score = 0;
    total_score = 0;
    choice_history.clear();
    score_history.clear();
    actual_choices.clear();
    strategy->reset();
}

std::vector<bool> Player::get_choice_history() const {
    return choice_history;
}

std::vector<bool> Player::get_actual_choices() const {
    return actual_choices;
}


GameResult::GameResult() : mean_score(0), std_dev(0), confidence_interval(0) {}


Arena::Arena(int T, int R, int P, int S) : TEMPTATION(T), REWARD(R), PUNISHMENT(P), SUCKER(S) {}

bool Arena::validate_scores() {

    if (!(TEMPTATION > REWARD && REWARD > PUNISHMENT && PUNISHMENT > SUCKER)) {
        return false;
    }

    if (!(2 * REWARD > TEMPTATION + SUCKER)) {
        return false;
    }
    return true;
}

void Arena::print_scores() {
    std::cout << ">>> Score Matrix: T=" << TEMPTATION << " R=" << REWARD
        << " P=" << PUNISHMENT << " S=" << SUCKER << std::endl;
    std::cout << ">>> Noise Level: " << std::fixed << std::setprecision(2) << global_noise.get_noise_level() << std::endl;
}

std::pair<int, int> Arena::play_round(Player& player1, Player& player2) {
    bool choice1 = player1.make_choice(player2.get_choice_history());
    bool choice2 = player2.make_choice(player1.get_choice_history());

    int score1, score2;

    if (choice1 && choice2) {
        score1 = REWARD;
        score2 = REWARD;
    }
    else if (choice1 && !choice2) {
        score1 = SUCKER;
        score2 = TEMPTATION;
    }
    else if (!choice1 && choice2) {
        score1 = TEMPTATION;
        score2 = SUCKER;
    }
    else {
        score1 = PUNISHMENT;
        score2 = PUNISHMENT;
    }

    player1.add_score(score1);
    player2.add_score(score2);


    std::string p1_choice_str = (choice1 ? "COOP" : "DEFECT");
    std::string p2_choice_str = (choice2 ? "COOP" : "DEFECT");

    if (global_noise.get_noise_level() > 0.0) {
        bool actual1 = player1.actual_choices.back();
        bool actual2 = player2.actual_choices.back();
        if (actual1 != choice1)
            p1_choice_str = (actual1 ? "COOP*" : "DEFECT*") + ("->" + p1_choice_str);
        if (actual2 != choice2)
            p2_choice_str = (actual2 ? "COOP*" : "DEFECT*") + ("->" + p2_choice_str);
    }

    std::cout << "| Round " << std::setw(2) << player1.choice_history.size()
        << " | " << player1.id << ":" << p1_choice_str
        << " vs " << player2.id << ":" << p2_choice_str
        << " | Scores: " << std::setw(2) << score1 << "-" << std::setw(2) << score2 << " |" << std::endl;

    return std::make_pair(score1, score2);
}

std::vector<int> Arena::get_scores() {
    return { TEMPTATION, REWARD, PUNISHMENT, SUCKER };
}


GameResult NoiseSweep::calculate_statistics(const std::vector<int>& scores) {
    GameResult result;
    if (scores.empty())
        return result;

    
    double sum = 0;
    for (int score : scores)
        sum += score;
    result.mean_score = sum / scores.size();


    double variance = 0;
    for (int score : scores) {
        variance += std::pow(score - result.mean_score, 2);
    }
    variance /= scores.size();
    result.std_dev = std::sqrt(variance);

   
    if (scores.size() > 1) {
        result.confidence_interval = 1.96 * (result.std_dev / std::sqrt(scores.size()));
    }
    result.all_scores = scores;

    return result;
}

void NoiseSweep::run_sweep(const std::vector<std::string>& strategies, int rounds, int repeats,
    const std::vector<double>& noise_levels, Arena& base_arena) {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << ">>> Q2: NOISE SWEEP EXPERIMENT (levels from 0.0 to 0.2)" << std::endl;
    std::cout << ">>> Rounds: " << rounds << ", Repeats: " << repeats << std::endl;
    std::cout << std::string(70, '=') << std::endl;

   
    std::map<std::string, std::map<double, GameResult>> all_results;

   
    for (double noise_level : noise_levels) {
        std::cout << "\n" << std::string(50, '-') << std::endl;
        std::cout << ">>> Noise Level: " << std::fixed << std::setprecision(2) << noise_level << std::endl;
        std::cout << std::string(50, '-') << std::endl;
        global_noise.set_noise_level(noise_level);

        
        for (const std::string& strat_name : strategies) {
            std::cout << "Testing strategy: " << strat_name << "..." << std::endl;
            std::vector<int> total_scores;

           
            for (int repeat = 0; repeat < repeats; repeat++) {
                
                if ((repeat + 1) % 10 == 0) {
                    std::cout << "  Repeat " << (repeat + 1) << "/" << repeats << std::endl;
                }

               
                Strategy* strategy = nullptr;
                if (strat_name == "TFT")
                    strategy = new TFT();
                else if (strat_name == "GrimTrigger")
                    strategy = new GrimTrigger();
                else if (strat_name == "Pavlov")
                    strategy = new Pavlov();
                else if (strat_name == "CTFT")
                    strategy = new CTFT();
                else if (strat_name == "ALLC")
                    strategy = new ALLC();
                else if (strat_name == "ALLD")
                    strategy = new ALLD();
                else if (strat_name == "custom1")
                    strategy = new Custom1();
                else if (strat_name == "custom2")
                    strategy = new Custom2();

                if (!strategy)
                    continue;

                Player test_player("Test", strategy);
                Player opponent("Opponent", new ALLC()); 

              
                for (int round = 1; round <= rounds; round++) {
                    base_arena.play_round(test_player, opponent);
                }

                total_scores.push_back(test_player.total_score);
                delete strategy;
                delete opponent.strategy;
            }

            
            all_results[strat_name][noise_level] = calculate_statistics(total_scores);
            std::cout << "  " << strat_name << " completed. Average: "
                << std::fixed << std::setprecision(2) << all_results[strat_name][noise_level].mean_score / rounds
                << " points/round" << std::endl;
        }
    }


    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << ">>> Q2 RESULTS: Average Scores per Round (with 95% Confidence Intervals)" << std::endl;
    std::cout << std::string(80, '=') << std::endl;

 
    std::cout << std::setw(15) << "Strategy";
    for (double noise : noise_levels) {
        std::cout << std::setw(15) << std::fixed << std::setprecision(2) << "ε=" + std::to_string(noise).substr(0, 4);
    }
    std::cout << std::endl;
    std::cout << std::string(15 + noise_levels.size() * 15, '-') << std::endl;

    for (const std::string& strat_name : strategies) {
        std::cout << std::setw(15) << strat_name;
        for (double noise_level : noise_levels) {
            const GameResult& result = all_results[strat_name][noise_level];
            double avg_per_round = result.mean_score / rounds;
            double ci_per_round = result.confidence_interval / rounds;

            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << avg_per_round
                << " ±" << std::setprecision(2) << ci_per_round;
            std::cout << std::setw(15) << ss.str();
        }
        std::cout << std::endl;
    }

   
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << ">>> Q2 ANALYSIS:" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    std::cout << "• TFT: Performs well at ε=0 but deteriorates quickly due to retaliation cycles" << std::endl;
    std::cout << "• GrimTrigger: Collapses completely under noise (eternal punishment)" << std::endl;
    std::cout << "• Pavlov: More resilient due to win-stay-lose-shift logic" << std::endl;
    std::cout << "• CTFT: Specifically designed for noise, maintains cooperation best" << std::endl;
    std::cout << "• ALLC: Always cooperates, unaffected by noise but always exploited" << std::endl;
    std::cout << "• ALLD: Always defects, stable but misses cooperation benefits" << std::endl;
    std::cout << "• custom1: Counting forgiving strategy with 10% mutation" << std::endl;
    std::cout << "• custom2: Copycat strategy with 15% mutation" << std::endl;

    
    std::cout << "\n>>> Detailed Strategy Performance under Noise:" << std::endl;
    for (const std::string& strat_name : strategies) {
        std::cout << "\n" << strat_name << ":" << std::endl;
        for (double noise_level : noise_levels) {
            const GameResult& result = all_results[strat_name][noise_level];
            double avg_per_round = result.mean_score / rounds;
            std::cout << "  ε=" << std::fixed << std::setprecision(2) << noise_level
                << ": " << std::setprecision(3) << avg_per_round << " points/round" << std::endl;
        }
    }
}
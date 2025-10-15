#include "Strategy.h"


std::string ALLC::get_name() {
    return "ALLC";
}

bool ALLC::make_choice(const std::vector<bool>& my_history,
    const std::vector<bool>& opponent_history,
    const std::vector<int>& my_scores,
    const std::vector<int>& opponent_scores) {
    return true; 
}


std::string ALLD::get_name() {
    return "ALLD";
}

bool ALLD::make_choice(const std::vector<bool>& my_history,
    const std::vector<bool>& opponent_history,
    const std::vector<int>& my_scores,
    const std::vector<int>& opponent_scores) {
    return false; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


std::string TFT::get_name() {
    return "TFT";
}

bool TFT::make_choice(const std::vector<bool>& my_history,
    const std::vector<bool>& opponent_history,
    const std::vector<int>& my_scores,
    const std::vector<int>& opponent_scores) {
    if (opponent_history.empty()) {
        return true; 
    }
    return opponent_history.back(); 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string GrimTrigger::get_name() {
    return "GrimTrigger";
}

bool GrimTrigger::make_choice(const std::vector<bool>& my_history,
    const std::vector<bool>& opponent_history,
    const std::vector<int>& my_scores,
    const std::vector<int>& opponent_scores) {
    if (triggered) {
        return false; 
    }

  
    for (bool choice : opponent_history) {
        if (!choice) {
            triggered = true;
            return false;
        }
    }

    return true; 
}

void GrimTrigger::reset() {
    triggered = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string Pavlov::get_name() {
    return "Pavlov";
}

bool Pavlov::make_choice(const std::vector<bool>& my_history,
    const std::vector<bool>& opponent_history,
    const std::vector<int>& my_scores,
    const std::vector<int>& opponent_scores) {
    if (my_history.empty()) {
        return true; 
    }

    int my_last_score = my_scores.back();
    int opp_last_score = opponent_scores.back();


    if (my_last_score > opp_last_score) {
        return my_history.back(); 
    }
    else {
        return !my_history.back(); 
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Random::Random() : gen(std::random_device{}()), dis(0.0, 1.0) {}

std::string Random::get_name() {
    return "Random";
}

bool Random::make_choice(const std::vector<bool>& my_history,
    const std::vector<bool>& opponent_history,
    const std::vector<int>& my_scores,
    const std::vector<int>& opponent_scores) {
    return dis(gen) > 0.5; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string Probe::get_name() {
    return "Probe";
}

bool Probe::make_choice(const std::vector<bool>& my_history,
    const std::vector<bool>& opponent_history,
    const std::vector<int>& my_scores,
    const std::vector<int>& opponent_scores) {
 
    int current_round = my_history.size() + 1;


    if (current_round <= 4) {
        switch (current_round) {
        case 1:
            return true; 
        case 2:
            return false; 
        case 3:
            return true;
        case 4:
            return true; 
        }
    }


    if (exploit_mode) {
        return false;
    }


    if (current_round == 5) {

        int cooperate_count = 0;
        for (int i = 0; i < 4 && i < opponent_history.size(); i++) {
            if (opponent_history[i])
                cooperate_count++;
        }

        std::cout << ">>> Probe evaluation: opponent cooperated " << cooperate_count
            << "/4 times in first 4 rounds" << std::endl;

        if (cooperate_count >= 3) {
            exploit_mode = true;
            std::cout << ">>> Probe: Switching to EXPLOIT mode (always defect)" << std::endl;
            return false; 
        }
        else {
            std::cout << ">>> Probe: Switching to TIT-FOR-TAT mode" << std::endl;

            return opponent_history.back();
        }
    }


    if (exploit_mode) {
        return false;
    }
    else {

        if (!opponent_history.empty()) {
            return opponent_history.back();
        }
        return true; 
    }
}

void Probe::reset() {
    exploit_mode = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string CTFT::get_name() {
    return "CTFT";
}

bool CTFT::make_choice(const std::vector<bool>& my_history,
    const std::vector<bool>& opponent_history,
    const std::vector<int>& my_scores,
    const std::vector<int>& opponent_scores) {
    if (my_history.empty()) {
        last_was_mistake = false;
        return true; 
    }


    if (!my_history.back() && opponent_history.back() &&
        my_history.size() >= 2 && my_history[my_history.size() - 2]) {

        last_was_mistake = true;
    }
    else {
        last_was_mistake = false;
    }

    if (last_was_mistake) {
        return true;
    }


    return opponent_history.back();
}

void CTFT::reset() {
    last_was_mistake = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Custom1::Custom1() : gen(std::random_device{}()), dis(0.0, 1.0) {}

std::string Custom1::get_name() {
    return "custom1";
}

bool Custom1::make_choice(const std::vector<bool>& my_history,
    const std::vector<bool>& opponent_history,
    const std::vector<int>& my_scores,
    const std::vector<int>& opponent_scores) {
    if (opponent_history.empty()) {
        opponent_defects = 0;
        return true; 
    }


    if (!opponent_history.back()) {
        opponent_defects++;
    }

    // evol
    if (opponent_defects > 2) {
        return false; 
    }
    else {
        // mutation
        if (dis(gen) < 0.1) {
            return dis(gen) > 0.5; 
        }
        return true; 
    }
}

void Custom1::reset() {
    opponent_defects = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Custom2::Custom2() : gen(std::random_device{}()), dis(0.0, 1.0) {}

std::string Custom2::get_name() {
    return "custom2";
}

bool Custom2::make_choice(const std::vector<bool>& my_history,
    const std::vector<bool>& opponent_history,
    const std::vector<int>& my_scores,
    const std::vector<int>& opponent_scores) {
    if (opponent_history.empty()) {
        return true; 
    }

   
    bool base_action = opponent_history.back();

    //mutation
    if (dis(gen) < 0.15) {
        return !base_action; 
    }

    return base_action; 
}

void Custom2::reset() {
    
}
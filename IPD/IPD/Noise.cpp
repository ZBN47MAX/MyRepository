#include "Noise.h"


Noise global_noise(0.0);

Noise::Noise(double level) : noise_level(std::max(0.0, std::min(1.0, level))),
gen(std::random_device{}()),
dis(0.0, 1.0) {
}

void Noise::set_seed(unsigned seed) {
    gen.seed(seed);
}

bool Noise::apply_noise(bool original_choice) {
    if (noise_level <= 0.0) {
        return original_choice; 
    }

    if (dis(gen) < noise_level) {
        return !original_choice; 
    }

    return original_choice; 
}

double Noise::get_noise_level() const {
    return noise_level;
}

void Noise::set_noise_level(double level) {
    noise_level = std::max(0.0, std::min(1.0, level));
}
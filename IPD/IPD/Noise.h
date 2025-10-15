#ifndef NOISE_H
#define NOISE_H

#include "pch.h"

class Noise {
private:
    double noise_level; // 0=non 1=total
    std::mt19937 gen;
    std::uniform_real_distribution<double> dis;

public:
    Noise(double level = 0.0);
    void set_seed(unsigned seed);
    bool apply_noise(bool original_choice);
    double get_noise_level() const;
    void set_noise_level(double level);
};


extern Noise global_noise;

#endif 
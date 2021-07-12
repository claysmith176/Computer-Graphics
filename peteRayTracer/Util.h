#pragma once
#include <iostream>

float random_float() {
    return (float)rand() / (RAND_MAX + 1.0);
}

float random_float(float min, float max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_float();
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_float(min, max + 1));
}


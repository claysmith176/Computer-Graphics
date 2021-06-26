#pragma once
#include <iostream>

float random_float() {
    return (float)rand() / (RAND_MAX + 1.0);
}

float random_float(float min, float max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_float();
}
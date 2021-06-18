#pragma once

#include <GL/glm/vec3.hpp>

class Ray {
public:
    Ray() {}
    Ray(glm::vec3 origin, glm::vec3 direction)
        : orig_(origin), direction_(direction) {}

    glm::vec3 origin() const { return orig_; }
    glm::vec3 dir() const { return direction_; }

    glm::vec3 at(float t)  const {
        return orig_ + t * direction_;
    }
private:
    glm::vec3 orig_;
    glm::vec3 direction_;
};

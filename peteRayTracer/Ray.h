#pragma once

class Ray {
public:
    Ray() {}
    Ray(vec3 origin, vec3 direction)
        : orig_(origin), direction_(direction) {}

    vec3 origin() const { return orig_; }
    vec3 dir() const { return direction_; }

    vec3 at(float t)  const {
        return orig_ + t * direction_;
    }
private:
    vec3 orig_;
    vec3 direction_;
};

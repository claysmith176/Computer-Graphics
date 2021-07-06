#pragma once

class Ray {
public:
    Ray() {}
    Ray(vec3 origin, vec3 direction, float time)
        : orig_(origin), direction_(direction), time_(time) {}

    vec3 origin() const { return orig_; }
    vec3 dir() const { return direction_; }
    float time() const { return time_; }

    vec3 at(float t)  const {
        return orig_ + t * direction_;
    }
private:
    vec3 orig_;
    vec3 direction_;
    float time_;
};

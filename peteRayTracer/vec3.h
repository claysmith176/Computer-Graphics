#pragma once

#include <cmath>
#include <iostream>
#include "Util.h"

using std::sqrt;
using std::fabs;

class vec3 {
public:
    vec3() : e{ 0,0,0 } {}
    vec3(float e0, float e1, float e2) : e{ e0, e1, e2 } {}

    float x() const { return e[0]; }
    float y() const { return e[1]; }
    float z() const { return e[2]; }

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    float operator[](int i) const { return e[i]; }
    float& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3& operator*=(const float t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(const float t) {
        return *this *= 1 / t;
    }

    float length() const {
        return sqrtf(length_squared());
    }

    float length_squared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    bool near_zero() const {
        // Return true if the vector is close to zero in all dimensions.
        const auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }

    inline static vec3 random() {
        return vec3(random_float(), random_float(), random_float());
    }

    inline static vec3 random(float min, float max) {
        return vec3(random_float(min, max), random_float(min, max), random_float(min, max));
    }

public:
    float e[3];
};


// Type aliases for vec3
using point3 = vec3;   // 3D point
using color = vec3;    // RGB color


// vec3 Utility Functions

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3& v) {
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) {
    return t * v;
}

inline vec3 operator/(vec3 v, double t) {
    return (1 / t) * v;
}

inline double dot(const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0]
        + u.e[1] * v.e[1]
        + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
        u.e[2] * v.e[0] - u.e[0] * v.e[2],
        u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}

inline vec3 random_in_unit_disk() {
    while (true) {
        vec3 p = vec3(random_float(-1, 1), random_float(-1, 1), 0);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

inline vec3 random_in_unit_sphere() {
    while (true) {
        auto p = vec3::random(-1, 1);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

inline vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

inline vec3 reflect_ray(const vec3& v, const vec3& n) {
    return v - 2 * dot(v, n) * n;
}

inline vec3 refract_ray(const vec3& uv, const vec3& n, double etai_over_etat) {
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

inline void print_vec(vec3 vec) {
    std::cout << vec.x() << ", " << vec.y() << ", " << vec.z() << std::endl;
}

inline vec3 write_color(vec3 color, float samples_per_pixel) {
    float scale = 1.0f / (samples_per_pixel * 255);
    color *= scale;

    // gamma correction
    vec3 new_color(std::sqrtf(color.x()), std::sqrtf(color.y()), std::sqrtf(color.z()));
    return new_color * 255.0f;
}

vec3 rotate(const float degrees, const vec3& axis, const vec3& eye) {
    float x = axis.x();
    float y = axis.y();
    float z = axis.z();
    float radians = degrees * (M_PI / 180);

    glm::vec3 eye_g(eye.x(), eye.y(), eye.z());
    glm::vec3 axis_g(axis.x(), axis.y(), axis.z());

    glm::mat3 xmat(1.0f);
    glm::mat3 ymat = glm::outerProduct(axis_g, axis_g);
    glm::mat3 zmat(0, z, -y, -z, 0, x, y, -x, 0);

    xmat = xmat * cos(radians);
    ymat = ymat * (1 - cos(radians));
    zmat = zmat * sin(radians);

    glm::mat3 test = xmat + ymat + zmat;
    glm::vec3 res = test * eye_g;
    return vec3(res.x, res.y, res.z);
}

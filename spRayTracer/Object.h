#define _USE_MATH_DEFINES

#include <iostream>

#include <GL/glm/glm.hpp>
#include <GL/glm/vec3.hpp>;
#include <GL/glm/vec3.hpp> // glm::vec3
#include <GL/glm/vec4.hpp> // glm::vec4
#include <GL/glm/mat4x4.hpp> // glm::mat4
#include <GL/glm/gtc/matrix_transform.hpp>
#include "GL/glm/ext.hpp"

class Object {
public:
    glm::vec3 center;
    glm::vec3 color;
    Object() {
        center = glm::vec3(0, 0, 0);
        color = glm::vec3(0, 0, 0);
    }
    /*virtual bool intersect(const glm::vec3& orgin, const glm::vec3& dir, float& dist) {
        dist = 0;
        return true;
    };*/

    virtual bool intersect(const glm::vec3& orgin, const glm::vec3& dir, float& dist) const = 0;
    virtual void getSurfaceData(const glm::vec3 &Phit, glm::vec3 &Nhit, glm::vec2 &tex) {
        tex.x = 1;
        tex.y = 1;
        Nhit = glm::vec3(0, 1, 0);
    };
};

class Sphere : public Object {
public:
    float radius;
    float radius2;
    Sphere(glm::vec3 center, glm::vec3 color, float radius) {
        this->radius = radius;
        this->radius2 = radius * radius;
        this->center = center;
        this->color = color;
    }

    bool intersect(const glm::vec3& orig, const glm::vec3& dir, float& dist) const {
        float x0, x1;
        glm::vec3 test = center;
        glm::vec3 L = orig - center; // 0, -1, -5
        float a = glm::dot(dir, dir);
        float b = 2 * glm::dot(dir, L);
        float c = glm::dot(L, L) - (radius2);

        float discr = b * b - 4 * a * c;
        if (discr < 0) {
            dist = INFINITY;
            return false;
        }
        else if (discr == 0) {
            x0 = -b / (2 * a);
            dist = x0;
        }
        else {
            float q;
            if (b < 0) {
                q = -0.5 * (b + (float)std::sqrt(discr));
            }
            else {
                q = -0.5 * (b - (float)std::sqrt(discr));
            }
            x0 = q / a;
            x1 = c / q;

            dist = std::min(x0, x1);
        }
        return true;
    }

    void getSurfaceData(const glm::vec3& Phit, glm::vec3& Nhit, glm::vec2& tex) {
        Nhit = Phit - center;
        Nhit = glm::normalize(Nhit);

        tex.x = (1 + atan2(Nhit.z, Nhit.x) / M_PI) * 0.5;
        tex.y = acosf(Nhit.y) / M_PI;
    }
};

class Plane : public Object {
public:
    glm::vec3 normal;
    Plane(glm::vec3 normal, glm::vec3 center, glm::vec3 color) {
        this->normal = normal;
        this->center = center;
        this->color = color;
    }
    bool intersect(const glm::vec3& orig, const glm::vec3& dir, float& dist) const {
        float denom = glm::dot(dir, normal);
        if (std::abs(denom) > 1e-6) {
            glm::vec3 p0l0 =  center - orig;
            float num = glm::dot(p0l0, normal);
            dist = num / denom;
            if (dist > 0) {
                int adc = 5;
            }
            return true;
        }
        return false;
    }
    void getSurfaceData(const glm::vec3& Phit, glm::vec3& Nhit, glm::vec2& tex) {
        Nhit = glm::normalize(normal);

        tex.x = 0;
        tex.y = 1.9;
    }
};

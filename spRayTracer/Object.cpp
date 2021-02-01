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
	Object() {}
	virtual bool intersect(const glm::vec3& orig, const glm::vec3& dir, float& t);
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

	bool intersect(const glm::vec3& orig, const glm::vec3& dir, float& dist) {
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
#pragma once
#include <GL/glm/glm.hpp>
#include <GL/glm/vec3.hpp>
#include "ray.h"

struct hit_data {
	glm::vec3 point;
	glm::vec3 normal;
	double dist;
	bool front_face;
	inline void set_face_normal(const Ray& r, const glm::vec3& outward_normal) {
		front_face = glm::dot(r.dir(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class Hittable {
public:
	virtual bool hit(const Ray& r, double t_min, double t_max, hit_data& rec) const = 0;
};
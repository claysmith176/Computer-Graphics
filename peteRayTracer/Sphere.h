#pragma once
#include <GL/glm/glm.hpp>
#include <GL/glm/vec3.hpp>
#include "Ray.h"
#include "Hittable.h"

class Sphere : public Hittable {
public:
	Sphere(glm::vec3 cen, float rad);
	bool hit(const Ray& r, double t_min, double t_max, hit_data& rec) const override;


private:
	glm::vec3 center;
	float radius;
	float radius2;
};
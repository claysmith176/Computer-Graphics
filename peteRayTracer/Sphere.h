#pragma once
#include <memory>

#include <GL/glm/glm.hpp>
#include <GL/glm/vec3.hpp>
#include "Ray.h"
#include "Hittable.h"

class Sphere : public Hittable {
public:
	Sphere(glm::vec3 cen, float rad, std::shared_ptr<Material> mat);
	bool hit(const Ray& r, double t_min, double t_max, hit_data& rec) const override;


private:
	glm::vec3 center;
	float radius;
	float radius2;
	std::shared_ptr<Material> mat_ptr;
};

Sphere::Sphere(glm::vec3 cen, float rad, std::shared_ptr<Material> mat) : center(cen), radius(rad), mat_ptr(mat) {
	radius2 = radius * radius;
}

bool Sphere::hit(const Ray& r, double t_min, double t_max, hit_data& rec) const {
	glm::vec3 oc = r.origin() - center;
	float c = glm::dot(oc, oc) - radius2;
	float b = 2.0 * glm::dot(r.dir(), oc);
	float a = glm::dot(r.dir(), r.dir());
	float discriminant = b * b - 4 * a * c;
	if (discriminant < 0) {
		return false;
	}
	float sqrt_disc = std::sqrtf(discriminant);
	float x0 = 0.5 * (-b + sqrt_disc) / (a);
	float x1 = 0.5 * (-b - sqrt_disc) / (a);
	float x;
	if (std::fmax(x0, x1) < 0) { return false; }
	if (x0 > 0 && x1 > 0) { x = std::fmin(x0, x1); }
	else { x = std::fmax(x0, x1); }
	if (x < t_min) { return false; }
	else if (x > t_max) { return false; }

	rec.dist = x;
	rec.point = r.at(x);
	rec.mat_ptr = mat_ptr;
	glm::vec3 out_normal = (rec.point - center) / radius;
	rec.set_face_normal(r, out_normal);
	return true;
}
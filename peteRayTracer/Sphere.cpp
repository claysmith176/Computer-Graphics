#include "Sphere.h"


Sphere::Sphere(glm::vec3 cen, float rad) : center(cen), radius(rad) {
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
	if (x0 > 0 && x1 > 0) { x = std::fmax(x0, x1); }
	else { x = std::fmax(x0, x1); }

	rec.dist = x;
	rec.point = r.at(x);
	glm::vec3 out_normal = (rec.point - center) / radius;
	rec.set_face_normal(r, out_normal);
	return true;
}
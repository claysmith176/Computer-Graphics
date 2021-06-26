#pragma once
#include "Material.h"
#include "Util.h"
#include <GL/glm/glm.hpp>
#include <GL/glm/vec3.hpp>

class Lambertian : public  Material {
public:
	Lambertian(const glm::vec3& color) : albedo(color) {}
    virtual bool scatter(
        const Ray& r_in, hit_data rec, glm::vec3& attenuation, Ray& scattered
    ) const override {
        auto scatter_direction = rec.normal + random_unit_sphere();
        scattered = Ray(rec.point, scatter_direction);
        attenuation = albedo;
        return true;
    }

public:
	glm::vec3 albedo;
};

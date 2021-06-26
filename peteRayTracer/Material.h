#pragma once
#include "Util.h"

struct hit_data;

class Material {
public:
    virtual bool scatter(
        const Ray& r_in, vec3& attenuation, Ray& scattered, hit_data& rec) const = 0;
};


class Lambertian : public Material {
public:
    Lambertian(const vec3& a) : albedo(a) {}
    virtual bool scatter(const Ray& r_in, vec3& attenuation, Ray& scattered, hit_data& rec) const override {
        vec3 scatter_direction = unit_vector(rec.normal + random_in_unit_sphere());
        float near_zero = 1e-8;
        if (fabs(scatter_direction.x()) < near_zero && fabs(scatter_direction.y()) < near_zero && fabs(scatter_direction.z()) < near_zero) {
            scatter_direction = rec.normal;
        }
        scattered = Ray(rec.point, scatter_direction);
        attenuation = albedo;
        return dot(scattered.dir(), rec.normal) > 0;
        return true;
    }

public:
    vec3 albedo;
};

class Metal : public Material {
public:
    Metal(const vec3 a, float f) : albedo(a), fuzz(f) {}
    virtual bool scatter(const Ray& r_in, vec3& attenuation, Ray& scattered, hit_data& rec) const override {
        vec3 reflected = reflect_ray(r_in.dir(), rec.normal);
        scattered = Ray(rec.point, unit_vector(reflected + fuzz* random_in_unit_sphere()));
        attenuation = albedo;
        return dot(scattered.dir(), rec.normal) > 0; 
        return true;
    }
public:
    vec3 albedo;
    float fuzz;
};

class Dielectric : public Material {
public:
    Dielectric(float index_of_refraction) : ior(index_of_refraction) {}
    virtual bool scatter(const Ray& r_in, vec3& attenuation, Ray& scattered, hit_data& rec) const override {
        attenuation = vec3(1, 1, 1);
        float ratio_ior = rec.front_face ? (1.0f / ior) : ior;
        float cos_theta = std::fmin(dot(-r_in.dir(), rec.normal), 1.0);
        float sin_theta = std::sqrtf(1 - cos_theta * cos_theta);
        
        vec3 direction;
        if (ratio_ior * sin_theta > 1 || reflectance(cos_theta, ratio_ior) > random_float()) {
            direction = reflect_ray(r_in.dir(), rec.normal);
        }
        else {
            direction = refract_ray(r_in.dir(), rec.normal, ratio_ior);
        }
        scattered =  Ray(rec.point, direction); 
        return true;
    }
public:
    float ior;
private:
    static float reflectance(float cosine, float ref_idx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};
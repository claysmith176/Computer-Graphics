#pragma once
class Material;

struct hit_data {
	vec3 point;
	vec3 normal;
	double dist;
	bool front_face;
	std::shared_ptr<Material> mat_ptr;

	inline void set_face_normal(const Ray& r, const vec3& outward_normal) {
		front_face = dot(r.dir(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class Hittable {
public:
	virtual bool hit(const Ray& r, double t_min, double t_max, hit_data& rec) const = 0;
};


class Sphere : public Hittable {
public:
	Sphere(vec3 cen, float rad, std::shared_ptr<Material> mat) : center(cen), radius(rad), mat_ptr(mat) {
		radius2 = radius * radius;
	}
	bool hit(const Ray& r, double t_min, double t_max, hit_data& rec) const override {
		vec3 oc = r.origin() - center;
		float c = dot(oc, oc) - radius2;
		float half_b = dot(r.dir(), oc);
		float a = dot(r.dir(), r.dir());
		float discriminant = half_b * half_b - a * c;
		if (discriminant < 0) {
			return false;
		}

		auto sqrt_disc = std::sqrt(discriminant);
		float x = (-half_b - sqrt_disc) / (a);
		if (x < t_min || x > t_max) {
			x = (-half_b + sqrt_disc) / (a);
			if (x < t_min || x > t_max) {
				return false;
			}
		}

		rec.dist = x;
		rec.point = r.at(x);
		rec.mat_ptr = mat_ptr;
		vec3 out_normal = (rec.point - center) / radius;
		rec.set_face_normal(r, out_normal);
		return true;
	}

private:
	vec3 center;
	float radius;
	float radius2;
	std::shared_ptr<Material> mat_ptr;
};

class Hittable_List : public Hittable {
public:
    Hittable_List() {}
    Hittable_List(std::shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }
    void add(std::shared_ptr<Hittable> object) { objects.push_back(object); }

    virtual bool hit(
        const Ray& r, double t_min, double t_max, hit_data& rec) const override;

public:
    std::vector<std::shared_ptr<Hittable>> objects;
};

bool Hittable_List::hit(const Ray& r, double t_min, double t_max, hit_data& rec) const {
    hit_data temp_rec;
    bool hit_anything = false;;
    float closest = t_max;
    for (std::shared_ptr<Hittable> object : objects) {
        if (object->hit(r, t_min, t_max, temp_rec)) {
            if (temp_rec.dist < closest) {
                hit_anything = true;
                closest = temp_rec.dist;
                rec = temp_rec;
            }
        }
    }
    return hit_anything;
}
#pragma once
class Sphere : public Hittable {
public:
	Sphere(vec3 cen, float rad, std::shared_ptr<Material> mat) : center(cen), radius(rad), mat_ptr(mat) {
		radius2 = radius * radius;
	}
	bool hit(const Ray& r, float t_min, float t_max, hit_data& rec) const override {
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

	bool bounding_box(float time0, float time1, aabb& output_bot) const override {
		vec3 rad = vec3(radius, radius, radius);
		output_bot = aabb(center - rad, center + rad);
		return true;
	}

private:
	vec3 center;
	float radius;
	float radius2;
	std::shared_ptr<Material> mat_ptr;
};

class Moving_sphere : public Hittable {
public:
	Moving_sphere(vec3 cen0, vec3 cen1, float _time0, float _time1, float rad, std::shared_ptr<Material> mat)
		: center0(cen0), center1(cen1), time0(_time0), time1(_time1), radius(rad), mat_ptr(mat) {
		radius2 = radius * radius;
	}
	bool hit(const Ray& r, float t_min, float t_max, hit_data& rec) const override {
		vec3 oc = r.origin() - get_center(r.time());
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
		vec3 out_normal = (rec.point - get_center(r.time())) / radius;
		rec.set_face_normal(r, out_normal);
		return true;
	}
	bool bounding_box(float time0, float time1, aabb& output_bot) const override {
		vec3 rad = vec3(radius, radius, radius);
		aabb box0 = aabb(get_center(time0) - rad, get_center(time0) + rad);
		aabb box1 = aabb(get_center(time1) - rad, get_center(time1) + rad);
		output_bot = surrounding_box(box0, box1);
		return true;
	}
	vec3 get_center(float time) const {
		return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
	}

private:
	vec3 center0;
	vec3 center1;
	float time0;
	float time1;
	float radius;
	float radius2;
	std::shared_ptr<Material> mat_ptr;
};
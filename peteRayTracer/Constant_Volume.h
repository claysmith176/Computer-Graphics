#pragma once
class Constant_Volume : public Hittable {
public:
	Constant_Volume(std::shared_ptr<Hittable> b, float d, std::shared_ptr<Texture> t)
		: boundry(b), 
		mat(std::make_shared<Isotropic>(t)), 
		neg_inv_density(-1 / d) {}
	Constant_Volume(std::shared_ptr<Hittable> b, float d, vec3 c)
		: boundry(b),
		neg_inv_density(-1 / d),
		mat(std::make_shared<Isotropic>(c)) {}

	bool hit(const Ray& r, float t_min, float t_max, hit_data& rec) const override {
		hit_data rec1, rec2;
		if (!boundry->hit(r, -INFINITY, INFINITY, rec1)) { return false; }
		if (!boundry->hit(r, rec1.dist + 0.0001, INFINITY, rec2)) { return false; }

		if (rec1.dist < t_min) { rec1.dist = t_min; }
		if (rec2.dist > t_max) { rec2.dist = t_max; }
		if (rec1.dist >= rec2.dist) { return false; }
		if (rec1.dist < 0) { rec1.dist = 0; }

		float distance_inside = rec2.dist - rec1.dist;
		float hit_distance = neg_inv_density * std::log(random_float());
		if (hit_distance > distance_inside) { return false; }

		rec.dist = rec1.dist + hit_distance;
		rec.point = r.at(rec.dist);
		rec.normal = vec3(0, 0, 1); // does not matter
		rec.front_face = true; // does not matter
		rec.mat_ptr = mat;
		return true;
	}

	bool bounding_box(float time0, float time1, aabb& output_box) const override {
		return boundry->bounding_box(time0, time1, output_box);
	}
public:
	std::shared_ptr<Hittable> boundry;
	std::shared_ptr<Isotropic> mat;
	float neg_inv_density;
};
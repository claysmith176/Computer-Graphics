#pragma once
class Material;

struct hit_data {
	vec3 point;
	vec3 normal;
	double dist;
    float u, v;
	bool front_face;
	std::shared_ptr<Material> mat_ptr;

	inline void set_face_normal(const Ray& r, const vec3& outward_normal) {
		front_face = dot(r.dir(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class Hittable {
public:
	virtual bool hit(const Ray& r, float t_min, float t_max, hit_data& rec) const = 0;
    virtual bool bounding_box(float time0, float time1, aabb& output_box) const = 0;
};



class Hittable_List : public Hittable {
public:
    Hittable_List() {}
    Hittable_List(std::shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }
    void add(std::shared_ptr<Hittable> object) { objects.push_back(object); }

    virtual bool hit(const Ray& r, float t_min, float t_max, hit_data& rec) const override;
    virtual bool bounding_box(float time0, float time1, aabb& output_box) const override;

public:
    std::vector<std::shared_ptr<Hittable>> objects;
};

bool Hittable_List::hit(const Ray& r, float t_min, float t_max, hit_data& rec) const {
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
bool Hittable_List::bounding_box(float time0, float time1, aabb& output_box) const {
    if (objects.empty()) { return false; }
    aabb tmp;
    bool first = true;
    for (const auto& obj : objects) {
        if (!obj->bounding_box(time0, time1, tmp)) { return false; }
        if (first) { output_box = tmp; }
        else { output_box = surrounding_box(output_box, tmp); }
        first = false;
    }
    return true;
}
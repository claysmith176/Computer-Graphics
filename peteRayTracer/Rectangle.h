#pragma once
class XY_Rect : public Hittable {
public:
	XY_Rect() {}
	XY_Rect(float _x0, float _x1, float _y0, float _y1, float _k, std::shared_ptr<Material> mat) 
	: x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {}

	bool hit(const Ray& r, float t_min, float t_max, hit_data& rec) const override {
		float t = (k - r.origin().z()) / r.dir().z();
		if (t < t_min || t > t_max) { return false; }
		float x = r.at(t).x();
		float y = r.at(t).y();
		if (x < x0 || x > x1 || y < y0 || y > y1) { return false; }
		rec.u = (x - x0) / (x1 - x0);
		rec.v = (y - y0) / (y1 - y0);
		rec.dist = t;
		rec.set_face_normal(r, vec3(0, 0, 1));
		rec.mat_ptr = mp;
		rec.point = r.at(t);
		return true;
	}

	bool bounding_box(float time0, float time1, aabb& output_box) const override {
		output_box = aabb(vec3(x0, y0, k - 0.0001), vec3(x1, y1, k + 0.0001));
		return true;
	}
private:
	std::shared_ptr<Material> mp;
	float x0, x1, y0, y1, k;
};

class XZ_Rect : public Hittable {
public:
    XZ_Rect() {}

    XZ_Rect(float _x0, float _x1, float _z0, float _z1, float _k,
        std::shared_ptr<Material> mat)
        : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

    bool hit(const Ray& r, float t_min, float t_max, hit_data& rec) const override {
        auto t = (k - r.origin().y()) / r.dir().y();
        if (t < t_min || t > t_max)
            return false;
        auto x = r.origin().x() + t * r.dir().x();
        auto z = r.origin().z() + t * r.dir().z();
        if (x < x0 || x > x1 || z < z0 || z > z1)
            return false;
        rec.u = (x - x0) / (x1 - x0);
        rec.v = (z - z0) / (z1 - z0);
        rec.dist = t;
        auto outward_normal = vec3(0, 1, 0);
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mp;
        rec.point = r.at(t);
        return true;
    }

    bool bounding_box(float time0, float time1, aabb& output_box) const override {
        output_box = aabb(point3(x0, k - 0.0001, z0), point3(x1, k + 0.0001, z1));
        return true;
    }

public:
    std::shared_ptr<Material> mp;
    float x0, x1, z0, z1, k;
};

class YZ_Rect : public Hittable {
public:
    YZ_Rect() {}

    YZ_Rect(float _y0, float _y1, float _z0, float _z1, float _k,
        std::shared_ptr<Material> mat)
        : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

    virtual bool hit(const Ray& r, float t_min, float t_max, hit_data& rec) const override {
        auto t = (k - r.origin().x()) / r.dir().x();
        if (t < t_min || t > t_max)
            return false;
        auto y = r.origin().y() + t * r.dir().y();
        auto z = r.origin().z() + t * r.dir().z();
        if (y < y0 || y > y1 || z < z0 || z > z1)
            return false;
        rec.u = (y - y0) / (y1 - y0);
        rec.v = (z - z0) / (z1 - z0);
        rec.dist = t;
        auto outward_normal = vec3(1, 0, 0);
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mp;
        rec.point = r.at(t);
        return true;
    }

    bool bounding_box(float time0, float time1, aabb& output_box) const override {
        output_box = aabb(point3(k - 0.0001, y0, z0), point3(k + 0.0001, y1, z1));
        return true;
    }

public:
    std::shared_ptr<Material> mp;
    float y0, y1, z0, z1, k;
};

class Box : public Hittable {
public:
    Box(vec3 min, vec3 max, std::shared_ptr<Material> ptr) {
        box_min = min;
        box_max = max;

        sides.add(std::make_shared<XY_Rect>(min.x(), max.x(), min.y(), max.y(), min.z(), ptr));
        sides.add(std::make_shared<XY_Rect>(min.x(), max.x(), min.y(), max.y(), max.z(), ptr));

        sides.add(std::make_shared<XZ_Rect>(min.x(), max.x(), min.z(), max.z(), min.y(), ptr));
        sides.add(std::make_shared<XZ_Rect>(min.x(), max.x(), min.z(), max.z(), max.y(), ptr));

        sides.add(std::make_shared<YZ_Rect>(min.y(), max.y(), min.z(), max.z(), min.x(), ptr));
        sides.add(std::make_shared<YZ_Rect>(min.y(), max.y(), min.z(), max.z(), max.x(), ptr));
    }
    bool hit(const Ray& r, float t_min, float t_max, hit_data& rec) const override {
        return sides.hit(r, t_min, t_max, rec);
    }

    bool bounding_box(float time0, float time1, aabb& output_box) const override {
        output_box = aabb(box_min, box_max);
        return true;
    }

    vec3 box_min, box_max;
    Hittable_List sides;
};
#pragma once
class Translate : public Hittable {
public:
    Translate(std::shared_ptr<Hittable> p, vec3 move) : offset(move), ptr(p) {}

    bool hit(const Ray& r, float t_min, float t_max, hit_data& rec) const override {
        Ray new_ray = Ray(r.origin() - offset, r.dir(), r.time());
        if (!ptr->hit(new_ray, t_min, t_max, rec)) {
            return false;
        }
        rec.point += offset;
        rec.set_face_normal(new_ray, rec.normal);   
        return true;
    }

    bool bounding_box(float time0, float time1, aabb& output_box) const override {
        if (!ptr->bounding_box(time0, time1, output_box)) {
            return false;
        }
        output_box = aabb(output_box.min() + offset, output_box.max() + offset);
        return true;
    }
public:
    std::shared_ptr<Hittable> ptr;
    vec3 offset;
};

class Y_Rotate : public Hittable {
public:
    Y_Rotate(std::shared_ptr<Hittable> p, float angle) {
        ptr = p;
        float radians = angle * (M_PI / 180);
        sin_theta = std::sin(radians);
        cos_theta = std::cos(radians);
        hasbox = ptr->bounding_box(0, 1, bbox);

        vec3 min(INFINITY, INFINITY, INFINITY);
        vec3 max(-INFINITY, -INFINITY, -INFINITY);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    float x = i * bbox.min().x() + (1 - i) * bbox.max().x();
                    float y = j * bbox.min().y() + (1 - j) * bbox.max().y();
                    float z = k * bbox.min().z() + (1 - k) * bbox.max().z();

                    float newx = cos_theta * x + sin_theta * z;
                    float newz = -sin_theta * x + cos_theta * z;
                    vec3 tester(newx, y, newz);
                    for (int c = 0; c < 3; c++) {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }
            }
        }
        bbox = aabb(min, max);
    }
    bool hit(const Ray& r, float t_min, float t_max, hit_data& rec) const override {
        vec3 origin = r.origin();
        vec3 direction = r.dir();
        
        origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
        origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

        direction[0] = cos_theta * r.dir()[0] - sin_theta * r.dir()[2];
        direction[2] = sin_theta * r.dir()[0] + cos_theta * r.dir()[2];

        Ray rotated_r(origin, direction, r.time());

        if (!ptr->hit(rotated_r, t_min, t_max, rec))
            return false;

        vec3 p = rec.point;
        vec3 normal = rec.normal;

        p[0] = cos_theta * rec.point[0] + sin_theta * rec.point[2];
        p[2] = -sin_theta * rec.point[0] + cos_theta * rec.point[2];

        normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
        normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

        rec.point = p;
        rec.set_face_normal(rotated_r, normal);

        return true;
    }

    bool bounding_box(float time0, float time1, aabb& output_box) const override {
        output_box = bbox;
        return true;
    }
public:
    float sin_theta, cos_theta;
    bool hasbox;
    aabb bbox;
    std::shared_ptr<Hittable> ptr;
};
#pragma once
#include "Hittable.h"

#include <memory>
#include <vector>


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
            if (rec.dist < closest) {
                hit_anything = true;
                closest = rec.dist;
                rec = temp_rec;
            }
        }
    }
    return hit_anything;
}
#pragma once
class bvh : public Hittable {
public:
	bvh() {}
	bvh(const Hittable_List& list, float time0, float time1)
		: bvh(list.objects, 0, list.objects.size(), time0, time1) {}
	bvh(const std::vector<std::shared_ptr<Hittable>> objects,
		size_t start, size_t end, float time0, float time1);

	virtual bool hit(const Ray& r, float t_min, float t_max, hit_data& rec) const override;
	virtual bool bounding_box(float time0, float time1, aabb& output_box) const override {
		output_box = box;
		return true;
	}

	std::shared_ptr<Hittable> left;
	std::shared_ptr<Hittable> right;
	aabb box;
};

bool bvh::hit(const Ray& r, float t_min, float t_max, hit_data& rec) const {
	if (!box.hit(r, t_min, t_max)) { return false; }
	bool hit_left = left->hit(r, t_min, t_max, rec);
	bool hit_right = right->hit(r, t_min, hit_left ? rec.dist : t_max, rec);

	return (hit_left || hit_right);
}

inline bool box_compare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b, int axis) {
	aabb box_a, box_b;
	if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b)) {
		std::cerr << "Can Make Bounding Box" << std::endl;
	}
	return box_a.min().e[axis] < box_b.min().e[axis];
}
inline bool box_x_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
	return box_compare(a, b, 0);
}
inline bool box_y_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
	return box_compare(a, b, 1);
}
inline bool box_z_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
	return box_compare(a, b, 2);
}

bvh::bvh(const std::vector<std::shared_ptr<Hittable>> src_objects, size_t start, size_t end, float time0, float time1) {
	auto objects = src_objects;
	int axis = random_int(0, 3);
	

	int object_span = end - start;

	if (object_span == 1) {
		left = right = objects[start];
	}
	else if (object_span == 2) {
		if (box_compare(objects[start], objects[start + 1], axis)) {
			left = objects[start];
			right = objects[start + 1];
		}
		else {
			left = objects[start+1];
			right = objects[start];
		}
	}
	else {
		if (axis == 0) {
			std::sort(objects.begin()+start, objects.begin()+end, box_x_compare);
		}
		else if (axis == 1) {
			std::sort(objects.begin() + start, objects.begin() + end, box_y_compare);
		}
		else {
			std::sort(objects.begin() + start, objects.begin() + end, box_z_compare);
		}
		int mid = start + object_span / 2;
		left = std::make_shared<bvh>(objects, start, mid, time0, time1);
		right = std::make_shared<bvh>(objects, mid, end, time0, time1);
	}
	aabb box_left, box_right;
	if (!left->bounding_box(time0, time1, box_left) || !right->bounding_box(time0, time1, box_right)) {
		std::cerr << "Oh no" << std::endl;
	}
	box = surrounding_box(box_left, box_right);
}



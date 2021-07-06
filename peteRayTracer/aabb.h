#pragma once
class aabb {
public:
	aabb() {}
	aabb(const vec3& a, const vec3& b) : min_(a), max_(b) {}

	vec3  min() const {return min_;}
	vec3  max() const {return max_;}

	bool hit(const Ray& r, float t_min, float t_max) const {
		for (int i = 0; i < 3; i++) {
			float invD = 1.0f / r.dir()[i];
			float t0 = (min_[i] - r.origin()[i]) * invD;
			float t1 = (max_[i] - r.origin()[i]) * invD;
			if (invD < 0) {
				std::swap(t0, t1);
			}
			t_min = std::max(t0, t_min);
			t_max = std::min(t1, t_max);
			if (t_max <= t_min) {
				return false;
			}
		}
		return true;
	}

private:
	vec3 min_;
	vec3 max_;
};

aabb surrounding_box(aabb box0, aabb box1) {
	vec3 small(std::min(box0.min().x(), box1.min().x()),
		std::min(box0.min().y(), box1.min().y()),
		std::min(box0.min().z(), box1.min().z()));
	vec3 big(std::max(box0.max().x(), box1.max().x()),
		std::max(box0.max().y(), box1.max().y()),
		std::max(box0.max().z(), box1.max().z()));
	return aabb(small, big);
}
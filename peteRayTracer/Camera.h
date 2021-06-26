#pragma once
#define _USE_MATH_DEFINES

class Camera {
public:
	//horizontal fov
	Camera(
		float FOV,
		float aspect,
		vec3 lookfrom,
		vec3 lookat,
		vec3 up
	) {
		float left = std::tanf((FOV / 2) * (M_PI / 180));
		float viewportWidth = left * 2;
		float viewportHeight = viewportWidth / aspect;
		float focal_length = 1;

		//create corodinate system
		vec3 w = unit_vector(lookfrom - lookat);
		vec3 u = unit_vector(cross(up, w));
		vec3 v = cross(w, u);
		origin = lookfrom;
		horizontal = viewportWidth * u;
		vertical = viewportHeight * v;
		top_left_corner = origin - horizontal * 0.5f + vertical * 0.5f - w;
	}

	Ray get_ray(float x, float y) const {
		//glm::vec3 test = glm::normalize(top_left_corner + x * horizontal - y * vertical - origin);
		return Ray(origin, unit_vector(top_left_corner + x * horizontal - y * vertical - origin));
	}

private:
	vec3 origin;
	vec3 top_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 u, v, w;
	float lens_radius;
};
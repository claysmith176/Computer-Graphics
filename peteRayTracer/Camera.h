#pragma once
#define _USE_MATH_DEFINES
#include <cmath> 
#include <GL/glm/vec3.hpp>
#include <GL/glm/glm.hpp>
#include "Ray.h"

class Camera {
public:
	//horizontal fov
	Camera(
		float FOV,
		float aspect,
		glm::vec3 lookfrom,
		glm::vec3 lookat,
		glm::vec3 up
	) {
		float left = std::tanf((FOV / 2) * (M_PI / 180));
		float viewportWidth = left * 2;
		float viewportHeight = viewportWidth / aspect;
		float focal_length = 1;

		//create corodinate system
		glm::vec3 w = glm::normalize(lookfrom - lookat);
		glm::vec3 u = glm::normalize(glm::cross(up, w));
		glm::vec3 v = glm::cross(w, u);
		origin = lookfrom;
		horizontal = viewportWidth * u;
		vertical = viewportHeight * v;
		top_left_corner = origin - horizontal * 0.5f + vertical * 0.5f - w;
	}

	Ray get_ray(float x, float y) const {
		return Ray(origin, glm::normalize(top_left_corner + x * horizontal - y * vertical - origin));
	}

private:
	glm::vec3 origin;
	glm::vec3 top_left_corner;
	glm::vec3 horizontal;
	glm::vec3 vertical;
	glm::vec3 u, v, w;
};
#define _USE_MATH_DEFINES

#include <GL/glm/glm.hpp>
#include <GL/glm/vec3.hpp>;

#include "Light.h"

DistantLight::DistantLight(glm::vec3 dir, glm::vec3 color, float i) {
	this->color = color;
	this->intesnity = i;
	this->dir = glm::normalize(dir);
}

void DistantLight::getDirectionAndIntensity(glm::vec3& pHit, glm::vec3& dir, glm::vec3& lightIntensity, float& dist) const {
	dir = this->dir;
	dist = INFINITY;
	lightIntensity = color * intesnity;
}

SphereicalLight::SphereicalLight(glm::vec3 location, glm::vec3 color, float i) {
	this->location = location;
	this->color = color;
	this->intesnity = i;
}

void SphereicalLight::getDirectionAndIntensity(glm::vec3& pHit, glm::vec3& dir, glm::vec3& lightIntensity, float& dist) const {
	glm::vec3 direction = pHit - location;
	float r2 = glm::dot(direction, direction);
	dist = std::sqrt(r2);
	dir.x = direction.x / dist; dir.y = direction.y / dist; dir.z = direction.z / dist;
	lightIntensity = color * this->intesnity / (float)(4 * M_PI * r2);
}
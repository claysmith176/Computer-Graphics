#pragma once

#include <GL/glm/glm.hpp>
#include <GL/glm/vec3.hpp>;

class Light {	
public:
	glm::vec3 color;
	float intesnity;
	virtual void getDirectionAndIntensity(glm::vec3& pHit, glm::vec3& dir, glm::vec3& lightIntensity, float& dist) const = 0;
};

class DistantLight : public Light {
public:
	glm::vec3 dir;
	DistantLight(glm::vec3 dir, glm::vec3 color, float i);
	void getDirectionAndIntensity(glm::vec3& pHit, glm::vec3& dir, glm::vec3& lightIntensity, float& dist) const;
};

class SphereicalLight : public Light {
public:
	glm::vec3 location;
	SphereicalLight(glm::vec3 location, glm::vec3 color, float i);
	void getDirectionAndIntensity(glm::vec3& pHit, glm::vec3& dir, glm::vec3& lightIntensity, float& dist) const; 
};
#define _USE_MATH_DEFINES

#include <iostream>
#include <stdlib.h>
#include <cstdlib> 
#include <cstdio> 
#include <fstream> 
#include <cmath> 
#include <vector> 
#include <string>
#include <chrono>

#include <GL/glm/glm.hpp>
#include <GL/glm/vec3.hpp>;
#include <GL/glm/vec3.hpp> // glm::vec3
#include <GL/glm/vec4.hpp> // glm::vec4
#include <GL/glm/mat4x4.hpp> // glm::mat4
#include <GL/glm/gtc/matrix_transform.hpp>
#include "GL/glm/ext.hpp"

class ray {
public:
    ray() {}
    ray(glm::vec3 origin, glm::vec3 direction) 
        : orig_(origin), direction_(direction) {}

    glm::vec3 origin() const { return orig_; }
    glm::vec3 dir() const { return direction_; }

    glm::vec3 at(float t) {
        return orig_ + t * direction_;
    }
private:
    glm::vec3 orig_ = glm::vec3(0,0,0);
    glm::vec3 direction_ = glm::vec3(0,0,0);
};

void render(float iWidth, float iHeight, float FOV, glm::vec3 origin, glm::vec3 center, glm::vec3 up, std::string name) {
    glm::mat4 camera2World = glm::inverse(glm::lookAt(origin, center, up));
    glm::vec3 w = glm::normalize(origin - center);
    glm::vec3 u = glm::normalize(glm::cross(up, w));
    glm::vec3 v = glm::cross(w, u);

    glm::vec3* frameBuffer = new glm::vec3[iHeight * iWidth];
    for (int i = 0; i < iWidth * iHeight; i++) {
        frameBuffer[i] = { 0,0,0 };
    }



    float aspect = iWidth / iHeight;
    float left = std::tanf((FOV/2) * (M_PI/180));
    float top = left / aspect;
    
    glm::vec3 horizontal = glm::vec3(2*left, 0,0 ) * u;
    glm::vec3 vertical = glm::vec3(0, 2 * top, 0) * v;

    glm::vec3 bot_left_corner = origin - horizontal/2.0f - vertical/2.0f- w;

    for (int i = 0; i < iHeight; i++) {
        for (int j = 0; j < iWidth; j++) {
            glm::vec3 dir = bot_left_corner + vertical * (float(i) / iHeight) + horizontal * (float(j) / iWidth) - origin;
            ray r(origin, glm::normalize(dir));

        }
    }
}

int main()
{
    std::cout << "Hello World!\n";
}

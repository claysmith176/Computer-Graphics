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
#include <GL/glm/vec3.hpp> // glm::vec3
#include <GL/glm/vec4.hpp> // glm::vec4
#include <GL/glm/mat4x4.hpp> // glm::mat4
#include <GL/glm/gtc/matrix_transform.hpp>
#include "GL/glm/ext.hpp"

#include "Ray.h"
#include "Camera.h"
#include "Sphere.h"
#include "Hittable.h"

std::vector<Sphere*> objs;
double t_min = 0;
double t_max = 1000000;

struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

glm::vec3 cast_ray(const Ray& r) {
    hit_data rec;

    for (int i = 0; i < objs.size(); i++) {
        if (objs[i]->hit(r, t_min, t_max, rec)) {
            return glm::vec3(255, 0, 0);
        }
    }
    float y = r.dir().y;
    float t = ((y + 1) / 2);
    return (1.0 - t)* glm::vec3(0, 0, 255) + t * glm::vec3(128, 128, 128);
}

void render(float iWidth, float iHeight, Camera cam, std::string name) {
    color* frameBuffer = new color[iHeight * iWidth];
    for (int i = 0; i < iWidth * iHeight; i++) {
        frameBuffer[i] = { 0,0,0 };
    }

    for (int i = 0; i < iHeight; i++) {
        for (int j = 0; j < iWidth; j++) {
            Ray r = cam.get_ray(float(j) / (iWidth - 1), float(i) / (iHeight - 1));
            glm::vec3 colorVector = cast_ray(r);
            int thing = i * iWidth + j;
            color c;
            c.r = (unsigned char)colorVector.x;
            c.g = (unsigned char)colorVector.y;
            c.b = (unsigned char)colorVector.z;
            frameBuffer[thing] = c;
        }
        std::cout << std::endl;
    }

    std::ofstream ofs;
    ofs.open("C:/Users/smith/source/repos/Computer-Graphics/peteRayTracer/Video/" + name + ".ppm", std::ios_base::out | std::ios_base::binary);
    ofs << "P6 " << iWidth << " " << iHeight << " 255 ";
    ofs.write((char*)frameBuffer, iHeight * iWidth * 3);
    ofs.close();
    delete[] frameBuffer;

}

int main()
{
    objs.push_back(new Sphere(glm::vec3(0, 0, -5), 3));
    Camera cam = Camera(120, 16.0f / 9.0f, glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
    render(853, 480, cam, "out");
}

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
#include "Hittable_List.h"

Hittable_List world;
const double t_min = 0;
const double t_max = 1000000;
const int samples_per_pixel = 100;
const int max_depth = 4;

struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

void print_vec(glm::vec3 vec) {
    std::cout << vec.x << ", " << vec.y << ", " << vec.z << std::endl;
}

glm::vec3 write_color(glm::vec3 color) {
    float scale = 1.0f / (samples_per_pixel * 255);
    color *= scale;

    // gamma correction
    glm::vec3 new_color(std::sqrtf(color.x), std::sqrtf(color.y), std::sqrtf(color.z));
    return new_color * 255.0f;
}

float random_float() {
    return (float)rand() / (RAND_MAX + 1.0);
}

float random_float(float min, float max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_float();
}

glm::vec3 random_unit_sphere() {
    while (true) {
        glm::vec3 vec(random_float(-1, 1), random_float(-1, 1), random_float(-1, 1));
        if (glm::dot(vec, vec) <= 1) {
            return vec;
        }
    }
}


glm::vec3 cast_ray(const Ray& r, int depth) {
    hit_data rec;

    if (depth == 0) {
        return glm::vec3(0, 0, 0);
    }
    if (world.hit(r, 0.001, t_max, rec)) {
        glm::vec3 tangent = rec.point + rec.normal + random_unit_sphere();
        glm::vec3 test = tangent - rec.point;
        return 0.5 * cast_ray(Ray(rec.point, glm::normalize(tangent - rec.point)), depth - 1);

        //return glm::vec3(255, 255, 255) * (glm::dot(rec.normal, r.dir()) * -1);
    }
    float y = r.dir().y;
    float t = ((y + 1) / 2);
    //return glm::vec3(200, 200, 200);
    return (1.0 - t) * glm::vec3(255, 255, 255) + t * glm::vec3(127, 178, 255);
    return (1.0 - t)* glm::vec3(0, 0, 255) + t * glm::vec3(128, 128, 128);
}

void render(float iWidth, float iHeight, Camera cam, std::string name) {
    color* frameBuffer = new color[iHeight * iWidth];
    for (int i = 0; i < iWidth * iHeight; i++) {
        frameBuffer[i] = { 0,0,0 };
    }

    for (int i = 0; i < iHeight; i++) {
        for (int j = 0; j < iWidth; j++) {
            if ( i == 178 && j == 197) {
                int tqe = 4;
            }
            glm::vec3 colorVector;
            for (int a = 0; a < samples_per_pixel; a++) {
                Ray r = cam.get_ray(float(j + random_float()) / (iWidth - 1), float(i + random_float()) / (iHeight - 1));
                colorVector += cast_ray(r, max_depth);
            }
            colorVector = write_color(colorVector);
            color c;
            int thing = i * iWidth + j;
            c.r = (unsigned char)colorVector.x;
            c.g = (unsigned char)colorVector.y;
            c.b = (unsigned char)colorVector.z;
            frameBuffer[thing] = c;
        }
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
    srand(static_cast <unsigned> (time(0)));

    world.add(std::make_shared<Sphere>(glm::vec3(0, 0, -1), 0.5));
    world.add(std::make_shared<Sphere>(glm::vec3(0, -100.5, -1), 100));

    Camera cam = Camera(120, 16.0f / 9.0f, glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
    render(400, 224, cam, "out");
}

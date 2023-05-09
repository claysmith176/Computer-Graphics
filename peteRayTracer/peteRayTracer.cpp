#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <stdlib.h>
#include <cstdlib> 
#include <cstdio> 
#include <fstream> 
#include <cmath> 
#include <vector> 
#include <string>
#include <chrono>
#include <thread>

#include <GL/glm/glm.hpp>
#include <GL/glm/vec3.hpp>;
#include <GL/glm/vec3.hpp> // glm::vec3
#include <GL/glm/vec4.hpp> // glm::vec4
#include <GL/glm/mat4x4.hpp> // glm::mat4
#include <GL/glm/gtc/matrix_transform.hpp>
#include "GL/glm/ext.hpp"

#include <algorithm>
#include <functional>
#include <array>
#include <string_view>

#include "stb_image.h"
#include "Util.h"
#include "vec3.h"
#include "Ray.h"
#include "Perlin.h"
#include "Texture.h"
#include "aabb.h"
#include "Camera.h"
#include "Hittable.h"
#include "Material.h"
#include "Rectangle.h"
#include "Instance.h"
#include "Sphere.h"
#include "Constant_Volume.h"
#include "bvh.h"

bvh world;
const double t_min = 0;
const double t_max = 2000;
const int samples_per_pixel = 10;
const int max_depth = 4;
const vec3 background_color(0, 0, 0);

float total_time = 0;
float average = 0;

struct other_color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

Hittable_List random_scene() {
    Hittable_List objects;

    auto red = std::make_shared<Lambertian>(color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(color(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(color(.12, .45, .15));
    auto light = std::make_shared<Diffuse_Light>(color(15, 15, 15));

    objects.add(std::make_shared<YZ_Rect>(0, 555, 0, 555, 555, green));
    objects.add(std::make_shared<YZ_Rect>(0, 555, 0, 555, 0, red));
    objects.add(std::make_shared<XZ_Rect>(213, 343, 227, 332, 554, light));
    objects.add(std::make_shared<XZ_Rect>(0, 555, 0, 555, 0, white));
    objects.add(std::make_shared<XZ_Rect>(0, 555, 0, 555, 555, white));
    objects.add(std::make_shared<XY_Rect>(0, 555, 0, 555, 555, white));

    std::shared_ptr<Hittable> box1 = std::make_shared<Box>(vec3(0, 0, 0), vec3(165, 330, 165), white);
    box1 = std::make_shared<Y_Rotate>(box1, 15);
    box1 = std::make_shared<Translate>(box1, vec3(265, 0, 295));
    objects.add(box1);

    std::shared_ptr<Hittable> box2 = std::make_shared<Box>(vec3(0, 0, 0), vec3(165, 165, 165), white);
    box2 = std::make_shared<Y_Rotate>(box2, -18);
    box2 = std::make_shared<Translate>(box2, vec3(130, 0, 65));
    objects.add(box2);

    return objects;
}

vec3 cast_ray(const Ray& r, int depth) {
    hit_data rec;
    if (depth == 0) {
        return vec3(0, 0, 0);
    }  
    if (!world.hit(r, 0.001, t_max, rec)) {
        return background_color;
    }
    Ray scattered;
    vec3 attenuation;
    vec3 emit = rec.mat_ptr->emitted(rec.u, rec.v, rec.point);
    if (!rec.mat_ptr->scatter(r, attenuation, scattered, rec)) {
        return emit;
    }

    return emit + attenuation * cast_ray(scattered, depth - 1);
}

void render(int iWidth, int total_height, int start_height, int end_height, Camera cam, void* void_pointer) {
    other_color* frameBuffer = (other_color*)void_pointer;
    for (int i = start_height; i < end_height; i++) {
        //std::cerr << "\rScanlines remaining: " << total_height - i << ' ' << std::flush;
        for (int j = 0; j < iWidth; j++) {
            vec3 colorVector;
            for (int a = 0; a < samples_per_pixel; a++) {
                Ray r = cam.get_ray(float(j + random_float()) / (iWidth - 1), float(i + random_float()) / (total_height - 1));
                colorVector += cast_ray(r, max_depth);     
            }

            colorVector = write_color(colorVector, samples_per_pixel);
            other_color c;
            int thing = i * iWidth + j;
            c.r = (unsigned char)colorVector.x();
            c.g = (unsigned char)colorVector.y();
            c.b = (unsigned char)colorVector.z();
            frameBuffer[thing] = c;

            if (i == 160 && j == 160) {
                vec3 lookat = vec3(0.0, 0.0, 1.0);
                vec3 origin = vec3(0.0, 0.0, 0.0);
                vec3 u = vec3(0.0, 1.0, 0.0);
                Camera c = Camera(60.0f, 1.0f, 1, 1, origin, lookat, u);
                Ray test = c.get_ray(float(j) / (500 - 1), float(i) / (500 - 1));
                std::cout << test.dir()[0] << ", " << test.dir()[1] << ", " << test.dir()[2] << std::endl;
            }
        }
    }
}

void make_movie(Camera cam, int image_width, int image_height, std::string name) {
    unsigned int nthreads = std::thread::hardware_concurrency();
    int job_size = (image_height) / nthreads;
    int left_over = image_height - (nthreads * (job_size));
    std::vector<std::thread> jobs;
    other_color* frameBuffer = new other_color[image_height * image_width];
    for (int i = 0; i < image_height * image_width; i++) {
        frameBuffer[i] = { 0,0,0 };
    }

    int curr = 0;
    for (int i = 0; i < nthreads; i++) {
        if (i == nthreads - 1) {
            jobs.push_back(std::thread(render, image_width, image_height, curr, curr + job_size + left_over, cam, frameBuffer));
        }
        else {
            jobs.push_back(std::thread(render, image_width, image_height, curr, curr + job_size, cam, frameBuffer));
        }
        curr += job_size;;
    }

    for (int i = 0; i < jobs.size(); i++) {
        jobs[i].join();
        std::cout << "\rThread " << i << " Done " << std::endl;
    }
    std::cout << std::flush << "Image " << name << " Done " << std::endl;
    std::ofstream ofs;
    ofs.open("C:/Users/smith/source/repos/Computer-Graphics/peteRayTracer/Video/" + name + ".ppm", std::ios_base::out | std::ios_base::binary);
    ofs << "P6 " << image_width << " " << image_height << " 255 ";
    ofs.write((char*)frameBuffer, image_height * image_width * 3);
    ofs.close();
    delete[] frameBuffer;
}

int main()
{
    srand(static_cast <unsigned> (time(0)));
    auto startTime = std::chrono::high_resolution_clock::now();

    world = bvh(random_scene(), 0, 1);

    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278, 278, 0);
    vec3 up(0, 1, 0);
    float hfov = 40;

    const float aspect_ratio = 1;
    const int image_height = 800;
    const int image_width = static_cast<int>(image_height * aspect_ratio);;
    Camera cam = Camera(hfov, aspect_ratio, 0, 1, lookfrom, lookat, up);

    for (int i = 0; i < 1; i++) {
        make_movie(cam, image_width, image_height, std::to_string(i));
        lookfrom = rotate(1, up, lookfrom);
        cam = Camera(34, aspect_ratio, 0, 1, lookfrom, lookat, up);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "Total Time " << ((int)duration.count() / 1000) / 60 << ":" << ((int)duration.count() / 1000) % 60 << "." << duration.count() % 1000 << std::endl;
}
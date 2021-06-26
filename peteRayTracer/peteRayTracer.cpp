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

#include "Util.h"
#include "vec3.h"
#include "Ray.h"

#include "Camera.h"
#include "Hittable.h"
#include "Material.h"

Hittable_List world;
const double t_min = 0;
const double t_max = 50;
const int samples_per_pixel = 100;
const int max_depth = 4;

float total_time = 0;
float average = 0;

struct other_color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

Hittable_List random_scene() {
    Hittable_List world;

    auto ground_material = std::make_shared<Lambertian>(vec3(0.5, 0.5, 0.5));
    world.add(std::make_shared<Sphere>(vec3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_float();
            vec3 center(a + 0.9 * random_float(), 0.2, b + 0.9 * random_float());

            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = vec3(random_float(0.15, 1), random_float(0.15, 1), random_float(0.15, 1)) /* * vec3(random_float(0.5, 1), random_float(0.5, 1), random_float(0.5, 1)) */;
                    sphere_material = std::make_shared<Lambertian>(albedo);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = vec3(random_float(0.25, 1), random_float(0.25, 1), random_float(0.25, 1));
                    auto fuzz = random_float(0, 0.5);
                    sphere_material = std::make_shared<Metal>(albedo, fuzz);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = std::make_shared<Dielectric>(1.5);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    world.add(std::make_shared<Sphere>(vec3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(vec3(0.4, 0.6, 0.3));
    world.add(std::make_shared<Sphere>(vec3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(vec3(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<Sphere>(vec3(4, 1, 0), 1.0, material3));

    return world;
}

vec3 cast_ray(const Ray& r, int depth) {
    hit_data rec;
    if (depth == 0) {
        return vec3(0, 0, 0);
    }  
    if (world.hit(r, 0.001, t_max, rec)) {
        Ray scattered;
        vec3 attenuation;
        if (rec.mat_ptr->scatter(r, attenuation, scattered, rec)) {
            return attenuation * cast_ray(scattered, depth - 1);
        }
        return vec3(0, 0, 0);
    }
    float y = r.dir().y();
    float t = ((y + 1) / 2);

    return (1.0 - t) * vec3(255, 255, 255) + t * vec3(127, 178, 255);
}

void render(float iWidth, float iHeight, Camera cam, std::string name) {
    other_color* frameBuffer = new other_color[iHeight * iWidth];
    for (int i = 0; i < iWidth * iHeight; i++) {
        frameBuffer[i] = { 0,0,0 };
    }

    for (int i = 0; i < iHeight; i++) {
        std::cerr << "\rScanlines remaining: " << iHeight - i << ' ' << std::flush;
        for (int j = 0; j < iWidth; j++) {
            vec3 colorVector;
            auto startTime = std::chrono::high_resolution_clock::now();
            for (int a = 0; a < samples_per_pixel; a++) {
                Ray r = cam.get_ray(float(j + random_float()) / (iWidth - 1), float(i + random_float()) / (iHeight - 1));
                colorVector += cast_ray(r, max_depth);                
            }

            colorVector = write_color(colorVector, samples_per_pixel);
            other_color c;
            int thing = i * iWidth + j;
            c.r = (unsigned char)colorVector.x();
            c.g = (unsigned char)colorVector.y();
            c.b = (unsigned char)colorVector.z();
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
    auto startTime = std::chrono::high_resolution_clock::now();

    /*auto material_ground = std::make_shared<Lambertian>(glm::vec3(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<Dielectric>(1.5);
    auto material_left = std::make_shared<Dielectric>(1.5);
    auto material_right = std::make_shared<Metal>(glm::vec3(0.8, 0.6, 0.2), 1);
    world.add(std::make_shared<Sphere>(glm::vec3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(std::make_shared<Sphere>(glm::vec3(0.0, 0.0, -1.0), 0.5, material_center));
    world.add(std::make_shared<Sphere>(glm::vec3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(std::make_shared<Sphere>(glm::vec3(1.0, 0.0, -1.0), 0.5, material_right));*/

    world = random_scene();

    vec3 lookfrom(13, 2, 3);
    vec3 lookat(0, 0, 0);
    vec3 up(0, 1, 0);

    const float aspect_ratio = 16.0f / 9.0f;
    const int image_width = 1080;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    Camera cam = Camera(34, aspect_ratio, lookfrom, lookat, up);
    render(image_width, image_height, cam, "out");  

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "Total Time " << ((int)duration.count() / 1000) / 60 << ":" << ((int)duration.count() / 1000) % 60 << "." << duration.count() % 1000 << std::endl;
}

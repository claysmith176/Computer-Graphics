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
#include "Sphere.h"
#include "bvh.h"

bvh world;
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
    Hittable_List rand_scene;

    auto texture = std::make_shared<Checkered_Texture>(vec3(0.1, 0.2, 0.8), vec3(0.7, 0.1, 0.7));
    auto ground_material = std::make_shared<Lambertian>(texture);
    //auto ground_material = std::make_shared<Lambertian>(vec3(0.5, 0.5, 0.5));
    rand_scene.add(std::make_shared<Sphere>(vec3(0, -1000, 0), 1000, ground_material));

    //for (int a = -11; a < 11; a++) {
    //    for (int b = -11; b < 11; b++) {
    //        auto choose_mat = random_float();
    //        vec3 center(a + 0.9 * random_float(), 0.2, b + 0.9 * random_float());

    //        if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
    //            std::shared_ptr<Material> sphere_material;

    //            if (choose_mat < 0.8) {
    //                // diffuse
    //                auto albedo = vec3(random_float(0.15, 1), random_float(0.15, 1), random_float(0.15, 1)) /* * vec3(random_float(0.5, 1), random_float(0.5, 1), random_float(0.5, 1)) */;
    //                sphere_material = std::make_shared<Lambertian>(albedo);
    //                rand_scene.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
    //                //auto center2 = center + vec3(0, random_float(0, .5), 0);
    //                //rand_scene.add(std::make_shared<Moving_sphere>(
    //                //    center, center2, 0.0, 1.0, 0.2, sphere_material));
    //            }
    //            else if (choose_mat < 0.95) {
    //                // metal
    //                auto albedo = vec3(random_float(0.25, 1), random_float(0.25, 1), random_float(0.25, 1));
    //                auto fuzz = random_float(0, 0.5);
    //                sphere_material = std::make_shared<Metal>(albedo, fuzz);
    //                rand_scene.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
    //            }
    //            else {
    //                // glass
    //                sphere_material = std::make_shared<Dielectric>(1.5);
    //                rand_scene.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
    //            }
    //        }
    //    }
    //}

    auto material1 = std::make_shared<Dielectric>(1.5);
    rand_scene.add(std::make_shared<Sphere>(vec3(0, 1, 0), 1.0, material1));

    //auto material2 = std::make_shared<Lambertian>(texture);
    auto material2 = std::make_shared<Lambertian>(vec3(0.4, 0.6, 0.3));
    rand_scene.add(std::make_shared<Sphere>(vec3(-6, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(vec3(0.7, 0.6, 0.5), 0.0);
    rand_scene.add(std::make_shared<Sphere>(vec3(4, 1, 0), 1.0, material3));

    return rand_scene;
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
        std::cerr << "\rThread " << i << " Done " << std::flush;
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

    Hittable_List objects;

    auto pertext = std::make_shared<Noise_Texture>(5.0);
    auto obama = std::make_shared<Image_Texture>("obama.png");
    //objects.add(std::make_shared<Sphere>(vec3(0, -1000, 0), 1000, std::make_shared<Lambertian>(pertext)));
    objects.add(std::make_shared<Sphere>(vec3(0, 2, 0), 2, std::make_shared<Lambertian>(obama)));
    world = bvh(objects, 0, 1);
    //world = bvh(random_scene(), 0, 1);

    //vec3 lookfrom(13, 2, 3);
    vec3 lookfrom(0, 2, -14);
    vec3 lookat(0, 2, 0);
    vec3 up(0, 1, 0);

    const float aspect_ratio = 16.0f / 9.0f;
    const int image_height = 480;
    const int image_width = static_cast<int>(image_height * aspect_ratio);;
    //const int image_height = static_cast<int>(image_width / aspect_ratio);
    Camera cam = Camera(34, aspect_ratio, 0, 1, lookfrom, lookat, up);

    for (int i = 0; i < 180; i++) {
        make_movie(cam, image_width, image_height, std::to_string(i));
        lookfrom = rotate(1, up, lookfrom);
        cam = Camera(34, aspect_ratio, 0, 1, lookfrom, lookat, up);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "Total Time " << ((int)duration.count() / 1000) / 60 << ":" << ((int)duration.count() / 1000) % 60 << "." << duration.count() % 1000 << std::endl;
}
// spRayTracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

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

#include "Object.h"
#include "Light.h"

using namespace std;

struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};


const int objectNumber = 3;
vector<Object*> objects;
vector<Light*> lights;
const glm::vec3 backgroundColor(57, 120, 200);


glm::mat3 rotate(const float degrees, const glm::vec3& axis) {
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;
    float radians = degrees * (M_PI / 180);

    glm::mat3 xmat(1.0f);
    glm::mat3 ymat = glm::outerProduct(axis, axis);
    glm::mat3 zmat(0, z, -y, -z, 0, x, y, -x, 0);

    xmat = xmat * cos(radians);
    ymat = ymat * (1 - cos(radians));
    zmat = zmat * sin(radians);

    glm::mat3 test = xmat + ymat + zmat;
    return test;
}

glm::vec3 left(float degrees, glm::vec3 eye, glm::vec3 up) {
    glm::mat3 R = rotate(degrees, up);
    return R * eye;
    //up = up * R;
}

glm::vec3 reflect(glm::vec3& I, glm::vec3& N) {
    return I - 2.0f * (glm::dot(I, N) * N);
}

glm::vec3 castRay(glm::vec3 dir, glm::vec3 origin, int depth) {
    float distance = INFINITY;
    int hitObject = 0;
    if (depth >= 4) {
        return backgroundColor;
    }
    for (int i = 0; i < objects.size(); i++) {
        float currentD = INFINITY;
        if (objects[i]->intersect(origin, dir, currentD )) {
            if (currentD > 0 && currentD < distance) {
                distance = currentD;
                hitObject = i;
            }
        }
    }

    if (distance < INFINITY) {
        glm::vec3 pHit = origin + (distance * dir);
        glm::vec3 normal;
        glm::vec2 tex;

        objects[hitObject]->getSurfaceData(pHit, normal, tex);

        glm::vec3 cWhite(255, 255, 255);
        glm::vec3 cObject = objects[hitObject]->color;
        glm::vec3 hitColor(0,0,0);

        if (objects[hitObject]->surfaceType == 0) {
            for (int j = 0; j < lights.size(); j++) {
                bool vis = true;
                glm::vec3 lightDir, lightIntensity;
                float lightDist;
                lights[j]->getDirectionAndIntensity(pHit, lightDir, lightIntensity, lightDist);
                float dotProduct = glm::dot(normal, (lightDir * -1.0f));
                if (dotProduct > 0) {
                    for (int i = 0; i < objects.size(); i++) {
                        float distance = INFINITY;
                        if (objects[i]->intersect(pHit + normal * 1e-2f, (lightDir * -1.0f), distance)) {
                            if (distance > 0 && distance < lightDist) {
                                vis = false;
                            }
                        }
                    }
                    hitColor += (float)vis * (objects[hitObject]->albedo / (float)M_PI) * lightIntensity * std::max(0.0f, dotProduct);
                }
            }
        }
        else if (objects[hitObject]->surfaceType == 1) {
            hitColor = .8f * castRay(reflect(dir, normal), pHit + normal * 1e-2f, depth + 1);
        }
        hitColor.x = std::min(255.0f, hitColor.x);
        hitColor.y = std::min(255.0f, hitColor.y);
        hitColor.z = std::min(255.0f, hitColor.z);
        return hitColor;
    }
    return backgroundColor;
}

void render(int iWidth, int iHeight, float FOV, glm::vec3 origin, glm::vec3 center, glm::vec3 up, std::string name) {

    glm::mat4 camera2World = glm::inverse(glm::lookAt(origin, center, up));

    color* frameBuffer = new color[iHeight * iWidth];
    for (int i = 0; i < iWidth * iHeight; i++) {
        frameBuffer[i] = { 0,0,0 };
    }
    float aspectRatio = iWidth / (float)iHeight;
    float scale = tan((FOV * (M_PI / 180))/2);

    for (int i = 0; i < iHeight; i++) {
        std::cerr << "\rScanlines remaining: " << iHeight - i << ' ' << std::flush;
        for (int j = 0; j < iWidth; j++) {
            glm::vec3 colorVector(0, 0, 0);
            for (int k = 0; k < 4; k++) {
                float val1 = (.25 + (.5 * (k / 2)));
                float val2 = (.25 + (.5 * (k % 2)));
                float px = (2 * (j + val1) / (float)iWidth - 1) * scale * aspectRatio;
                float py = (1 - 2 * (i + val2) / (float)iHeight) * scale;

                glm::vec4 dirH(px, py, -1, 1);
                dirH = camera2World * dirH;
                glm::vec3 dir(dirH.x / dirH.w, dirH.y / dirH.w, dirH.z / dirH.w);

                dir = dir - origin;
                dir = glm::normalize(dir);
                colorVector = colorVector + castRay(dir, origin, 0);
            }
            colorVector /= glm::vec3(4,4,4);
            color c;
            c.r = (unsigned char)colorVector.x;
            c.g = (unsigned char)colorVector.y;
            c.b = (unsigned char)colorVector.z;

            frameBuffer[i * iWidth + j] = c; 
        }
    }
    std::cout << "Render " << name << " Done" << std::endl;

    std::ofstream ofs;
    ofs.open("C:/Users/smith/source/repos/Computer-Graphics/spRayTracer/Video/" + name + ".ppm", std::ios_base::out | std::ios_base::binary);
    ofs << "P6 " << iWidth << " " << iHeight << " 255 ";
    ofs.write((char*)frameBuffer, iHeight * iWidth * 3);
    ofs.close();

    delete[] frameBuffer;
}

int main()
{
    //objects.push_back(new TriangleMesh("C:/Users/smith/source/repos/Computer-Graphics/spRayTracer/Objects/fox_rotate.obj", glm::vec3(.8, .8, .8), 0));
    //objects.push_back(new TriangleMesh("C:/Users/smith/source/repos/Computer-Graphics/spRayTracer/Objects/foxPlane.obj", glm::vec3(.5, .5, .5), 0));
    //objects.push_back(new TriangleMesh("C:/Users/smith/source/repos/Computer-Graphics/spRayTracer/Objects/porous_sphere.obj", glm::vec3(.5, .5, .5), 0));
    objects.push_back(new Sphere(glm::vec3(0, 0, -5), glm::vec3(192, 192, 192), 3, glm::vec3(.5, .5, .5), 0));
    //objects.push_back(new TriangleMesh("Objects/flatPlane.obj", glm::vec3(.5,.5,.5), 1));
    /*objects.push_back(new TriangleMesh("Objects/rightPlane.obj", glm::vec3(.5, .5, .5), 1));
    objects.push_back(new TriangleMesh("Objects/leftPlane.obj", glm::vec3(.5, .5, .5), 1));
    objects.push_back(new TriangleMesh("Objects/backPlane.obj", glm::vec3(.5, .5, .5), 1));
    objects.push_back(new TriangleMesh("Objects/frontPlane.obj", glm::vec3(.5, .5, .5), 1)); */
    
    //objects.push_back(new TriangleMesh("C:/Users/smith/source/repos/Computer-Graphics/spRayTracer/Objects/sphere.obj", glm::vec3(.5, .5, .5), 0));

    //lights.push_back(new DistantLight(glm::vec3(0, -1, -1), glm::vec3(255, 255, 255), 3));
    //lights.push_back(new DistantLight(glm::vec3(0, -1, 1), glm::vec3(255, 255, 255), 3));
    //lights.push_back(new DistantLight(glm::vec3(1, -1, 0), glm::vec3(255, 255, 255), 3));
    lights.push_back(new DistantLight(glm::normalize(glm::vec3(0, -1, -1)), glm::vec3(255, 255, 255), 3));

    std::cout << "Loaded" << std::endl;

    glm::vec3 eye(0, 0, 0);
    glm::vec3 center(0, 0, -1);
    glm::vec3 up(0, 1, 0);
    const float FOV = 120;
    auto startTime = chrono::high_resolution_clock::now();

    /*for (int i = 170; i < 180; i++) {
        glm::vec3 lightDir = glm::vec3(0, 0, -1) * rotate(i, glm::vec3(1, 0, 0));
        lights.clear();
        lights.push_back(new DistantLight(lightDir, glm::vec3(255, 255, 255), 3));

        auto renderStart = chrono::high_resolution_clock::now();
        render(853, 480, FOV, eye, center, up, std::to_string(i));
        auto renderStop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(renderStop - renderStart);
        std::cout << (int)duration.count() / 1000 << "." << duration.count() % 1000 << " Seconds" << std::endl;
        //eye = left(1, eye, glm::vec3(0, 1, 0));
    } */

    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
    std::cout << "Total Time "<< ((int)duration.count() / 1000) / 60 << ":" << ((int)duration.count() / 1000) % 60 << "." << duration.count() % 1000  << std::endl;

    render(853, 480, FOV, eye, center, up, "out.ppm");
    for (int i = 0; i < objects.size(); i++) {
        delete objects[i];
    }
}

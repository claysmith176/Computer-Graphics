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

#include <GL/glm/glm.hpp>
#include <GL/glm/vec3.hpp>;
#include <GL/glm/vec3.hpp> // glm::vec3
#include <GL/glm/vec4.hpp> // glm::vec4
#include <GL/glm/mat4x4.hpp> // glm::mat4
#include <GL/glm/gtc/matrix_transform.hpp>
#include "GL/glm/ext.hpp"

#include "Object.h"

struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct Circle {
    glm::vec3 center;
    float radius;
    color c;
};

const int objectNumber = 6;
Object* objectsTest[objectNumber];

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

glm::vec3 castRay(glm::vec3 dir, glm::vec3 origin) {
    float distance = INFINITY;
    int hitObject = 0;
    for (int i = 0; i < objectNumber; i++) {
        float currentD = INFINITY;
        if (objectsTest[i]->intersect(origin, dir, currentD )) {
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

        objectsTest[hitObject]->getSurfaceData(pHit, normal, tex);

        glm::vec3 cWhite(255, 255, 255);
        glm::vec3 cObject = objectsTest[hitObject]->color;

        float mixValue = (fmodf(tex.x * 2, 1) > 0.5) ^ (fmodf(tex.y * 2, 1) > 0.5);
        glm::vec3 hitColor = cWhite* (1 - mixValue) + cObject * mixValue;
        return hitColor;
    }
    return glm::vec3(0, 0, 0);
}

void render(int iWidth, int iHeight, float FOV, glm::vec3 origin, std::string name) {
    glm::vec3 center(0, 0, 0);
    glm::vec3 up(0, 1, 0);

    glm::vec3 w = glm::normalize(origin - center);
    glm::vec3 u = glm::normalize(glm::cross(up, w));
    glm::vec3 v = glm::cross(w, u);

    glm::mat4 camera2World = glm::inverse(glm::lookAt(origin, center, glm::vec3(0, 1, 0)));

    color* frameBuffer = new color[iHeight * iWidth];
    for (int i = 0; i < iWidth * iHeight; i++) {
        frameBuffer[i] = { 0,0,0 };
    }
    float aspectRatio = iWidth / (float)iHeight;
    float scale = tan((FOV * (M_PI / 180))/2);

    for (int i = 0; i < iHeight; i++) {
        for (int j = 0; j < iWidth; j++) {
            if (j == 422 && i == 633) {
                int ad = 45;
               }

            float px = (2 * (j + 0.5) / (float)iWidth-1) * scale * aspectRatio;
            float py = (1 - 2 * (i + 0.5) / (float)iHeight) * scale;
            
            glm::vec4 dirH(px, py, -1, 1);
            dirH = camera2World * dirH;
            glm::vec3 dir(dirH.x / dirH.w, dirH.y / dirH.w, dirH.z / dirH.w);

            dir = dir - origin;
            dir = glm::normalize(dir);
            
            glm::vec3 colorVector = castRay(dir, origin);
            color c;
            c.r = (unsigned char)colorVector.x;
            c.g = (unsigned char)colorVector.y;
            c.b = (unsigned char)colorVector.z;

            frameBuffer[i * iWidth + j] = c; 
        }
    }
    std::cout << "Render " << name.at(0) << " Done" << std::endl;

    std::ofstream ofs;
    ofs.open("./Video/" + name, std::ios_base::out | std::ios_base::binary);
    ofs << "P6 " << iWidth << " " << iHeight << " 255 ";
    ofs.write((char*)frameBuffer, iHeight * iWidth * 3);
    ofs.close();

    delete[] frameBuffer;
}

int main()
{
    Sphere mySphere(glm::vec3(0, 0, 0), glm::vec3(255, 0, 0), 2);
    std::srand(time(0));
    for (int i = 0; i < objectNumber-3; i++) {
        float x, y, z;
        x = std::rand() % 8 - 4;
        y = std::rand() % 4;
        z = std::rand() % 8 - 4;
        glm::vec3 cent(x, y, z);
        glm::vec3 colr(std::rand() % 126 + 126, std::rand() % 126 + 126, std::rand() % 126 + 126);
        objectsTest[i] = new Sphere(cent, colr, 1.5);
    }
    objectsTest[objectNumber - 3] = new Plane(glm::vec3(0, 1, 0), glm::vec3(0, -3, 0), glm::vec3(126, 126, 0));
    objectsTest[objectNumber - 2] = new Plane(glm::normalize(glm::vec3(-5, 0, 5)), glm::vec3(5, 0, -5), glm::vec3(64, 0, 64));
    objectsTest[objectNumber - 1] = new Plane(glm::normalize(glm::vec3(5, 0, 5)), glm::vec3(-5, 0, -5), glm::vec3(0, 126, 126));


    glm::vec3 eye(0, 1, 7);
    eye = left(-60, eye, glm::vec3(0, 1, 0));
    for (int i = 0; i < 120; i++) {
        render(1920, 1080, 90, eye, std::to_string(i) + ".ppm");
        eye = left(1, eye, glm::vec3(0, 1, 0));
    }
    //render(844, 844, 90, eye, "output.ppm");
    for (int i = 0; i < objectNumber; i++) {
        delete objectsTest[i];
    }
}

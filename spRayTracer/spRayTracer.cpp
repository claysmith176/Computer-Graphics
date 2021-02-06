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

using namespace std;

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

const int objectNumber = 10;
Object* objects[objectNumber];

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

        float mixValue = (fmodf(tex.x * 2, 1) > 0.5) ^ (fmodf(tex.y * 2, 1) > 0.5);
        glm::vec3 hitColor = cWhite* (1 - mixValue) + cObject * mixValue;
        return hitColor;
    }
    return glm::vec3(0, 0, 0);
}

void render(int iWidth, int iHeight, float FOV, glm::vec3 origin, std::string name) {
    glm::vec3 center(0, 0, 0);
    glm::vec3 up(0, 1, 0);

    glm::mat4 camera2World = glm::inverse(glm::lookAt(origin, center, glm::vec3(0, 1, 0)));

    color* frameBuffer = new color[iHeight * iWidth];
    for (int i = 0; i < iWidth * iHeight; i++) {
        frameBuffer[i] = { 0,0,0 };
    }
    float aspectRatio = iWidth / (float)iHeight;
    float scale = tan((FOV * (M_PI / 180))/2);

    for (int i = 0; i < iHeight; i++) {
        for (int j = 0; j < iWidth; j++) {
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
    std::cout << "Render " << name << " Done" << std::endl;

    std::ofstream ofs;
    ofs.open("./Video/" + name + ".ppm", std::ios_base::out | std::ios_base::binary);
    ofs << "P6 " << iWidth << " " << iHeight << " 255 ";
    ofs.write((char*)frameBuffer, iHeight * iWidth * 3);
    ofs.close();

    delete[] frameBuffer;
}

int main()
{
    std::srand(time(0));

    glm::vec3 v0(-5, -3, -5);
    glm::vec3 v1(5, -3, -5);
    glm::vec3 v2(5, -3, 5);
    glm::vec3 v3(-5,-3, 5);
    glm::vec3 v4(0, 5, 0);
    glm::vec3 red(255, 0, 0);
    glm::vec3 green(0, 255, 0);
    glm::vec3 blue(0, 0, 255);
    glm::vec3 white(255, 255, 255);

    objects[0] = new Triangle(v0, v1, v4, red);
    objects[1] = new Triangle(v0, v3, v4, green);
    objects[2] = new Triangle(v2, v1, v4, blue);
    objects[3] = new Triangle(v2, v3, v4, white);
    objects[4] = new Sphere(glm::vec3(0, 6, 0), glm::vec3(200, 0, 200), 1);

    /*for (int i = 0; i < objectNumber-3; i++) {
        float x, y, z;
        x = std::rand() % 8 - 4;
        y = std::rand() % 4;
        z = std::rand() % 8 - 4;
        glm::vec3 cent(x, y, z);
        glm::vec3 colr(std::rand() % 126 + 126, std::rand() % 126 + 126, std::rand() % 126 + 126);
        objects[i] = new Sphere(cent, colr, 1.5);
    } */
    objects[5] = new Plane(glm::vec3(0, 1, 0), glm::vec3(0, -3, 0), glm::vec3(126, 126, 0));
    objects[6] = new Plane(glm::vec3(1, 0, 0), glm::vec3(-15, 0, 0), glm::vec3(64, 0, 64));
    objects[7] = new Plane(glm::vec3(-1, 0, 0), glm::vec3(15, 0, 0), glm::vec3(0, 126, 126));
    objects[8] = new Plane(glm::vec3(0, 0, 1), glm::vec3(0, 0, -15), glm::vec3(126, 126, 126));
    objects[9] = new Plane(glm::vec3(0, 0, -1), glm::vec3(0, 0, 15), glm::vec3(0, 200, 200));


    glm::vec3 eye(-9, 3, -9);
    //eye = left(-60, eye, glm::vec3(0, 1, 0));
    for (int i = 0; i < 360; i++) {
        render(1440, 810, 90, eye, std::to_string(i));
        eye = left(1, eye, glm::vec3(0, 1, 0));
    } 
    //render(1440, 810, 90, eye, "output.ppm");
    for (int i = 0; i < objectNumber; i++) {
        delete objects[i];
    }
}

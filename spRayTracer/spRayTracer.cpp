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

#include<GL/glm/glm.hpp>
#include<GL/glm/vec3.hpp>;
#include <GL/glm/vec3.hpp> // glm::vec3
#include <GL/glm/vec4.hpp> // glm::vec4
#include <GL/glm/mat4x4.hpp> // glm::mat4
#include <GL/glm/gtc/matrix_transform.hpp>
#include "GL/glm/ext.hpp"

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

const int objectNumber = 1;
const Circle objects[objectNumber] = {
    {glm::vec3(-1,2,-1), 1, {255,0,0}} /*,
    {glm::vec3(-1,1,0), 1, {0,255,0}},
    {glm::vec3(0.1,-2,-2), 2, {0,0,255}},
    {glm::vec3(1,-1,0), 1, {255,255,255}} */
};

bool checkObject(glm::vec3 dir, glm::vec3 origin, int i, float &dist) {
    float x0, x1;
    glm::vec3 test = objects[i].center;
    glm::vec3 L = origin - objects[i].center; // 0, -1, -5
    float a = glm::dot(dir, dir);
    float b = 2 * glm::dot(dir, L);
    float c = glm::dot(L, L) - (objects[i].radius * objects[i].radius);


    float discr = b * b - 4 * a * c;
    if (discr < 0) {
        return false;
    }
    else if (discr == 0) {
        x0 = -b / (2 * a);
        dist = x0;
    }
    else {
        float q;
        if (b < 0) {
            q = -0.5 * (b + (float)std::sqrt(discr));
        }
        else {
            q = -0.5 * (b - (float)std::sqrt(discr));
        }
        x0 = q / a;
        x1 = c / q;

        dist = std::min(x0, x1);
    }
    return true;
}

int castRay(glm::vec3 dir, glm::vec3 origin) {
    float distance = INFINITY;
    int object = -1;
    for (int i = 0; i < objectNumber; i++) {
        float currentD;
        if (checkObject(dir, origin, i, currentD)) {
            if (currentD != 0) {
                distance = std::min(distance, currentD);
                object = i;
            }
        }
    }
    return object;
}

void render(int iWidth, int iHeight, float FOV) {
    glm::vec3 origin(1, 0, 1);
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
            if (j == 5 && i == 5) {
                int ad = 45;
               }

            float px = (2 * (j + 0.5) / (float)iWidth-1) * scale * aspectRatio;
            float py = (1 - 2 * (i + 0.5) / (float)iHeight) * scale;
            
            glm::vec4 dirH(px, py, -1, 1);
            dirH = camera2World * dirH;
            glm::vec3 dir(dirH.x / dirH.w, dirH.y / dirH.w, dirH.z / dirH.w);

            dir = dir - origin;
            dir = glm::normalize(dir);
            int object = castRay(dir, origin);
            if (object >= 0 ) {
                frameBuffer[i * iWidth + j] = objects[object].c;
            }
        }
    }
    std::cout << "Render Done" << std::endl;

    std::ofstream ofs;
    ofs.open("./output.ppm", std::ios_base::out | std::ios_base::binary);
    ofs << "P6 " << iWidth << " " << iHeight << " 255 ";
    ofs.write((char*)frameBuffer, iHeight * iWidth * 3);
    ofs.close();

    delete[] frameBuffer;
}

int main()
{
    render(844, 844, 90);
    //std::cout << "Hello World!\n";
}

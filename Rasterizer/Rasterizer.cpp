// Playground.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

#include<GL/glm/glm.hpp>
#include<GL/glm/vec3.hpp>;
#include <GL/glm/vec3.hpp> // glm::vec3
#include <GL/glm/vec4.hpp> // glm::vec4
#include <GL/glm/mat4x4.hpp> // glm::mat4
#include <GL/glm/gtc/matrix_transform.hpp>
#include "GL/glm/ext.hpp"

const int numOfTris = 6;
const glm::vec3 verts[5] = {
	{-5,0,-5}, {-5,0,5}, {5,0,-5},
	{5,0,5}, {0,15,0} };

const glm::vec3 vertColors[5] = {
	{255,0,0}, {0,255,0}, {255,0,0},
	{0,255,0}, {0,0,255} };

const uint32_t  tris[numOfTris * 3] = {
	0, 1, 3,
	0, 2, 3,
	0, 1, 4,
	1, 3, 4,
	3, 2, 4,
	2, 0, 4
};

struct color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

glm::mat3 rotate(const float degrees, const glm::vec3& axis) {
	// YOUR CODE FOR HW1 HERE
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

glm::vec3 computePixelCordinates(glm::vec3 pWorld,
	glm::mat4& world2Camera,
	float nearPlane,
	float l,
	float r,
	float t,
	float b,
	int iWidth,
	int iHeight) {

	//multiply to points by the matrix
	glm::vec4 pWorldHom = glm::vec4(pWorld.x, pWorld.y, pWorld.z, 1);
	glm::vec4 pCameraHom = world2Camera * pWorldHom;
	glm::vec3 pCamera = glm::vec3(pCameraHom.x / pCameraHom.w, pCameraHom.y / pCameraHom.w, pCameraHom.z / pCameraHom.w);

	// project the points to canvas space

	glm::vec2 pCanvas = glm::vec2(nearPlane * pCamera.x / -pCamera.z, nearPlane * pCamera.y / -pCamera.z);

	// convert to NDC
	glm::vec2 pNDC = glm::vec2(((2 * pCanvas.x) / (r - l)) - (r + l) / (r - l), ((2 * pCanvas.y) / (t - b)) - (t + b) / (t - b));
	//convert to Raster
	glm::vec3 pRaster;
	pRaster.x = ((pNDC.x + 1) / 2) * iWidth;
	pRaster.y = ((1 - pNDC.y)) / 2 * iHeight;
	pRaster.z = -pCamera.z;
	return pRaster;
}

void ComputeScreenCord(float FOV, float iWidth, float iHeight, float nearClippingPane,
	float &left, float &right, float &top, float &bottom) {

	float aspectRatio = iWidth / iHeight; // width / height

	float cWidth = 2 * tan((FOV / 2) * (M_PI / 180)) * nearClippingPane;
	right = cWidth / 2;
	left = -right;

	top = (cWidth / aspectRatio) / 2;
	bottom = -top;
}

float edgeFunction(glm::vec3 v0, glm::vec3 v1, glm::vec2 p) {
	return ((p.x - v0.x) * (v1.y - v0.y) - (p.y - v0.y) * (v1.x - v0.x));
}

bool checkClockwise(glm::vec3& v0, glm::vec3& v1, glm::vec3& v2) {
	glm::vec3 newv0, newv1, newv2;
	int test = (v0.x * v1.y) - (v1.x * v0.y);
	test += (v1.x * v2.y) - (v2.x * v1.y);
	test += (v2.x * v0.y) - (v0.x * v2.y);
	if (test > 0) {
		return true;
	}
	else {
		return false;
	}
}

void createImage(glm::vec3 eye, glm::vec3 center, glm::vec3 up, std::string name) {

	float FOV = 120;
	float nearPlane = 1;
	float farPlane = INFINITY;
	// 1500 844
	const int image_width = 1500, image_height = 844;
	float l, r, t, b;
	
	color* colors = new color[image_height * image_width];
	float *zBuffer= new float[image_height * image_width];

	for (int i = 0; i < image_height * image_width; i++) {
		colors[i].r = 0;
		colors[i].g = 0;
		colors[i].b = 0;

		zBuffer[i] = farPlane;
	}
	
	
	ComputeScreenCord(FOV, image_width, image_height, nearPlane, l, r, t, b);
	glm::mat4 world2Camera = glm::lookAt(eye, center, up);

	for (int i = 0; i < numOfTris; i++) {
		glm::vec3 vWorld0 = (verts[tris[i * 3]]);
		glm::vec3 vWorld1 = (verts[tris[i * 3 + 1]]);
		glm::vec3 vWorld2 = (verts[tris[i * 3 + 2]]);

		glm::vec3 v0Color = (vertColors[tris[i * 3]]);
		glm::vec3 v1Color = (vertColors[tris[i * 3 + 1]]);
		glm::vec3 v2Color = (vertColors[tris[i * 3 + 2]]);

		glm::vec3 v0Raster, v1Raster, v2Raster;

		//Project world verticies to raster space
		v0Raster = computePixelCordinates(vWorld0, world2Camera, nearPlane, l, r, t, b, image_width, image_height);
		v1Raster = computePixelCordinates(vWorld1, world2Camera, nearPlane, l, r, t, b, image_width, image_height);
		v2Raster = computePixelCordinates(vWorld2, world2Camera, nearPlane, l, r, t, b, image_width, image_height);
	
		// Ensure verticies are clockwise so the edge function works properly
		if (checkClockwise(v0Raster, v1Raster, v2Raster)) {
			glm::vec3 tmp = v0Raster;
			v0Raster = v1Raster;
			v1Raster = tmp;

			tmp = v0Color;
			v0Color = v1Color;
			v1Color = tmp;
		}
		
		// Divide vertex atributes by the vertex so cordinate for correct interpolation
		glm::vec3 v0ColorDivide; glm::vec3 v1ColorDivide; glm::vec3 v2ColorDivide;
		v0ColorDivide.x = v0Color.x / v0Raster.z; v0ColorDivide.y =v0Color.y / v0Raster.z; v0ColorDivide.z = v0Color.z / v0Raster.z;
		v1ColorDivide.x = v1Color.x / v1Raster.z; v1ColorDivide.y = v1Color.y / v1Raster.z; v1ColorDivide.z = v1Color.z / v1Raster.z;
		v2ColorDivide.x = v2Color.x / v2Raster.z; v2ColorDivide.y = v2Color.y / v2Raster.z; v2ColorDivide.z = v2Color.z / v2Raster.z;

		//Pre compute 1 / z
		v0Raster.z = 1.0 / v0Raster.z; v1Raster.z = 1.0 / v1Raster.z; v2Raster.z = 1.0 / v2Raster.z;

		float xmin = std::min(v0Raster.x, std::min(v1Raster.x, v2Raster.x));
		float xmax = std::max(v0Raster.x, std::max(v1Raster.x, v2Raster.x));
		float ymin = std::min(v0Raster.y, std::min(v1Raster.y, v2Raster.y));
		float ymax = std::max(v0Raster.y, std::max(v1Raster.y, v2Raster.y)); 
		
		if (xmin > image_width - 1 || xmax < 0 || ymin > image_height - 1 || ymax < 0) {
			continue;
		}
		
		int x0 = std::max(0, (int) std::floor(xmin));
		int x1 = std::min(image_width - 1, (int)std::floor(xmax));
		int y0 = std::max(0, (int)std::floor(ymin));
		int y1 = std::min(image_height - 1, (int)std::floor(ymax));

		float area = edgeFunction(v0Raster, v1Raster, v2Raster);

		for (int k = y0; k < y1; k++) {
			for (int j = x0; j < x1; j++) {
				if (k == 342 && j == 714) {
					int gnd = 55;
				} 

				float w0 = edgeFunction(v1Raster, v2Raster, glm::vec3(j, k, 0));
				float w1 = edgeFunction(v2Raster, v0Raster, glm::vec3(j, k, 0));
				float w2 = edgeFunction(v0Raster, v1Raster, glm::vec3(j, k, 0));
				if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
					w0 /= area;
					w1 /= area;
					w2 /= area;
					float oneOverZ = (v0Raster.z) * w0 + (v1Raster.z) * w1 + (v2Raster.z) * w2;
					float z = 1 / oneOverZ;

					if (z < zBuffer[k * image_width + j]) {
						zBuffer[k * image_width + j] = z;

						colors[k * image_width + j].r = (w0 * v0ColorDivide.x + w1 * v1ColorDivide.x + w2 * v2ColorDivide.x) * z;
						colors[k * image_width + j].g = (w0 * v0ColorDivide.y + w1 * v1ColorDivide.y + w2 * v2ColorDivide.y) * z;
						colors[k * image_width + j].b = (w0 * v0ColorDivide.z + w1 * v1ColorDivide.z + w2 * v2ColorDivide.z) * z;
					}
				}
			}
		}
	}

	std::cout << "Render Done" << std::endl;

	std::ofstream ofs;
	ofs.open("./VideoP/" + name, std::ios_base::out | std::ios_base::binary);
	ofs << "P6 " << image_width << " " << image_height << " 255 ";
	ofs.write((char*)colors, image_height * image_width * 3);
	ofs.close();

	delete[] zBuffer;
	delete[] colors;
}

void startAnimation() {
	glm::vec3 eye(-10, 2, -10);
	glm::vec3 up(0, 1, 0);
	glm::vec3 center(0, 5, 0);
	for (int i = 0; i < 360; i++) {
		std::string name = ("image" + std::to_string(i) + ".ppm");
		createImage(eye, center, up, name);
		eye = left(1, eye, up);
	}
}

void main(int argc, char** argv) {
	startAnimation();
	glm::vec3 test = left(22, glm::vec3(-10, 2, -10), glm::vec3(0, 1, 0));

	//createImage(test, glm::vec3(0, 5, 0), glm::vec3(0, 1, 0), "atest.ppm");
}
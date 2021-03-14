#define _USE_MATH_DEFINES


#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

#include <GL/glm/glm.hpp>
#include <GL/glm/vec3.hpp>;
#include <GL/glm/vec3.hpp> // glm::vec3
#include <GL/glm/vec4.hpp> // glm::vec4
#include <GL/glm/mat4x4.hpp> // glm::mat4
#include <GL/glm/gtc/matrix_transform.hpp>
#include "GL/glm/ext.hpp"

#include "Object.h"

Object::Object() {}

    Sphere::Sphere(glm::vec3 center, glm::vec3 color, float radius, glm::vec3 albedo, int surfaceType) {
        this->radius = radius;
        this->radius2 = radius * radius;
        this->center = center;
        this->color = color;
        this->albedo = albedo;
        this->surfaceType = surfaceType;
    }

    bool Sphere::intersect(const glm::vec3& orig, const glm::vec3& dir, float& dist)
    
    {
        float x0, x1;
        glm::vec3 test = center;
        glm::vec3 L = orig - center; // 0, -1, -5
        float a = glm::dot(dir, dir);
        float b = 2 * glm::dot(dir, L);
        float c = glm::dot(L, L) - (radius2);

        float discr = b * b - 4 * a * c;
        if (discr < 0) {
            dist = INFINITY;
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

    void Sphere::getSurfaceData(const glm::vec3& Phit, glm::vec3& Nhit, glm::vec2& tex) const {
        Nhit = Phit - center;
        Nhit = glm::normalize(Nhit);

        tex.x = (1 + atan2(Nhit.z, Nhit.x) / 3.15159) * 0.5;
        tex.y = acosf(Nhit.y) / 3.14159;
    }


    Plane::Plane(glm::vec3 normal, glm::vec3 center, glm::vec3 color, glm::vec3 albedo, int surfaceType) {
        this->normal = normal;
        this->center = center;
        this->color = color;
        this->albedo = albedo;
        this->surfaceType = surfaceType;
    }
    bool Plane::intersect(const glm::vec3& orig, const glm::vec3& dir, float& dist) {
        float denom = glm::dot(dir, normal);
        if (std::abs(denom) > 1e-6) {
            glm::vec3 p0l0 = center - orig;
            float num = glm::dot(p0l0, normal);
            dist = num / denom;
            if (dist > 0) {
                int adc = 5;
            }
            return true;
        }
        return false;
    }
    void Plane::getSurfaceData(const glm::vec3& Phit, glm::vec3& Nhit, glm::vec2& tex) const {
        Nhit = glm::normalize(normal);

        tex.x = 0;
        tex.y = 1.9;
    }


    Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, glm::vec3 albedo, int surfaceType) {
        this->v0 = v0;
        this->v1 = v1;
        this->v2 = v2;
        this->color = color;
        this->albedo = albedo;
        this->surfaceType = surfaceType;
    }

    bool Triangle::intersect(const glm::vec3& orig, const glm::vec3& dir, float& dist) {
        glm::vec3 v0v1 = (v1 - v0);
        glm::vec3 v0v2 = (v2 - v0);
        glm::vec3 pvec = glm::cross(dir, v0v2);
        float det = glm::dot(pvec, v0v1);
        float invDet = 1 / det;

        glm::vec3 tvec = orig - v0;
        float u = glm::dot(tvec, pvec) * invDet;
        if (u < 0 || u > 1) {
            return false;
        }

        glm::vec3 qvec = glm::cross(tvec, v0v1);
        float v = glm::dot(qvec, dir) * invDet;
        if (v < 0 || u + v > 1) {
            return false;
        }
        dist = glm::dot(qvec, v0v2) * invDet;
        return true;
    }
    void Triangle::getSurfaceData(const glm::vec3& Phit, glm::vec3& Nhit, glm::vec2& tex) const {
        Nhit = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        tex.x = 0;
        tex.y = 1.9;
    }

    BoundingBox::BoundingBox() {}

    BoundingBox::BoundingBox(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax, glm::vec3 albedo, int surfaceType) {
        minBound = glm::vec3(xmin, ymin, zmin);
        maxBound = glm::vec3(xmax, ymax, zmax);
        this->albedo = albedo;
        this->surfaceType = surfaceType;
    }

    bool BoundingBox::intersect(const glm::vec3& orig, const glm::vec3& dir, float& dist) {
        glm::vec3 invDir(1 / dir.x, 1 / dir.y, 1 / dir.z);
        float tmin, tmax, tymin, tymax, tzmin, tzmax;

        if (invDir.x >= 0) {
            tmin = (minBound.x - orig.x) * invDir.x;
            tmax = (maxBound.x - orig.x) * invDir.x;
        }
        else {
            tmin = (maxBound.x - orig.x) * invDir.x;
            tmax = (minBound.x - orig.x) * invDir.x;
        }

        if (invDir.y >= 0) {
            tymin = (minBound.y - orig.y) * invDir.y;
            tymax = (maxBound.y - orig.y) * invDir.y;
        }
        else {
            tymin = (maxBound.y - orig.y) * invDir.y;
            tymax = (minBound.y - orig.y) * invDir.y;
        }

        if ((tmin > tymax) || (tymin > tmax))
            return false;
        if (tymin > tmin)
            tmin = tymin;
        if (tymax < tmax)
            tmax = tymax;

        if (invDir.z >= 0) {
            tzmin = (minBound.z - orig.z) * invDir.z;
            tzmax = (maxBound.z - orig.z) * invDir.z;
        }
        else {
            tzmin = (maxBound.z - orig.z) * invDir.z;
            tzmax = (minBound.z - orig.z) * invDir.z;
        }
        if ((tmin > tzmax) || (tzmin > tmax))
            return false;
        if (tzmin > tmin)
            tmin = tzmin;
        if (tzmax < tmax)
            tmax = tzmax;

        if (tmin < 0) {
            dist = tmax;
        }
        else {
            dist = tmin;
        }
        return true;
    }
    void BoundingBox::getSurfaceData(const glm::vec3& Phit, glm::vec3& Nhit, glm::vec2& tex) const {
        Nhit = glm::vec3(0, 1, 0);
    }


    void TriangleMesh::triangulateMesh (
        const int numFaces,
        std::vector<int> &faceIndex,
        std::vector<int> &vertsIndex,
        std::vector<int> &polyNormalIndex,
        std::vector<glm::vec3> &verts,
        std::vector<glm::vec3> &norms,
        int numVerts,
        int numNorms)
    {
        int numTris = 0;

        for (int i = 0; i < numFaces; i++) {
            numTris += faceIndex[i] - 2;
        }

        glm::vec3 minBounds(INFINITY, INFINITY, INFINITY);
        glm::vec3 maxBounds(-INFINITY, -INFINITY, -INFINITY);
        
        for (int i = 0; i < numVerts; i++) {      
            minBounds.x = std::min(minBounds.x, verts[i].x);
            maxBounds.x = std::max(maxBounds.x, verts[i].x);
            minBounds.y = std::min(minBounds.y, verts[i].y);
            maxBounds.y = std::max(maxBounds.y, verts[i].y);
            minBounds.z = std::min(minBounds.z, verts[i].z);
            maxBounds.z = std::max(maxBounds.z, verts[i].z);
        }

        bounding = BoundingBox(minBounds.x, maxBounds.x, minBounds.y, maxBounds.y, minBounds.z, maxBounds.z, glm::vec3(.8,.8,.8), 0);
        std::vector<int> triNormsIndex;

        int l = 0;
        for (int i = 0, k = 0; i < numFaces; i++) {
            for (int j = 0; j < faceIndex[i] - 2; j++) {
                trisIndex.push_back(vertsIndex[k]);
                trisIndex.push_back(vertsIndex[k + j + 1]);
                trisIndex.push_back(vertsIndex[k + j + 2]);

                normIndex.push_back(polyNormalIndex[k]);
                normIndex.push_back(polyNormalIndex[k + j + 1]);
                normIndex.push_back(polyNormalIndex[k + j + 2]);
                l += 3;
            }
            k += faceIndex[i];
        }

        this->numTris = numTris;
        this->color = glm::vec3(255, 0, 255);
        this->verticies = std::vector<glm::vec3>(verts);
        this->normals = std::vector<glm::vec3>(norms);
    }

    TriangleMesh::TriangleMesh(std::string file, glm::vec3 albedo, int surfaceType) {
        this->albedo = albedo;
        this->surfaceType = surfaceType;

        std::ifstream ifs;
        ifs.open(file);
        std::string line;

        std::vector<int> numOfVerts;
        std::vector<int> vertexIndex;
        std::vector<int> polyNormalIndex;
        std::vector<glm::vec3> verticies;
        std::vector<glm::vec3> vertexNorm;
        std::vector<glm::vec3> vertexTex;

        try {
            while (std::getline(ifs, line)) {
                if (line.length() == 0) {
                    continue;
                }
                if (line.substr(0, 2).compare("vn") == 0) {
                    std::stringstream ss(line.substr(3));
                    float x, y, z;
                    ss >> x; ss >> y; ss >> z;
                    glm::vec3 norm(x, y, z);
                    vertexNorm.push_back(norm);
                }
                else if (line.at(0) == 'v') {
                    std::stringstream ss(line.substr(2));
                    float x, y, z;
                    ss >> x; ss >> y; ss >> z;
                    glm::vec3 vert(x, y, z);
                    verticies.push_back(vert);
                }
                else if (line.at(0) == 'f') {
                    std::stringstream ss(line.substr(2));
                    std::string values;
                    int vertsNum = 0;
                    while (std::getline(ss, values, ' ')) {
                        if (values.length() == 0) {
                            continue;
                        }
                        int a, b, c;
                        const char* chh = values.c_str();
                        sscanf_s(chh, "%i//%i", &a, &b, &c);
                        a--; b--; c--;
                        vertexIndex.push_back(a);
                        polyNormalIndex.push_back(b);
                        vertsNum++;
                    }
                    numOfVerts.push_back(vertsNum);
                }
            }
        }
        catch (int e) {
            std::cout << "It Failed" << std::endl;
        }

        triangulateMesh(numOfVerts.size(), numOfVerts, vertexIndex, polyNormalIndex, verticies, vertexNorm, verticies.size(), vertexNorm.size());
    }

    bool TriangleMesh::intersectTriangle(const glm::vec3& orig, const glm::vec3& dir, float& dist, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, bool& edge, float &u, float &v) const {
        glm::vec3 v0v1 = (v1 - v0);
        glm::vec3 v0v2 = (v2 - v0);
        glm::vec3 pvec = glm::cross(dir, v0v2);
        float det = glm::dot(pvec, v0v1);
        float invDet = 1 / det;

        glm::vec3 tvec = orig - v0;
        u = glm::dot(tvec, pvec) * invDet;
        if (u < 0 || u > 1) {
            return false;
        }

        glm::vec3 qvec = glm::cross(tvec, v0v1);
        v = glm::dot(qvec, dir) * invDet;
        if (v < 0 || u + v > 1) {
            return false;
        }

        float min = .02;
        if (u < min || v < min || 1 - u - v < min) {
            edge = true;
        }
        else {
            edge = false;
        }

        dist = glm::dot(qvec, v0v2) * invDet;
        return true;
    }

    bool TriangleMesh::intersect(const glm::vec3& orig, const glm::vec3& dir, float& dist) {
        bool edge = false;
        float nDist = INFINITY;
        bool hit = false;
        float test;
        int hitTriangle = -1;
        if (!bounding.intersect(orig, dir, test)) {
            return false;
        }
        for (int i = 0; i < numTris; i++) {
            float currDist = INFINITY;
            bool currentEdge = false;
            float u, v;
            if (intersectTriangle(orig, dir, currDist, verticies[trisIndex[3 * i]], verticies[trisIndex[3 * i + 1]], verticies[trisIndex[3 * i + 2]], currentEdge, u, v)) {
                if (currDist > 0 && currDist < nDist) {
                    nDist = currDist;
                    hit = true;
                    hitTriangle = i;
                    lastHitU = u;
                    lastHitV = v;
                    edge = currentEdge;
                }
            }
        }
        lastHitTri = hitTriangle;
        dist = nDist;
        //return !edge && hit;
        return hit;
    }
    void TriangleMesh::getSurfaceData(const glm::vec3& Phit, glm::vec3& Nhit, glm::vec2& tex) const {
        glm::vec3 n0 = normals[normIndex[lastHitTri * 3]];
        glm::vec3 n1 = normals[normIndex[lastHitTri * 3 + 1]];
        glm::vec3 n2 = normals[normIndex[lastHitTri * 3 + 2]];
        Nhit = lastHitU * n0 + lastHitV * n1 + (1 - lastHitU - lastHitV) * n2;

        //Nhit = normals[normIndex[lastHitTri * 3]];
        tex.x = 0;
        tex.y = 1.9;
    }
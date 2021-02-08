#define _USE_MATH_DEFINES


#include <iostream>
#include <sstream>

#include <GL/glm/glm.hpp>
#include <GL/glm/vec3.hpp>;
#include <GL/glm/vec3.hpp> // glm::vec3
#include <GL/glm/vec4.hpp> // glm::vec4
#include <GL/glm/mat4x4.hpp> // glm::mat4
#include <GL/glm/gtc/matrix_transform.hpp>
#include "GL/glm/ext.hpp"

class Object {
public:
    glm::vec3 color;
    Object() {}

    virtual bool intersect(const glm::vec3& orgin, const glm::vec3& dir, float& dist) const = 0;
    virtual void getSurfaceData(const glm::vec3& Phit, glm::vec3& Nhit, glm::vec2& tex) {
        tex.x = 1;
        tex.y = 1;
        Nhit = glm::vec3(0, 1, 0);
    };
};

class Sphere : public Object {
public:
    float radius;
    float radius2;
    glm::vec3 center;
    Sphere(glm::vec3 center, glm::vec3 color, float radius) {
        this->radius = radius;
        this->radius2 = radius * radius;
        this->center = center;
        this->color = color;
    }

    bool intersect(const glm::vec3& orig, const glm::vec3& dir, float& dist) const {
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

    void getSurfaceData(const glm::vec3& Phit, glm::vec3& Nhit, glm::vec2& tex) {
        Nhit = Phit - center;
        Nhit = glm::normalize(Nhit);

        tex.x = (1 + atan2(Nhit.z, Nhit.x) / 3.15159) * 0.5;
        tex.y = acosf(Nhit.y) / 3.14159;
    }
};

class Plane : public Object {
public:
    glm::vec3 normal;
    glm::vec3 center;
    Plane(glm::vec3 normal, glm::vec3 center, glm::vec3 color) {
        this->normal = normal;
        this->center = center;
        this->color = color;
    }
    bool intersect(const glm::vec3& orig, const glm::vec3& dir, float& dist) const {
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
    void getSurfaceData(const glm::vec3& Phit, glm::vec3& Nhit, glm::vec2& tex) {
        Nhit = glm::normalize(normal);

        tex.x = 0;
        tex.y = 1.9;
    }
};

class Triangle : public Object {
public:
    glm::vec3 v0;
    glm::vec3 v1;
    glm::vec3 v2;
    Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color) {
        this->v0 = v0;
        this->v1 = v1;
        this->v2 = v2;
        this->color = color;
    }
    bool intersect(const glm::vec3& orig, const glm::vec3& dir, float& dist) const {
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
    void getSurfaceData(const glm::vec3& Phit, glm::vec3& Nhit, glm::vec2& tex) {
        Nhit = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        tex.x = 0;
        tex.y = 1.9;
    }
};

class TriangleMesh : public Object {
public:
    int numTris;
    std::unique_ptr<int []> trisIndex;
    std::unique_ptr<glm::vec3[]> verticies;
    //int* vertexIndex;
    //glm::vec3* verticies;

    void triangulateMesh(
        const int numFaces,
        int* faceIndex,
        int* vertsIndex,
        glm::vec3* verts,
        int numVerts)
    {
        int numTris = 0;
        for (int i = 0; i < numFaces; i++) {
            numTris += faceIndex[i] - 2;
        }

        verticies = std::unique_ptr<glm::vec3[]>(new glm::vec3[numVerts]);
        for (int i = 0; i < numVerts; i++) {
            verticies[i] = verts[i];
        }
        
        trisIndex = std::unique_ptr<int[]>(new int[numTris * 3]);
        int l = 0;
        for (int i = 0, k = 0; i < numFaces; i++) {
            for (int j = 0; j < faceIndex[i] - 2; j++) {
                trisIndex[l] = vertsIndex[k];
                trisIndex[l + 1] = vertsIndex[k + j + 1];
                trisIndex[l + 2] = vertsIndex[k + j + 2];
                l += 3;
            }
            k += faceIndex[i];
        }

        this->numTris = numTris;
        this->color = glm::vec3(255, 0, 255);
    }

    TriangleMesh(std::string file) {
        std::ifstream ifs;
        ifs.open(file);
        std::string line;

        std::vector<int> numOfVerts;
        std::vector<int> vertexIndex;
        std::vector<glm::vec3> verticies;
        std::vector<glm::vec3> vertexNorm;
        std::vector<glm::vec3> vertexTex;

        try {
            while (std::getline(ifs, line)) {
                if (line.length() == 0) {
                    continue;
                }
                if (line.at(0) == 'v') {
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
                        sscanf_s(chh, "%i/%i/%i", &a, &b, &c);
                        a--; b--; c--;
                        vertexIndex.push_back(a);
                        vertsNum++;
                    }
                    numOfVerts.push_back(vertsNum);
                }
            }
        }
        catch (int e) {
            std::cout << "It Failed" << std::endl;
        }

        triangulateMesh(numOfVerts.size(), &numOfVerts[0], &vertexIndex[0], &verticies[0], verticies.size());
    }

    bool intersectTriangle(const glm::vec3& orig, const glm::vec3& dir, float& dist, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) const {
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

    bool intersect(const glm::vec3& orig, const glm::vec3& dir, float& dist) const {
        /*for (int i = 0; i < numTris * 3; i+= 3) {
            std::cout << trisIndex[i] << " " << trisIndex[i + 1] << " " << trisIndex[i + 2] << std::endl;
        }
        std::cout << std::endl;
        for (int i = 0; i < 8; i++) {
            std::cout << verticies[i].x << " " << verticies[i].y << " " << verticies[i].z << std::endl;
        } */
        
        float nDist = INFINITY;
        bool hit = false;
        for (int i = 0; i < numTris; i++) {
            float currDist = INFINITY;
            if (intersectTriangle(orig, dir, currDist, verticies[trisIndex[3 * i]], verticies[trisIndex[3 * i + 1]], verticies[trisIndex[3 * i + 2]])) {
                if (currDist > 0 && currDist < nDist) {
                    nDist = currDist;
                    hit = true;
                }
            }
        }
        dist = nDist;
        return hit;
    }
    void getSurfaceData(const glm::vec3& Phit, glm::vec3& Nhit, glm::vec2& tex) {
        Nhit = glm::vec3(0, 1, 0);
        tex.x = 0;
        tex.y = 1.9;
    }
};
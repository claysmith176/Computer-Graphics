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
    Object();

    virtual bool intersect(const glm::vec3& orgin, const glm::vec3& dir, float& dist) const = 0;
    virtual void getSurfaceData(const glm::vec3& Phit, glm::vec3& Nhit, glm::vec2& tex) const = 0;
};

class Sphere : public Object {
public:
    float radius;
    float radius2;
    glm::vec3 center;
    Sphere(glm::vec3 center, glm::vec3 color, float radius);

    bool intersect(const glm::vec3& orig, const glm::vec3& dir, float& dist) const;
    void getSurfaceData(const glm::vec3& Phit, glm::vec3& Nhit, glm::vec2& tex) const;
};

class Plane : public Object {
public:
    glm::vec3 normal;
    glm::vec3 center;
    Plane(glm::vec3 normal, glm::vec3 center, glm::vec3 color);
    bool intersect(const glm::vec3& orig, const glm::vec3& dir, float& dist) const;
    void getSurfaceData(const glm::vec3& Phit, glm::vec3& Nhit, glm::vec2& tex) const;
};

class Triangle : public Object {
public:
    glm::vec3 v0;
    glm::vec3 v1;
    glm::vec3 v2;
    Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color);
    bool intersect(const glm::vec3& orig, const glm::vec3& dir, float& dist) const;
    void getSurfaceData(const glm::vec3& Phit, glm::vec3& Nhit, glm::vec2& tex) const;
};

class BoundingBox : public Object {
public:
    glm::vec3 minBound;
    glm::vec3 maxBound;

    BoundingBox();
    BoundingBox(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax);

    bool intersect(const glm::vec3& orig, const glm::vec3& dir, float& dist) const;
    void getSurfaceData(const glm::vec3& Phit, glm::vec3& Nhit, glm::vec2& tex) const;
};

class TriangleMesh : public Object {
public:
    int numTris;
    std::unique_ptr<int[]> trisIndex;
    std::unique_ptr<glm::vec3[]> verticies;
    //int* vertexIndex;
    //glm::vec3* verticies;
    BoundingBox bounding;

    void triangulateMesh(
        const int numFaces,
        int* faceIndex,
        int* vertsIndex,
        glm::vec3* verts,
        int numVerts);

    TriangleMesh(std::string file);
    bool intersectTriangle(const glm::vec3& orig, const glm::vec3& dir, float& dist, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, bool& edge) const;
    bool intersect(const glm::vec3& orig, const glm::vec3& dir, float& dist) const;
    void getSurfaceData(const glm::vec3& Phit, glm::vec3& Nhit, glm::vec2& tex) const;
};
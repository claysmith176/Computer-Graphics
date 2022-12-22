#pragma once

#include <cmath>
#include <iostream>
#include <assert.h>
#include "pbrt.h"

using std::sqrt;
using std::fabs;

template <typename T> class Vector2 {
public:
    Vector2() { x = y = 0; }
    Vector2(T xx, T yy)
        : x(xx), y(yy) {
        assert(!HasNaNs());
    }
    bool HasNaNs() const {
        return std::isnan(x) || std::isnan(y);
    }

    // The default versions of these are fine for release builds; for debug
    // we define them so that we can add the assert checks.
    Vector2(const Vector2<T>& v) {
        assert(!v.HasNaNs());
        x = v.x; y = v.y;
    }
    Vector2<T>& operator=(const Vector2<T>& v) {
        assert(!v.HasNaNs());
        x = v.x; y = v.y;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector2<T>& v) {
        os << "[" << v.x << ", " << v.y << "]";
        return os;
    }

    Vector2<T> operator+(const Vector2<T>& v) const {
        assert(!v.HasNaNs());
        return Vector2(x + v.x, y + v.y);
    }

    Vector2<T>& operator+=(const Vector2<T>& v) {
        assert(!v.HasNaNs());
        x += v.x; y += v.y;
        return *this;
    }
    Vector2<T> operator-(const Vector2<T>& v) const {
        assert(!v.HasNaNs());
        return Vector2(x - v.x, y - v.y);
    }

    Vector2<T>& operator-=(const Vector2<T>& v) {
        assert(!v.HasNaNs());
        x -= v.x; y -= v.y;
        return *this;
    }
    bool operator==(const Vector2<T>& v) const {
        return x == v.x && y == v.y;
    }
    bool operator!=(const Vector2<T>& v) const {
        return x != v.x || y != v.y;
    }
    Vector2<T> operator*(T f) const { return Vector2<T>(f * x, f * y); }

    Vector2<T>& operator*=(T f) {
        assert(!std::isnan(f));
        x *= f; y *= f;
        return *this;
    }
    Vector2<T> operator/(T f) const {
        assert(f != 0);
        float inv = (float)1 / f;
        return Vector2<T>(x * inv, y * inv);
    }

    Vector2<T>& operator/=(T f) {
        assert(f != 0);
        float inv = (float)1 / f;
        x *= inv; y *= inv;
        return *this;
    }
    Vector2<T> operator-() const { return Vector2<T>(-x, -y); }
    T operator[](int i) const {
        assert(i >= 0 && i <= 1);
        if (i == 0) return x;
        return y;
    }

    T& operator[](int i) {
        assert(i >= 0 && i <= 1);
        if (i == 0) return x;
        return y;
    }
    float LengthSquared() const { return x * x + y * y; }
    float Length() const { return std::sqrt(LengthSquared()); }
public:
    T x, y;
};

template <typename T> class Vector3 {
public:
    T operator[](int i) const {
        assert(i >= 0 && i <= 2);
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }
    T& operator[](int i) {
        assert(i >= 0 && i <= 2);
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }
    Vector3() { x = y = z = 0; }
    Vector3(T x, T y, T z)
        : x(x), y(y), z(z) {
        assert(!HasNaNs());
    }
    bool HasNaNs() const {
        return std::isnan(x) || std::isnan(y) || std::isnan(z);
    }

    // The default versions of these are fine for release builds; for debug
    // we define them so that we can add the assert checks.
    Vector3(const Vector3<T>& v) {
        assert(!v.HasNaNs());
        x = v.x; y = v.y; z = v.z;
    }

    Vector3<T>& operator=(const Vector3<T>& v) {
        assert(!v.HasNaNs());
        x = v.x; y = v.y; z = v.z;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector3<T>& v) {
        os << "[" << v.x << ", " << v.y << ", " << v.z << "]";
        return os;
    }
    Vector3<T> operator+(const Vector3<T>& v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }
    Vector3<T>& operator+=(const Vector3<T>& v) {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }
    Vector3<T> operator-(const Vector3<T>& v) const {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }
    Vector3<T>& operator-=(const Vector3<T>& v) {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }
    bool operator==(const Vector3<T>& v) const {
        return x == v.x && y == v.y && z == v.z;
    }
    bool operator!=(const Vector3<T>& v) const {
        return x != v.x || y != v.y || z != v.z;
    }
    Vector3<T> operator*(T s) const { return Vector3<T>(s * x, s * y, s * z); }
    Vector3<T>& operator*=(T s) {
        x *= s; y *= s; z *= s;
        return *this;
    }
    Vector3<T> operator/(T f) const {
        assert(f != 0);
        float inv = (float)1 / f;
        return Vector3<T>(x * inv, y * inv, z * inv);
    }

    Vector3<T>& operator/=(T f) {
        assert(f != 0);
        float inv = (float)1 / f;
        x *= inv; y *= inv; z *= inv;
        return *this;
    }
    Vector3<T> operator-() const { return Vector3<T>(-x, -y, -z); }
    float LengthSquared() const { return x * x + y * y + z * z; }
    float Length() const { return std::sqrt(LengthSquared()); }

public:
    T x, y, z;
};

typedef Vector2<float> Vector2f;
typedef Vector2<int>   Vector2i;
typedef Vector3<float> Vector3f;
typedef Vector3<int>   Vector3i;

// Vector Inline Functions
template <typename T> inline T Dot(const Vector2<T> &v1, const Vector2<T> &v2) {
    return (v1.x * v2.x) + (v1.y * v2.y);
}
template <typename T> inline T Dot(const Vector3<T> &v1, const Vector3<T> &v2) {
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}
template <typename T> inline T AbsDot(const Vector2<T>& v1, const Vector2<T>& v2) {
    return std::abs(Dot(v1, v2));
}
template <typename T> inline T AbsDot(const Vector3<T>& v1, const Vector3<T>& v2) {
    return std::abs(Dot(v1, v2));
}
template <typename T> inline Vector3<T> Cross(const Vector3<T>& v1, const Vector3<T>& v2) {
        double v1x = v1.x, v1y = v1.y, v1z = v1.z;
        double v2x = v2.x, v2y = v2.y, v2z = v2.z;
        return Vector3<T>((v1y * v2z) - (v1z * v2y),
            (v1z * v2x) - (v1x * v2z),
            (v1x * v2y) - (v1y * v2x));
}
template <typename T> Vector2<T> Normalize(const Vector2<T>& v) {
    return v / v.Length();
}
template <typename T> Vector3<T> Normalize(const Vector3<T>& v) {
    return v / v.Length();
}
template <typename T> T MinComponent(const Vector2<T>& v) {
    return std::min(v.x, v.y);
}
template <typename T> T MaxComponent(const Vector2<T>& v) {
    return std::max(v.x, v.y);
}
template <typename T> T MinComponent(const Vector3<T>& v) {
    return std::min(v.x, std::min(v.y, v.z));
}
template <typename T> T MaxComponent(const Vector3<T>& v) {
    return std::max(v.x, std::max(v.y, v.z));
}
template <typename T> int MaxDimension(const Vector2<T>& v) {
    return (v.x > v.y) ? 0 : 1;
}
template <typename T> int MaxDimension(const Vector3<T>& v) {
    return (v.x > v.y) ? ((v.x > v.z) ? 0 : 2) :
        ((v.y > v.z) ? 1 : 2);
}
template <typename T> Vector2<T> Min(const Vector2<T>& p1, const Vector2<T>& p2) {
    return Vector2<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
}
template <typename T> Vector2<T> Max(const Vector2<T>& p1, const Vector2<T>& p2) {
    return Vector2<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y));
}
template <typename T> Vector3<T> Min(const Vector3<T>& p1, const Vector3<T>& p2) {
        return Vector3<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y),
            std::min(p1.z, p2.z));
}
template <typename T> Vector3<T> Max(const Vector3<T>& p1, const Vector3<T>& p2) {
            return Vector3<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y),
                std::max(p1.z, p2.z));
}
template <typename T> Vector2<T> Permute(const Vector2<T>& v, int x, int y) {
    return Vector2<T>(v[x], v[y]);
}
template <typename T> Vector3<T> Permute(const Vector3<T>& v, int x, int y, int z) {
    return Vector3<T>(v[x], v[y], v[z]);
}
template <typename T> inline void CoordinateSystem(const Vector3<T>& v1, Vector3<T>* v2, Vector3<T>* v3) {
    if (std::abs(v1.x) > std::abs(v1.y))
        *v2 = Vector3<T>(-v1.z, 0, v1.x) /
        std::sqrt(v1.x * v1.x + v1.z * v1.z);
    else
        *v2 = Vector3<T>(0, v1.z, -v1.y) /
        std::sqrt(v1.y * v1.y + v1.z * v1.z);
    *v3 = Cross(v1, *v2);
}

// Points
template <typename T> class Point3 {
public:
    Point3() { x = y = z = 0; }
    Point3(T x, T y, T z) : x(x), y(y), z(z) {
        assert(!HasNaNs());
    }
    template <typename U> explicit Point3(const Point3<U>& p)
        : x((T)p.x), y((T)p.y), z((T)p.z) {
        assert(!HasNaNs());
    }
    template <typename U> explicit operator Vector3<U>() const {
        return Vector3<U>(x, y, z);
    }
    Point3(const Point3<T>& p) {
        assert(!p.HasNaNs());
        x = p.x; y = p.y; z = p.z;
    }
    Point3<T>& operator=(const Point3<T>& p) {
        assert(!p.HasNaNs());
        x = p.x; y = p.y; z = p.z;
        return *this;
    }
    friend std::ostream& operator<<(std::ostream& os, const Point3<T>& p) {
        os << "[" << p.x << ", " << p.y << ", " << p.z << "]";
        return os;
    }
    Point3<T> operator+(const Vector3<T>& v) const {
        return Point3<T>(x + v.x, y + v.y, z + v.z);
    }
    Point3<T>& operator+=(const Vector3<T>& v) {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }
    Vector3<T> operator-(const Point3<T>& p) const {
        return Vector3<T>(x - p.x, y - p.y, z - p.z);
    }
    Point3<T> operator-(const Vector3<T>& v) const {
        return Point3<T>(x - v.x, y - v.y, z - v.z);
    }
    Point3<T>& operator-=(const Vector3<T>& v) {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }
    Point3<T>& operator+=(const Point3<T>& p) {
        x += p.x; y += p.y; z += p.z;
        return *this;
    }
    Point3<T> operator+(const Point3<T>& p) const {
        return Point3<T>(x + p.x, y + p.y, z + p.z);
    }
    Point3<T> operator*(T f) const {
        return Point3<T>(f * x, f * y, f * z);
    }
    Point3<T>& operator*=(T f) {
        x *= f; y *= f; z *= f;
        return *this;
    }
    Point3<T> operator/(T f) const {
        float inv = (float)1 / f;
        return Point3<T>(inv * x, inv * y, inv * z);
    }
    Point3<T>& operator/=(T f) {
        float inv = (float)1 / f;
        x *= inv; y *= inv; z *= inv;
        return *this;
    }
    T operator[](int i) const {
        assert(i >= 0 && i <= 2);
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    T& operator[](int i) {
        assert(i >= 0 && i <= 2);
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }
    bool operator==(const Point3<T>& p) const {
        return x == p.x && y == p.y && z == p.z;
    }
    bool operator!=(const Point3<T>& p) const {
        return x != p.x || y != p.y || z != p.z;
    }
    bool HasNaNs() const {
        return std::isnan(x) || std::isnan(y) || std::isnan(z);
    }
    Point3<T> operator-() const { return Point3<T>(-x, -y, -z); }
public:
    T x, y, z;
};

template <typename T> class Point2 {
public:
    explicit Point2(const Point3<T>& p) : x(p.x), y(p.y) {
        assert(!HasNaNs());
    }
    Point2() { x = y = 0; }
    Point2(T xx, T yy)
        : x(xx), y(yy) {
        assert(!HasNaNs());
    }
    template <typename U> explicit Point2(const Point2<U>& p) {
        x = (T)p.x;
        y = (T)p.y;
        assert(!HasNaNs());
    }
    template <typename U> explicit Point2(const Vector2<U>& p) {
        x = (T)p.x;
        y = (T)p.y;
        assert(!HasNaNs());
    }
    template <typename U> explicit operator Vector2<U>() const {
        return Vector2<U>(x, y);
    }
    Point2(const Point2<T>& p) {
        assert(!p.HasNaNs());
        x = p.x; y = p.y;
    }
    Point2<T>& operator=(const Point2<T>& p) {
        assert(!p.HasNaNs());
        x = p.x; y = p.y;
        return *this;
    }
    friend std::ostream& operator<<(std::ostream& os, const Point2<T>& p) {
        os << "[" << p.x << ", " << p.y << "]";
        return os;
    }
    Point2<T> operator+(const Vector2<T>& v) const {
        assert(!v.HasNaNs());
        return Point2<T>(x + v.x, y + v.y);
    }
    Point2<T>& operator+=(const Vector2<T>& v) {
        assert(!v.HasNaNs());
        x += v.x; y += v.y;
        return *this;
    }
    Vector2<T> operator-(const Point2<T>& p) const {
        assert(!p.HasNaNs());
        return Vector2<T>(x - p.x, y - p.y);
    }
    Point2<T> operator-(const Vector2<T>& v) const {
        assert(!v.HasNaNs());
        return Point2<T>(x - v.x, y - v.y);
    }
    Point2<T> operator-() const { return Point2<T>(-x, -y); }
    Point2<T>& operator-=(const Vector2<T>& v) {
        assert(!v.HasNaNs());
        x -= v.x; y -= v.y;
        return *this;
    }
    Point2<T>& operator+=(const Point2<T>& p) {
        assert(!p.HasNaNs());
        x += p.x; y += p.y;
        return *this;
    }
    Point2<T> operator+(const Point2<T>& p) const {
        assert(!p.HasNaNs());
        return Point2<T>(x + p.x, y + p.y);
    }
    Point2<T> operator* (T f) const {
        return Point2<T>(f * x, f * y);
    }
    Point2<T>& operator*=(T f) {
        x *= f; y *= f;
        return *this;
    }
    Point2<T> operator/ (T f) const {
        float inv = (float)1 / f;
        return Point2<T>(inv * x, inv * y);
    }
    Point2<T>& operator/=(T f) {
        float inv = (float)1 / f;
        x *= inv; y *= inv;
        return *this;
    }
    T operator[](int i) const {
        assert(i >= 0 && i <= 1);
        if (i == 0) return x;
        return y;
    }
    T& operator[](int i) {
        assert(i >= 0 && i <= 1);
        if (i == 0) return x;
        return y;
    }
    bool operator==(const Point2<T>& p) const {
        return x == p.x && y == p.y;
    }
    bool operator!=(const Point2<T>& p) const {
        return x != p.x || y != p.y;
    }
    bool HasNaNs() const {
        return std::isnan(x) || std::isnan(y);
    }
public:
    T x, y;
};


// Point Declarations
typedef Point2<float> Point2f;
typedef Point2<int>   Point2i;
typedef Point3<float> Point3f;
typedef Point3<int>   Point3i;

// Point Inline Functions
template <typename T> inline float Distance(const Point2<T>& p1, const Point2<T>& p2) {
    return (p1 - p2).Length();
}
template <typename T> inline float DistanceSquared(const Point2<T>& p1, const Point2<T>& p2) {
    return (p1 - p2).LengthSquared();
}
template <typename T> inline float Distance(const Point3<T>& p1, const Point3<T>& p2) {
    return (p1 - p2).Length();
}
template <typename T> inline float DistanceSquared(const Point3<T>& p1, const Point3<T>& p2) {
    return (p1 - p2).LengthSquared();
}
template <typename T> Point2<T> Lerp(float t, const Point2<T>& p0, const Point2<T>& p1) {
    return (1 - t) * p0 + t * p1;
}
template <typename T> Point3<T> Lerp(float t, const Point3<T>& p0, const Point3<T>& p1) {
        return (1 - t) * p0 + t * p1;
}
template <typename T> Point2<T> Min(const Point2<T>& p1, const Point2<T>& p2) {
    return Point2<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
}
template <typename T> Point2<T> Max(const Point2<T>& p1, const Point2<T>& p2) {
    return Point2<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y));
}
template <typename T> Point3<T> Min(const Point3<T>& p1, const Point3<T>& p2) {
    return Point3<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y),
    std::min(p1.z, p2.z));
}
template <typename T> Point3<T> Max(const Point3<T>& p1, const Point3<T>& p2) {
    return Point3<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y),
    std::max(p1.z, p2.z));
}
template <typename T> Point2<T> Floor(const Point2<T>& p) {
    return Point2<T>(std::floor(p.x), std::floor(p.y));
}
template <typename T> Point2<T> Ceil(const Point2<T>& p) {
    return Point2<T>(std::ceil(p.x), std::ceil(p.y));
}
template <typename T> Point2<T> Abs(const Point2<T>& p) {
    return Point2<T>(std::abs(p.x), std::abs(p.y));
}
template <typename T> Point3<T> Floor(const Point3<T>& p) {
    return Point3<T>(std::floor(p.x), std::floor(p.y), std::floor(p.z));
}
template <typename T> Point3<T> Ceil(const Point3<T>& p) {
    return Point3<T>(std::ceil(p.x), std::ceil(p.y), std::ceil(p.z));
}
template <typename T> Point3<T> Abs(const Point3<T>& p) {
    return Point3<T>(std::abs(p.x), std::abs(p.y), std::abs(p.z));
}
template <typename T> Point2<T> Permute(const Point2<T>& p, int x, int y) {
    return Point2<T>(p[x], p[y]);
}
template <typename T> Point3<T> Permute(const Point3<T>& p, int x, int y, int z) {
    return Point3<T>(p[x], p[y], p[z]);
}

// Normal Declaration
template <typename T>
class Normal3 {
public:
    // Normal3 Public Methods
    Normal3() { x = y = z = 0; }
    Normal3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) { DCHECK(!HasNaNs()); }
    Normal3<T> operator-() const { return Normal3(-x, -y, -z); }
    Normal3<T> operator+(const Normal3<T>& n) const {
        DCHECK(!n.HasNaNs());
        return Normal3<T>(x + n.x, y + n.y, z + n.z);
    }

    Normal3<T>& operator+=(const Normal3<T>& n) {
        DCHECK(!n.HasNaNs());
        x += n.x;
        y += n.y;
        z += n.z;
        return *this;
    }
    Normal3<T> operator-(const Normal3<T>& n) const {
        DCHECK(!n.HasNaNs());
        return Normal3<T>(x - n.x, y - n.y, z - n.z);
    }

    Normal3<T>& operator-=(const Normal3<T>& n) {
        DCHECK(!n.HasNaNs());
        x -= n.x;
        y -= n.y;
        z -= n.z;
        return *this;
    }
    bool HasNaNs() const { return isNaN(x) || isNaN(y) || isNaN(z); }
    template <typename U>
    Normal3<T> operator*(U f) const {
        return Normal3<T>(f * x, f * y, f * z);
    }

    template <typename U>
    Normal3<T>& operator*=(U f) {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }
    template <typename U>
    Normal3<T> operator/(U f) const {
        CHECK_NE(f, 0);
        float inv = (float)1 / f;
        return Normal3<T>(x * inv, y * inv, z * inv);
    }

    template <typename U>
    Normal3<T>& operator/=(U f) {
        CHECK_NE(f, 0);
        float inv = (float)1 / f;
        x *= inv;
        y *= inv;
        z *= inv;
        return *this;
    }
    float LengthSquared() const { return x * x + y * y + z * z; }
    float Length() const { return std::sqrt(LengthSquared()); }

    explicit Normal3<T>(const Vector3<T>& v) : x(v.x), y(v.y), z(v.z) {
        DCHECK(!v.HasNaNs());
    }
    bool operator==(const Normal3<T>& n) const {
        return x == n.x && y == n.y && z == n.z;
    }
    bool operator!=(const Normal3<T>& n) const {
        return x != n.x || y != n.y || z != n.z;
    }

    T operator[](int i) const {
        assert(i >= 0 && i <= 2);
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    T& operator[](int i) {
        assert(i >= 0 && i <= 2);
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    // Normal3 Public Data
    T x, y, z;
};


//vec3 rotate(const float degrees, const vec3& axis, const vec3& eye) {
//    float x = axis.x();
//    float y = axis.y();
//    float z = axis.z();
//    float radians = degrees * (M_PI / 180);
//
//    glm::vec3 eye_g(eye.x(), eye.y(), eye.z());
//    glm::vec3 axis_g(axis.x(), axis.y(), axis.z());
//
//    glm::mat3 xmat(1.0f);
//    glm::mat3 ymat = glm::outerProduct(axis_g, axis_g);
//    glm::mat3 zmat(0, z, -y, -z, 0, x, y, -x, 0);
//
//    xmat = xmat * cos(radians);
//    ymat = ymat * (1 - cos(radians));
//    zmat = zmat * sin(radians);
//
//    glm::mat3 test = xmat + ymat + zmat;
//    glm::vec3 res = test * eye_g;
//    return vec3(res.x, res.y, res.z);
//}

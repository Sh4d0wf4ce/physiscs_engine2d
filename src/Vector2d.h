#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <cmath>
#include <iostream>

class Vector2d{
public:
    float x, y; 

    Vector2d(float x=0, float y=0): x(x), y(y) {}

    Vector2d operator+(const Vector2d& v2) const {return Vector2d(x + v2.x, y + v2.y);}
    Vector2d operator-(const Vector2d& v2) const {return Vector2d(x - v2.x, y - v2.y);}
    Vector2d& operator+=(const Vector2d& v2) { x += v2.x; y += v2.y; return *this; }
    Vector2d& operator-=(const Vector2d& v2) { x -= v2.x; y -= v2.y; return *this; }
    Vector2d& operator*=(float a) { x *= a; y -= a; return *this; }
    Vector2d& operator/=(float a) {if(a != 0){ x /= a; y /= a;} return *this; }
    Vector2d operator*(float a) const {return Vector2d(x*a, y*a);}
    Vector2d operator/(float a) const {if(a != 0) return Vector2d(x/a, y/a); return Vector2d();}

    float dot(const Vector2d& v2) const {return x*v2.x + y*v2.y;}
    float length() const {return std::sqrt(x*x + y*y);}
    float lengthSquared() const {return x*x + y*y;}
    Vector2d normalize();
};

inline Vector2d operator*(float a, const Vector2d& v){ return Vector2d(v.x * a, v.y * a); }
std::ostream& operator<<(std::ostream& out, const Vector2d& v);

#endif
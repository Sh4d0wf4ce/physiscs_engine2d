#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <cmath>
#include <iostream>

/// @brief 2D vector class.
class Vector2d{
public:
    float x, y; 

    /// @brief Default constructor.
    /// @param x 
    /// @param y 
    Vector2d(float x=0, float y=0): x(x), y(y) {}

    /// @brief Addition operator.
    Vector2d operator+(const Vector2d& v2) const {return Vector2d(x + v2.x, y + v2.y);}
    /// @brief Subtraction operator.
    Vector2d operator-(const Vector2d& v2) const {return Vector2d(x - v2.x, y - v2.y);}
    /// @brief  Plus-equals operator.
    Vector2d& operator+=(const Vector2d& v2) {x += v2.x; y += v2.y; return *this;}
    /// @brief Minus-equals operator.
    Vector2d& operator-=(const Vector2d& v2) {x -= v2.x; y -= v2.y; return *this;}
    /// @brief Multiply-equals operator.
    Vector2d& operator*=(float a) {x *= a; y *= a; return *this;}
    /// @brief Divide-equals operator.
    Vector2d& operator/=(float a) {if(a != 0){ x /= a; y /= a;} return *this;}
    /// @brief Scalar multiplication operator.
    Vector2d operator*(float a) const {return Vector2d(x*a, y*a);}
    /// @brief Scalar division operator.
    Vector2d operator/(float a) const {if(a != 0) return Vector2d(x/a, y/a); return Vector2d();}
    /// @brief Equality operator.
    bool operator==(const Vector2d& v2) const {return (x==v2.x && y==v2.y);}

    /// @brief Returns the dot product.
    float dot(const Vector2d& v2) const {return x*v2.x + y*v2.y;}
    /// @brief Returns the length of the vector.
    float length() const {return std::sqrt(x*x + y*y);}
    /// @brief Returns the squared length of the vector.
    float lengthSquared() const {return x*x + y*y;}
    /// @brief Normalizes the vector.
    Vector2d normalize();
};

/// @brief Scalar multiplication operator.
inline Vector2d operator*(float a, const Vector2d& v) {return Vector2d(v.x * a, v.y * a);}
/// @brief Output stream operator.
std::ostream& operator<<(std::ostream& out, const Vector2d& v);

#endif
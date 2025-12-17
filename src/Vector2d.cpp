#include "Vector2d.h"

Vector2d Vector2d::normalize(){
    float len = length();
    if(len != 0){
        x /= len;
        y /= len;
    }

    return *this;
}

std::ostream& operator<<(std::ostream& out, const Vector2d& v){
    out<<"("<<v.x<<", "<<v.y<<")";
    return out;
}
#include "Vector2d.h"

void Vector2d::normalize(){
    float len = length();
    if(len != 0){
        x /= len;
        y /= len;
    }
}

std::ostream& operator<<(std::ostream& out, const Vector2d& v){
    out<<"("<<v.x<<", "<<v.y<<")";
    return out;
}
//
//  Framework for a raytracer
//  File: triple.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Author: Maarten Everts
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#include "triple.h"
#include <cmath>

Color::Color(Color::component red, Color::component green, Color::component blue)
    : r(red), g(green), b(blue)
{ }

void Color::set(Color::component red, Color::component green, Color::component blue) {
    r = red;
    g = green;
    b = blue;
}

Vector::Vector(double x, double y, double z)
    : x(x), y(y), z(z)
{ }

double Vector::dot(const Vector &other) const {
    double sum = 0;
    for (int i = 0; i < 3; ++i)
        sum += (*this)[i] * other[i];

    return sum;
}

void Vector::normalize() {
    double n = norm();
    for (double& direction : *this)
        direction /= n;
}

Vector Vector::normalized() const {
    Vector output(*this);
    output.normalize();
    return output;
}

Vector Vector::cross(const Vector &other) const {
    Vector output{};

    output.x = y*other.z - z*other.y;
    output.y = z*other.x - x*other.z;
    output.z = x*other.y - y*other.x;

    return output;
}

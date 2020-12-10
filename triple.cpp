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

Color::Color(Color::component red, Color::component green, Color::component blue)
    : r(red), g(green), b(blue)
{ }

template<typename Triple, typename>
Color::Color(const Triple &t)
    : Color(t[0], t[1], t[2])
{ }

void Color::set(Color::component red, Color::component green, Color::component blue) {
    r = red;
    g = green;
    b = blue;
}

Vector::Vector(double x, double y, double z)
    : x(x), y(y), z(z)
{ }

template<typename Triple, typename>
Vector::Vector(const Triple &t)
    : Vector(t[0], t[1], t[2])
{ }

double Vector::dot(const Vector &other) const {
    double sum = 0;
    for (int i = 0; i < 3; ++i)
        sum += (*this)[i] * other[i];

    return sum;
}

double Vector::norm() const {
    return std::sqrt(X() * X() + Y() * Y() + Z() * Z());
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



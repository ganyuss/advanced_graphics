//
//  Framework for a raytracer
//  File: sphere.h
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Maarten Everts
//    Jasper van de Gronde
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#ifndef SPHERE_H_115209AE
#define SPHERE_H_115209AE

#include "object.h"
#include <array>

class Sphere : public Object
{
public:
    Sphere(Point Position,double Radius, Quaternion Rotation = Quaternion(0,0,0,1)) : Position(Position), Radius(Radius), Rotation(Rotation.normalized()) { }

    [[nodiscard]] Hit intersect(const Ray &ray) const override;
    [[nodiscard]] std::array<double, 2> getTextureCoordinatesFor(const Point &p) const override;

    const Point Position;
    const double Radius;
    const Quaternion Rotation;
};

#endif /* end of include guard: SPHERE_H_115209AE */

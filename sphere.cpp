//
//  Framework for a raytracer
//  File: sphere.cpp
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

#include "sphere.h"
#include <iostream>
#include <math.h>

/************************** Sphere **********************************/

Hit Sphere::intersect(const Ray &ray)
{

    // Intersection point calculation
    // source: https://fiftylinesofcode.com/ray-sphere-intersection/
    Vector o_minus_c = ray.Origin - position;

    double p = ray.Direction.dot(o_minus_c);
    double q = o_minus_c.dot(o_minus_c) - (r * r);

    double discriminant = (p * p) - q;
    if (discriminant < 0.0f)
    {
        return Hit::NO_HIT();
    }

    double dRoot = std::sqrt(discriminant);
    double distanceToOrigin = -p - dRoot;

    // Normal calculation
    Point intersectionPoint = ray.Origin + ray.Direction * distanceToOrigin;
    Vector normal = (-position + intersectionPoint).normalized();

    return {distanceToOrigin, intersectionPoint, normal, ray};
}

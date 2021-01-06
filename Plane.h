//
// Created by cleme on 06/01/2021.
//

#ifndef RAYTRACER_PLANE_H
#define RAYTRACER_PLANE_H


#include "triple.h"
#include "light.h"

class Plane {
public:
    Plane(Point Origin, Vector Normal) :
            Origin(Origin), Normal(Normal)
    { }

    Point getIntersectionPoint(const Ray &) const;

private:

    Point Origin;
    Vector Normal;
};


#endif //RAYTRACER_PLANE_H

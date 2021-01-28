//
// Created by cleme on 06/01/2021.
//

#ifndef RAYTRACER_PLANE_H
#define RAYTRACER_PLANE_H


#include "triple.h"
#include "light.h"
#include "object.h"

class Plane : public Object {
public:
    Plane(Point Origin, Vector Normal) :
            Origin(Origin), Normal(Normal)
    { }

    Hit intersect(const Ray &ray) override;
    std::array<double, 2> getTextureCoordinatesFor(Point) override;

    Vector projectOn(const Vector &);

private:

    Point Origin;
    Vector Normal;
};


#endif //RAYTRACER_PLANE_H

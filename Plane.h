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

    [[nodiscard]] Hit intersect(const Ray &ray) const override;
    [[nodiscard]] std::array<double, 2> getTextureCoordinatesFor(const Point &) const override;

    [[nodiscard]] Vector projectOn(const Vector &) const;

private:

    Point Origin;
    Vector Normal;
};


#endif //RAYTRACER_PLANE_H

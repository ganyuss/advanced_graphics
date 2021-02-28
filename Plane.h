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
    Plane(Point Position, Vector Normal, double UVScale = 1) :
            Object(Position), Normal(Normal), UVScale(UVScale)
    { }

    [[nodiscard]] Hit intersect(const Ray &ray) const override;
    [[nodiscard]] std::array<double, 2> getTextureCoordinatesFor(const Point &) const override;

    [[nodiscard]] Vector projectOn(const Vector &) const;

    const Vector Normal;
private:

    // Used in UV calculations
    const Vector UVVector1 = getAnyOrthogonalVector(Normal).normalized();
    const Vector UVVector2 = getThirdOrthogonalVector(Normal, UVVector1).normalized();

    const double firstComponentFactor = 1 / Normal.dot(UVVector1.cross(UVVector2));
    const double secondComponentFactor = 1  / Normal.dot(UVVector2.cross(UVVector1));

    const double UVScale;
};


#endif //RAYTRACER_PLANE_H

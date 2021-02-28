//
// Created by cleme on 06/01/2021.
//

#ifndef RAYTRACER_CONE_H
#define RAYTRACER_CONE_H


#include <vector>
#include "light.h"
#include "object.h"
#include "Plane.h"

class Cone : public Object {
public:
    Cone(Point Position, Vector Side, Vector Up) :
        Object(Position), Up(Up), DiskPlan(Position, Up), Side(Plane{Position, Up}.projectOn(Side)), Radius(Side.norm()),
        theta(std::atan(Side.norm() / Up.norm())), cosSquaredTheta(std::pow(std::cos(theta), 2)),
        v(-Up.normalized())
    { }

    [[nodiscard]] Hit intersect(const Ray &ray) const override;
    [[nodiscard]] std::array<double, 2> getTextureCoordinatesFor(const Point &point) const override;

    const Vector Side;
    const double Radius;
    const Vector Up;

private:

    [[nodiscard]] Vector getNormalAt(const Point&) const;
    [[nodiscard]] bool isInShadowCone(const Point&) const;

    [[nodiscard]] Hit getHitOnSlope(const Ray&) const;

    [[nodiscard]] bool isOnDisk(const Point&) const;

    // Used in the intersect calculations
    const double theta;
    const double cosSquaredTheta;
    const Vector v;
    const Plane DiskPlan;
};


#endif //RAYTRACER_CONE_H

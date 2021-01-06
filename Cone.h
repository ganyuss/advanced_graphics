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
    Cone(Point Position, double Radius, Vector Up) :
        Position(Position), Radius(Radius), Up(Up),
        theta(std::atan(Radius / Up.norm())), cosSquaredTheta(std::pow(std::cos(theta), 2)),
        v(-Up.normalized()),
        DiskPlan(Position, Up)
    { }

    Hit intersect(const Ray &ray) override;

    const Point Position;
    const double Radius;
    const Vector Up;

private:

    Vector getNormalAt(const Point&);
    bool isInShadowCone(const Point&);

    Hit getHitOnSlope(const Ray&);

    // Used in the intersect calculations
    const double theta;
    const double cosSquaredTheta;
    const Vector v;
    const Plane DiskPlan;
};


#endif //RAYTRACER_CONE_H

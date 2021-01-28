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
        Position(Position), Side(Side), Up(Up), Radius(Side.norm()),
        theta(std::atan(Side.norm() / Up.norm())), cosSquaredTheta(std::pow(std::cos(theta), 2)),
        v(-Up.normalized()),
        DiskPlan(Position, Up)
    { }

    Hit intersect(const Ray &ray) override;
    std::array<double, 2> getTextureCoordinatesFor(const Point &point) override;

    const Point Position;
    const Vector Side;
    const double Radius;
    const Vector Up;

private:

    Vector getNormalAt(const Point&);
    bool isInShadowCone(const Point&);

    Hit getHitOnSlope(const Ray&);

    bool isOnDisk(const Point&);

    // Used in the intersect calculations
    const double theta;
    const double cosSquaredTheta;
    const Vector v;
    Plane DiskPlan; // Can't be const because Object::intersect isn't const
};


#endif //RAYTRACER_CONE_H

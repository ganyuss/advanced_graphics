//
// Created by cleme on 06/01/2021.
//

#include "Plane.h"
#include "commongeometry.h"

Hit Plane::intersect(const Ray &ray) const {
    Vector rayOrthogonal = project(ray.Direction, Normal);

    Vector shortestPath = project(Origin - ray.Origin, Normal);

    double t = shortestPath.norm() / rayOrthogonal.norm();

    if (shortestPath.dot(rayOrthogonal) < 0)
        return Hit::NO_HIT();

    return Hit(
            t,
            ray.at(t),
            -shortestPath.normalized(),
            ray
        );
}

std::array<double, 2> Plane::getTextureCoordinatesFor(const Point &) const {
    return {0, 0};
}

Vector Plane::projectOn(const Vector& v) const {
    return v - project(v, Normal);
}

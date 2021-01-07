//
// Created by cleme on 06/01/2021.
//

#include "Plane.h"
#include "commongeometry.h"

Point Plane::getIntersectionPoint(const Ray& ray) const {

    Vector rayOrthogonal = project(ray.Direction, Normal);

    Vector shortestPath = project(Origin - ray.Origin, Normal);

    double t = shortestPath.norm() / rayOrthogonal.norm();
    return ray.at(t);
}

Hit Plane::intersect(const Ray &ray) {
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

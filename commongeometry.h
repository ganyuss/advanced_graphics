//
// Created by cleme on 06/01/2021.
//

#ifndef RAYTRACER_COMMONGEOMETRY_H
#define RAYTRACER_COMMONGEOMETRY_H

#define _USE_MATH_DEFINES
#include <cmath>
#include "triple.h"

inline Vector project(const Vector& vectorToProject, const Vector& reference) {
    return (vectorToProject.dot(reference) / reference.dot(reference)) * reference;
}

double angleBetween(const Vector&, const Vector&);
Vector rotateAround(const Vector& vectorToRotate, const Vector& vectorReference, double angleDegrees);
Vector getThirdOrthogonalVector(const Vector&, const Vector&);
Vector getAnyOrthogonalVector(const Vector&);

#endif //RAYTRACER_COMMONGEOMETRY_H

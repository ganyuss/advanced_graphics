//
// Created by cleme on 06/01/2021.
//

#ifndef RAYTRACER_COMMONGEOMETRY_H
#define RAYTRACER_COMMONGEOMETRY_H

#include "triple.h"
#include "light.h"

inline Vector project(const Vector& vectorToProject, const Vector& reference) {
    return (vectorToProject.dot(reference) / reference.dot(reference)) * reference;
}

double angleBetween(const Vector&, const Vector&);
Vector rotateAround(const Vector& vectorToRotate, const Vector& vectorReference, double angleDegrees);
Vector getThirdOrthogonalVector(const Vector&, const Vector&);


#endif //RAYTRACER_COMMONGEOMETRY_H
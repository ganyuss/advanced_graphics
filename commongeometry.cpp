//
// Created by cleme on 06/01/2021.
//

#include "commongeometry.h"

double angleBetween(const Vector &v1, const Vector &v2) {
    return std::acos(v1.normalized().dot(v2.normalized())) * 180 / M_PI;
}


Vector rotateAround(const Vector &vectorToRotate, const Vector &vectorReference, double angleDegrees) {

    // source : https://math.stackexchange.com/questions/511370/how-to-rotate-one-vector-about-another

    Vector vectorToRotateParallel = project(vectorToRotate, vectorReference);
    Vector vectorToRotateOrthogonal = vectorToRotate - vectorToRotateParallel;

    Vector w = getThirdOrthogonalVector(vectorToRotateOrthogonal, vectorReference.normalized());

    double x1 = std::cos(angleDegrees * M_PI / 180) / vectorToRotateOrthogonal.norm();
    double x2 = std::sin(angleDegrees * M_PI / 180) / w.norm();

    Vector newOrthogonalComponent = vectorToRotateOrthogonal.norm() * (x1*vectorToRotateOrthogonal + x2*w);

    return vectorToRotateParallel + newOrthogonalComponent;
}

Vector getThirdOrthogonalVector(const Vector& v1, const Vector& v2) {
    return v1.cross(v2);
}

Vector getAnyOrthogonalVector(const Vector &v) {
    Vector output = getThirdOrthogonalVector(v, Vector{1, 0, 0});

    if (output == Vector{0, 0, 0})
        return Vector{0, 1, 0};
    else
        return output;
}

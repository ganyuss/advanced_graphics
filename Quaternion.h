//
// Created by cleme on 24/01/2021.
//

#ifndef RAYTRACER_QUATERNION_H
#define RAYTRACER_QUATERNION_H

#include "triple.h"

class Quaternion {
public:
    double x, y, z, w; // Q = xi + yj + zk + w

    Quaternion(double x, double y, double z, double w)
        : x(x), y(y), z(z), w(w)
    { }

    Quaternion() : Quaternion(0, 0, 0, 0)
    { }

    explicit Quaternion(const Vector& v) : Quaternion(v.X(), v.Y(), v.Z(), 0)
    { }

    [[nodiscard]] double norm() const;
    void normalize();
    [[nodiscard]] Quaternion normalized() const;
    [[nodiscard]] Quaternion conjugate() const;
    [[nodiscard]] Quaternion inverse() const;

    explicit operator Vector() const;

    Vector applyRotation(const Vector& input) const;
};

Quaternion& operator*=(Quaternion&, const Quaternion&);
Quaternion operator*(const Quaternion&, const Quaternion&);
Quaternion operator*(double, const Quaternion&);
Quaternion operator*(const Quaternion&, double);

#endif //RAYTRACER_QUATERNION_H

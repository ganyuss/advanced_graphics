//
// Created by cleme on 24/01/2021.
//

#include "Quaternion.h"

double Quaternion::norm() const {
    return std::sqrt(x*x + y*y + z*z + w*w);
}

void Quaternion::normalize() {
    double normalizationFactor = 1 / norm();
    x *= normalizationFactor;
    y *= normalizationFactor;
    z *= normalizationFactor;
    w *= normalizationFactor;
}

Quaternion Quaternion::normalized() const {
    Quaternion copy{*this};
    copy.normalize();
    return copy;
}

Quaternion::operator Vector() const {
    return Vector(x, y, z);
}

Vector Quaternion::applyRotation(const Vector &input) const {
    Quaternion rotation{*this};
    Quaternion rotationInverse{this->inverse()};
    Quaternion objectToRotate{input};

    return Vector(rotation * objectToRotate * rotationInverse);
}

Quaternion Quaternion::conjugate() const {
    return Quaternion(-x, -y, -z, w);
}

Quaternion Quaternion::inverse() const {
    return 1 / (this->norm()*this->norm()) * conjugate();
}

Quaternion& operator*=(Quaternion& q1, const Quaternion& q2) {
    q1 = q1*q2;
    return q1;
}

Quaternion operator*(double d, const Quaternion& q1) {
    Quaternion res = {};
    res.x = q1.x * d;
    res.y = q1.y * d;
    res.z = q1.z * d;
    res.w = q1.w * d;
    return res;
}
Quaternion operator*(const Quaternion& q1, double d) {
    return d*q1;
}


Quaternion operator*(const Quaternion& q1, const Quaternion& q2) {
    Quaternion output{};
    // source: https://en.wikipedia.org/wiki/Quaternion#Hamilton_product
    output.x = q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y;
    output.y = q1.w*q2.y - q1.x*q2.z + q1.y*q2.w + q1.z*q2.x;
    output.z = q1.w*q2.z + q1.x*q2.y - q1.y*q2.x + q1.z*q2.w;
    output.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
    return output;
}

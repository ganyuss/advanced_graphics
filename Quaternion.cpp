//
// Created by cleme on 24/01/2021.
//

#include "Quaternion.h"

double Quaternion::norm() const {
    return x*x + y*y + z*z + w*w;
}

void Quaternion::normalize() {
    double normValue = norm();
    if (normValue != 1) {
        x /= normValue;
        y /= normValue;
        z /= normValue;
        w /= normValue;
    }
}

Quaternion Quaternion::normalized() const {
    Quaternion copy{*this};
    copy.normalize();
    return copy;
}

Quaternion::operator Vector() const {
    return Vector(x, y, z);
}

Vector Quaternion::applyRotation(const Vector &input) {
    Quaternion rotation{*this};
    Quaternion rotationConjugate{this->conjugate()};
    Quaternion objectToRotate{input};

    return Vector(rotationConjugate * objectToRotate * rotation);
}

Quaternion Quaternion::conjugate() const {
    return Quaternion(-x, -y, -z, w);
}

Quaternion& operator*=(Quaternion& q1, const Quaternion& q2) {
    q1 = q1*q2;
    return q1;
}

Quaternion operator*(const Quaternion& q1, const Quaternion& q2) {
    Quaternion output{};
    // source: https://en.wikipedia.org/wiki/Quaternion#Hamilton_product
    output.x = q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y;
    output.y = q1.w*q2.y - q1.x*q2.z + q1.y*q2.w - q1.z*q2.x;
    output.z = q1.w*q2.z + q1.x*q2.y - q1.y*q2.x + q1.z*q2.w;
    output.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
    return output;
}

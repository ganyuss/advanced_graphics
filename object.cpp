//
// Created by cleme on 21/02/2021.
//

#include "object.h"

Color Object::getColorOnPosition(const Point& position) const {
    if (material.texture.has_value()) {
        std::array<double, 2> uv = getTextureCoordinatesFor(position);
        return material.texture.value().colorAt(uv[0], uv[1]);
    }
    else {
        return material.color;
    }
}

double Object::getSpecularOnPosition(const Point& position) const {
    if (material.specularMap.has_value()) {
        std::array<double, 2> uv = getTextureCoordinatesFor(position);
        return material.specularMap.value().colorAt(uv[0], uv[1]).Red(); // Reading the red channel here, doesn't matter
    }
    else {
        return material.ks;
    }
}

Vector Object::applyNormalMap(const Point& position, const Vector& normal, const Vector& up) const {
    if (material.normalMap.has_value()) {
        std::array<double, 2> uv = getTextureCoordinatesFor(position);
        Vector left = getThirdOrthogonalVector(up, normal).normalized();
        Vector normalComponents = Vector{material.normalMap.value().colorAt(uv[0], uv[1])};
        normalComponents = normalComponents * 2 - 1; // converting to vector to bypass overflow prevention
        return (
                left.normalized() * normalComponents.X()
                + up.normalized() * normalComponents.Y()
                + normal.normalized() * normalComponents.Z()
        ).normalized();
    }
    else {
        return normal;
    }
}

std::array<double, 3> Object::getAdditionalLightFactor(const Light &light, const Point &position) const {
    if (material.refractedLightMaps.find(light) != material.refractedLightMaps.end()) {
        std::array<double, 2> uv = getTextureCoordinatesFor(position);

        return material.refractedLightMaps.at(light).colorAt(uv[0], uv[1]).value();
    }

    return {0, 0, 0};
}

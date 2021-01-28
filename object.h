//
//  Framework for a raytracer
//  File: object.h
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Maarten Everts
//    Jasper van de Gronde
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#ifndef OBJECT_H_AXKLE0OF
#define OBJECT_H_AXKLE0OF

#include "triple.h"
#include "material.h"
#include <array>
#include "Quaternion.h"
#include "commongeometry.h"

class Hit;
class Ray;

class Object {
public:
    Material material;

    virtual Hit intersect(const Ray &ray) = 0;
    virtual std::array<double, 2> getTextureCoordinatesFor(Point) = 0;

    inline Color getColorOnPosition(const Point& position) {
        if (material.texture.has_value()) {
            std::array<double, 2> uv = getTextureCoordinatesFor(position);
            return material.texture.value().colorAt(uv[0], uv[1]);
        }
        else {
            return material.color;
        }
    }

    inline double getSpecularOnPosition(const Point& position) {
        if (material.specularMap.has_value()) {
            std::array<double, 2> uv = getTextureCoordinatesFor(position);
            return material.specularMap.value().colorAt(uv[0], uv[1]).Red(); // Reading the red channel here, doesn't matter
        }
        else {
            return material.ks;
        }
    }

    inline Vector applyNormalMap(const Point& position, const Vector& normal, const Vector& up) {
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

    virtual ~Object() = default;
};

#endif /* end of include guard: OBJECT_H_AXKLE0OF */

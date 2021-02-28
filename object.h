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

struct Material;

class Object {
public:
    Material material;
    const Point Position;

    Object(const Point& Position) : Position(Position)
    { }

    [[nodiscard]] virtual Hit intersect(const Ray &ray) const = 0;
    [[nodiscard]] virtual std::array<double, 2> getTextureCoordinatesFor(const Point &) const = 0;

    [[nodiscard]] Color getColorOnPosition(const Point& position) const;
    [[nodiscard]] double getSpecularOnPosition(const Point& position) const;
    [[nodiscard]] Vector applyNormalMap(const Point& position, const Vector& normal, const Vector& up) const;
    [[nodiscard]] std::array<double, 3> getAdditionalLightFactor(const Light&, const Point& position) const;

    virtual ~Object() = default;
};

#endif /* end of include guard: OBJECT_H_AXKLE0OF */

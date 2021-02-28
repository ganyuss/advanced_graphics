//
// Created by Enzo on 05/01/2021.
//

#ifndef QUAD_H_115209AE
#define QUAD_H_115209AE

#include <unordered_map>
#include "object.h"
#include "Plane.h"

class Quadrilateral : public Object {
public:

    Quadrilateral(const Point& Position, const Vector& Up, const Vector& Side)
        : Object(Position), Up(Up), Side(Side)
    { }

    [[nodiscard]] Hit intersect(const Ray &ray) const override;
    [[nodiscard]] std::array<double, 2> getTextureCoordinatesFor(const Point &) const override;

    const Vector Up;
    const Vector Side;

private:

    const Plane ownPlane{Position, getThirdOrthogonalVector(Side, Up)};

    // Used in UV calculations
    const double sideComponentFactor = 1 / ownPlane.Normal.dot(Side.cross(Up));
    const double upComponentFactor = 1  / ownPlane.Normal.dot(Up.cross(Side));
};

class Box : public Object {
public:
    Box(const Point& Position, const Vector& Up, const Vector& Side, float Depth) :
    Object(Position), Faces(computeFaces(Position, Up, Side, Depth))  { }

    [[nodiscard]] Hit intersect(const Ray &ray) const override;
    [[nodiscard]] std::array<double, 2> getTextureCoordinatesFor(const Point &) const override;

    const std::array<Quadrilateral, 6> Faces;

private:
    static std::array<Quadrilateral, 6> computeFaces(const Point &vector,
                                                    const Vector &vector1,
                                                    const Vector &vector2,
                                                    float depth);


    static std::unordered_map<Point, const Quadrilateral&> parentOf;
};


#endif

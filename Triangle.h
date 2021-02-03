//
// Created by cleme on 30/01/2021.
//

#ifndef RAYTRACER_TRIANGLE_H
#define RAYTRACER_TRIANGLE_H

#include "triple.h"
#include "object.h"
#include "Plane.h"

struct Vertex {
    Point Position;
    Vector Normal;
    std::array<double, 2> UV;

    Vertex(const Point& position)
        : Position(position),
        Normal(),
        UV()
    { }

    Vertex(const Point& Position, const Vector& Normal, const std::array<double, 2> UV)
        : Position(Position), Normal(Normal), UV(UV)
    { }
};

class Triangle : public Object {
public:

    const std::array<Vertex, 3> Vertices;
    const double Area;

    Triangle(Vertex vertex1, Vertex vertex2, Vertex vertex3)
        : Vertices({vertex1, vertex2, vertex3}),
        Area(computeArea()), normalUp(computeNormalUp()),
        ownPlane(
                Vertices[0].Position,
                getThirdOrthogonalVector(
                        Vertices[0].Position - Vertices[1].Position,
                        Vertices[0].Position - Vertices[2].Position)
                )
    { }


    [[nodiscard]] Hit intersect(const Ray &ray) const override;
    [[nodiscard]] std::array<double, 2> getTextureCoordinatesFor(const Point &) const override;


private:

    typedef std::array<double, 3> BarycentricCoordinates;

    [[nodiscard]] BarycentricCoordinates computeBarycentricCoordinates(const Point &) const;
    [[nodiscard]] double computeArea() const;
    [[nodiscard]] Vector computeNormalUp() const;
    [[nodiscard]] Vertex extrapolateFor(const BarycentricCoordinates&, const Vector& position) const;

    [[nodiscard]] static double computeAreaBetween(const Point& p1, const Point& p2, const Point& p3) {
        // source: https://users.csc.calpoly.edu/~zwood/teaching/csc471/2017F/barycentric.pdf
        Vector v1 = p2 - p1;
        Vector v2 = p3 - p1;

        return v1.cross(v2).norm() / 2;
    }

    const Plane ownPlane;
    const Vector normalUp;
};


#endif //RAYTRACER_TRIANGLE_H

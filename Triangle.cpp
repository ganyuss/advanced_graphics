//
// Created by cleme on 30/01/2021.
//

#include "Triangle.h"
#include "light.h"


Hit Triangle::intersect(const Ray &ray) const {

    Hit planeHit = ownPlane.intersect(ray);

    if (planeHit == Hit::NO_HIT())
        return Hit::NO_HIT();

    BarycentricCoordinates barycentricCoordinates = computeBarycentricCoordinates(planeHit.Position);


    if (std::any_of(
            barycentricCoordinates.begin(), barycentricCoordinates.end(),
            [] (double d) { return d < 0 || d > 1; }))
        return Hit::NO_HIT();

    if (barycentricCoordinates[0] + barycentricCoordinates[1] + barycentricCoordinates[2] > 1 + std::numeric_limits<double>::epsilon()) {
        return Hit::NO_HIT();
    }

    Vertex extrapolationOnHit = extrapolateFor(barycentricCoordinates, planeHit.Position);

    return {
            (extrapolationOnHit.Position - ray.Origin).norm(),
            extrapolationOnHit.Position,
            applyNormalMap(extrapolationOnHit.Position, extrapolationOnHit.Normal, normalUp),
            ray
    };
}

std::array<double, 2> Triangle::getTextureCoordinatesFor(const Point &p) const {

    BarycentricCoordinates barycentricCoordinates = computeBarycentricCoordinates(p);

    return extrapolateFor(barycentricCoordinates, p).UV;
}

Triangle::BarycentricCoordinates Triangle::computeBarycentricCoordinates(const Point &p) const {

    BarycentricCoordinates output;

    for (std::size_t i = 0; i < 3; ++i) {
        output[i] = computeAreaBetween(p, Vertices[(i+1)%3].Position, Vertices[(i+2)%3].Position) * OneOverArea;
    }

    return output;
}

double Triangle::computeArea() const {
    return computeAreaBetween(
            Vertices[0].Position,
            Vertices[1].Position,
            Vertices[2].Position
    );
}

Vertex Triangle::extrapolateFor(const Triangle::BarycentricCoordinates & barycentric, const Vector& position) const {

    Vertex output{position};

    for (std::size_t i = 0; i < 3; ++i) {
        output.Normal += barycentric[i] * Vertices[i].Normal;
        output.UV[0] += barycentric[i] * Vertices[i].UV[0];
        output.UV[1] += barycentric[i] * Vertices[i].UV[1];
    }

    return output;
}

Vector Triangle::computeNormalUp() const {
    Vector output =
            (Vertices[1].UV[0] - Vertices[0].UV[0]) * (Vertices[1].Position - Vertices[0].Position)
            + (Vertices[2].UV[0] - Vertices[0].UV[0]) * (Vertices[2].Position - Vertices[0].Position);

    if (output == Vector{0, 0, 0}) {
        return Vector{1, 0, 0};
    }

    return output.normalized();
}

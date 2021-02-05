//
// Created by cleme on 31/01/2021.
//

#include "TriangleAggregate.h"

std::unordered_map<Point, const Triangle&> TriangleAggregate::parentOf{};

Sphere TriangleAggregate::computeCircumscribedSphere() const {

    // Not the most exact method, but quick enough with good enough results

    Point center{};

    for (const Triangle& triangle : triangles) {
        for (const Vertex& vertex : triangle.Vertices) {
            center += vertex.Position;
        }
    }

    center /= triangles.size()*3;

    Point furthest{center};
    double furthestDistance = 0;

    for (const Triangle& triangle : triangles) {
        for (const Vertex& vertex : triangle.Vertices) {
            double distance = (center - vertex.Position).norm2();
            if (distance >  furthestDistance) {
                furthestDistance = distance;
                furthest = vertex.Position;
            }
        }
    }

    return Sphere{center, furthestDistance};
}

Hit TriangleAggregate::intersect(const Ray &ray) const {

    if (circumscribedSphere.intersect(ray) == Hit::NO_HIT()) {
        return Hit::NO_HIT();
    }

    int triangleHitIndex = -1;
    Hit finalHit = Hit::NO_HIT();

    for (std::size_t i = 0; i < triangles.size(); ++i) {
        Hit currentHit = triangles[i].intersect(ray);

        if (currentHit.Distance < finalHit.Distance) {
            triangleHitIndex = i;
            finalHit = currentHit;
        }
    }

    if (finalHit != Hit::NO_HIT()) {
        #pragma omp critical
        TriangleAggregate::parentOf.insert(std::pair<Vector, const Triangle&>(finalHit.Position, triangles[triangleHitIndex]));
        return finalHit;
    }
    else {
        return Hit::NO_HIT();
    }
}

std::array<double, 2> TriangleAggregate::getTextureCoordinatesFor(const Point &position) const {
    return TriangleAggregate::parentOf.at(position).getTextureCoordinatesFor(position);
}

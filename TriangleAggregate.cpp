//
// Created by cleme on 31/01/2021.
//

#include "TriangleAggregate.h"

std::unordered_map<Point, const Triangle&> TriangleAggregate::parentOf{};

Sphere TriangleAggregate::computeCircumscribedSphere() const {

    // Not the most exact method, but quick enough with good enough results

    double minX = triangles[0].Vertices[0].Position.X(),
           maxX = triangles[0].Vertices[0].Position.X(),
           minY = triangles[0].Vertices[0].Position.Y(),
           maxY = triangles[0].Vertices[0].Position.Y(),
           minZ = triangles[0].Vertices[0].Position.Z(),
           maxZ = triangles[0].Vertices[0].Position.Z();

    for (const Triangle& triangle : triangles) {
        for (const Vertex& vertex : triangle.Vertices) {
            if (vertex.Position.X() < minX)
                minX = vertex.Position.X();
            if (vertex.Position.X() > maxX)
                maxX = vertex.Position.X();
            if (vertex.Position.Y() < minY)
                minY = vertex.Position.Y();
            if (vertex.Position.Y() > maxY)
                maxY = vertex.Position.Y();
            if (vertex.Position.Z() < minZ)
                minZ = vertex.Position.Z();
            if (vertex.Position.Z() > maxZ)
                maxZ = vertex.Position.Z();
        }
    }

    Point center{(maxX + minX)/2, (maxY + minY)/2, (maxZ + minZ)/2};

    return Sphere{center, std::max({maxX - minX, maxY - minY, maxZ - minZ}) / 2};
}

Hit TriangleAggregate::intersect(const Ray &ray) const {
    //return circumscribedSphere.intersect(ray);
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

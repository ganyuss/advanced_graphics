//
// Created by Enzo on 05/01/2021.
//

#include "box.h"

std::unordered_map<Point, const Quadrilateral&> Box::parentOf{};

Hit Quadrilateral::intersect(const Ray &ray) const {

    Hit planeHit = ownPlane.intersect(ray);

    if (planeHit == Hit::NO_HIT())
        return Hit::NO_HIT();

    std::array<double, 2> UV = getTextureCoordinatesFor(planeHit.Position);

    if (UV[0] < 0 || UV[0] > 1 || UV[1] < 0 || UV[1] > 1) {
        return Hit::NO_HIT();
    }

    return {(planeHit.Position - ray.Origin).norm(), planeHit.Position, planeHit.Normal, ray};
}

std::array<double, 2> Quadrilateral::getTextureCoordinatesFor(const Point& p) const {
    Vector positionToPoint = p - Position;

    // source: https://math.stackexchange.com/questions/148199/equation-for-non-orthogonal-projection-of-a-point-onto-two-vectors-representing
    // (second answer)

    const Vector& nz = ownPlane.Normal;

    double sideComponent = nz.dot(Side.cross(positionToPoint)) * sideComponentFactor;
    double upComponent = nz.dot(Up.cross(positionToPoint)) * upComponentFactor;

    // magical transformations
    return {sideComponent , 1-upComponent};
}

Hit Box::intersect(const Ray &ray) const {

    int triangleHitIndex = -1;
    Hit finalHit = Hit::NO_HIT();

    for (std::size_t i = 0; i < Faces.size(); ++i) {
        Hit currentHit = Faces[i].intersect(ray);

        if (currentHit.Distance < finalHit.Distance) {
            triangleHitIndex = i;
            finalHit = currentHit;
        }
    }

    if (finalHit != Hit::NO_HIT()) {
        #pragma omp critical
        Box::parentOf.insert(std::pair<Vector, const Quadrilateral&>(finalHit.Position, Faces[triangleHitIndex]));
        return finalHit;
    }
    else {
        return Hit::NO_HIT();
    }
}

std::array<double, 2> Box::getTextureCoordinatesFor(const Point& position) const {
    return Box::parentOf.at(position).getTextureCoordinatesFor(position);
}

std::array<Quadrilateral, 6>
Box::computeFaces(const Point& Position, const Vector& Up, const Vector& Side, float depth) {

    Vector depthVector = getThirdOrthogonalVector(Up, Side).normalized() * depth;

    // Be careful of the order of the vector, this determines the normals used
    return {
        Quadrilateral{Position, Side, Up},
        Quadrilateral{Position, Up, depthVector},
        Quadrilateral{Position, depthVector, Side},
        Quadrilateral{Position+Side, depthVector, Up},
        Quadrilateral{Position+Up, Side, depthVector},
        Quadrilateral{Position+depthVector, Up, Side}
    };

}

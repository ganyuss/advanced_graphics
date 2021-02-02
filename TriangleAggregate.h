//
// Created by cleme on 31/01/2021.
//

#ifndef RAYTRACER_TRIANGLEAGGREGATE_H
#define RAYTRACER_TRIANGLEAGGREGATE_H


#include <utility>
#include <vector>
#include <unordered_map>
#include "Triangle.h"
#include "sphere.h"

class TriangleAggregate : Object {
public:

    TriangleAggregate(std::initializer_list<Triangle> triangles)
            : triangles(triangles)
    { }

    TriangleAggregate(std::vector<Triangle>  triangles)
            : triangles(std::move(triangles))
    { }


    [[nodiscard]] Hit intersect(const Ray &ray) const override;
    [[nodiscard]] std::array<double, 2> getTextureCoordinatesFor(const Point &) const override;


private:

    [[nodiscard]] Sphere computeCircumscribedSphere() const;

    const std::vector<Triangle> triangles;
    const Sphere circumscribedSphere = computeCircumscribedSphere();

    static std::unordered_map<Point, const Triangle&> parentOf;
};


#endif //RAYTRACER_TRIANGLEAGGREGATE_H

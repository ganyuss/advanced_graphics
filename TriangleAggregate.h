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
#include "glm.h"


class TriangleAggregate : public Object {
public:

    TriangleAggregate(std::initializer_list<Triangle> triangles)
            : triangles(triangles)
    { }

    TriangleAggregate(std::vector<Triangle>  triangles)
            : triangles(std::move(triangles))
    { }

    TriangleAggregate(std::string fileName) : TriangleAggregate(objParsing(fileName))
    { }

    std::vector<Triangle> objParsing(std::string fileName){
        char* fName = (char*) malloc(fileName.size() + 1);
        fName[fileName.size()] = '\0';
        std::copy(fileName.begin(), fileName.end(), fName);

        GLMmodel *model = glmReadOBJ(fName);
        free(fName);

        std::vector<Triangle> triangles{};

        for(int i = 0; i < model->numtriangles; i++){

            std::vector<int> vertex_indices; // indices pour recup les points du triangle
            for (int j = 0; j < 3; j++){
                vertex_indices.push_back(model->triangles[i].vindices[j]);
            }

            Point p0{model->vertices[vertex_indices[0]], model->vertices[vertex_indices[0] + 1], model->vertices[vertex_indices[0] + 2]};
            Point p1{model->vertices[vertex_indices[1]], model->vertices[vertex_indices[1] + 1], model->vertices[vertex_indices[1] + 2]};
            Point p2{model->vertices[vertex_indices[2]], model->vertices[vertex_indices[2] + 1], model->vertices[vertex_indices[2] + 2]};

            Triangle t{p0, p1, p2};// besoin de 3 vertex = points a 3 coordonnées

            triangles.push_back(t);
        }
        return triangles;
    }


    [[nodiscard]] Hit intersect(const Ray &ray) const override;
    [[nodiscard]] std::array<double, 2> getTextureCoordinatesFor(const Point &) const override;


private:

    [[nodiscard]] Sphere computeCircumscribedSphere() const;

    const std::vector<Triangle> triangles;
    const Sphere circumscribedSphere = computeCircumscribedSphere();

    static std::unordered_map<Point, const Triangle&> parentOf;
};


#endif //RAYTRACER_TRIANGLEAGGREGATE_H

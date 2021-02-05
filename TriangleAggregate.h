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

    static std::vector<Triangle> objParsing(std::string fileName){
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

            int* normalIndices = model->triangles[i].nindices;

            Point p0{model->vertices[vertex_indices[0]*3], model->vertices[vertex_indices[0]*3 + 1], model->vertices[vertex_indices[0]*3 + 2]};
            Point p1{model->vertices[vertex_indices[1]*3], model->vertices[vertex_indices[1]*3 + 1], model->vertices[vertex_indices[1]*3 + 2]};
            Point p2{model->vertices[vertex_indices[2]*3], model->vertices[vertex_indices[2]*3 + 1], model->vertices[vertex_indices[2]*3 + 2]};

            Vector n0{model->normals[normalIndices[0]*3], model->normals[normalIndices[0]*3+1], model->normals[normalIndices[0]*3+2]};
            Vector n1{model->normals[normalIndices[1]*3], model->normals[normalIndices[1]*3+1], model->normals[normalIndices[1]*3+2]};
            Vector n2{model->normals[normalIndices[2]*3], model->normals[normalIndices[2]*3+1], model->normals[normalIndices[2]*3+2]};

            Triangle t{
                Vertex{p0, n0, {0, 0}},
                Vertex{p1, n1, {0, 0}},
                Vertex{p2, n2, {0, 0}},
                };// besoin de 3 vertex

            triangles.push_back(t);
        }

        glmDelete(model);
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

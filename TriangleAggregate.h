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

            int* vertexIndices = model->triangles[i].vindices;
            int* normalIndices = model->triangles[i].nindices;
            int* uvIndices = nullptr;
            if (model->numtexcoords > 0) {
                uvIndices = model->triangles[i].vindices;
            }
            std::vector<Vertex> vertices{};

            for (int j = 0; j < 3; ++j) {
                vertices.push_back(Vertex{
                    Point{model->vertices[vertexIndices[j]*3], model->vertices[vertexIndices[j]*3 + 1], model->vertices[vertexIndices[j]*3 + 2]},
                    Vector{model->normals[normalIndices[j]*3], model->normals[normalIndices[j]*3+1], model->normals[normalIndices[j]*3+2]},
                    (uvIndices ? std::array<double, 2>{model->texcoords[uvIndices[j]*2], model->texcoords[uvIndices[j]*2+1]} : std::array<double, 2>{0, 0})
                });
            }
            triangles.emplace_back(
                vertices[0],
                vertices[1],
                vertices[2]
            );
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

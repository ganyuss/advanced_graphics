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

    TriangleAggregate(const std::string& fileName) : TriangleAggregate(objParsing(fileName))
    { }

    static std::vector<Triangle> objParsing(std::string fileName){
        char* fName = (char*) malloc(fileName.size() + 1);
        fName[fileName.size()] = '\0';
        std::copy(fileName.begin(), fileName.end(), fName);

        GLMmodel *model = glmReadOBJ(fName);
        free(fName);

        if (model->numnormals == 0 && model->numfacetnorms == 0)
            glmFacetNormals(model);

        std::vector<Triangle> triangles{};

        for(int i = 0; i < model->numtriangles; i++){

            int* vertexIndices = model->triangles[i].vindices;
            int* normalIndices = model->triangles[i].nindices;
            int* uvIndices = model->triangles[i].vindices;

            std::vector<Vertex> vertices{};

            for (int j = 0; j < 3; ++j) {

                Point position{model->vertices[vertexIndices[j]*3], model->vertices[vertexIndices[j]*3 + 1], model->vertices[vertexIndices[j]*3 + 2]};

                Vector normal{};
                if (model->numnormals > 0)
                    normal = Vector{model->normals[normalIndices[j]*3], model->normals[normalIndices[j]*3+1], model->normals[normalIndices[j]*3+2]};
                else if (model->numfacetnorms > 0)
                    // facet normals are calculated using counter clockwise vertex order. That is why we use the opposite of the facet normals
                    normal = Vector{model->facetnorms[(i+1)*3], model->facetnorms[(i+1)*3+1], model->facetnorms[(i+1)*3+2]};

                std::array<double, 2> UV{0, 0};
                if (model->numtexcoords > 0)
                    UV = {model->texcoords[uvIndices[j]*2], model->texcoords[uvIndices[j]*2+1]};

                vertices.emplace_back(
                    position,
                    normal,
                    UV
                );
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

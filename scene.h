//
//  Framework for a raytracer
//  File: scene.h
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Maarten Everts
//    Jasper van de Gronde
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#ifndef SCENE_H_KNBLQLP6
#define SCENE_H_KNBLQLP6

#include <vector>
#include <memory>
#include <array>
#include "object.h"
#include "triple.h"
#include "image.h"
#include "yaml/yaml.h"
#include "commongeometry.h"

class Object;

class Light;
class Hit;
class Ray;
class Material;

enum Mode {PHONG, GOOCH, ZBUFFER, NORMAL, TEXTURE};

class Camera {
public:

    Camera(Point Eye, Point Center, Vector Up)
        : eye(Eye),
        scale(Up.norm()),
        viewDirection((Center - Eye).normalized()),
        upDirection(Up.normalized())
    { }

    explicit Camera(Point Eye) : Camera(Eye, {200,200,-200}, {0, 1, 0})
    { }

    Camera() : Camera(Vector{})
    { }

    [[nodiscard]] Vector Eye() const {return eye; }

    std::array<unsigned int, 2> ViewSize = {400, 400};

    [[nodiscard]] inline Point ViewDirection(int x, int y, double dx, double dy) const {

        Vector directionComponentX = (x - ViewSize[0] / 2.f + dx) * sideDirection * scale;
        Vector directionComponentY = (ViewSize[1] / 2.f - y + dy) * upDirection * scale;
        return viewDirection*1000 + directionComponentX + directionComponentY;
    }

private:

    Point eye;
    double scale;
    Vector viewDirection, upDirection;
    Vector sideDirection = getThirdOrthogonalVector(viewDirection, upDirection).normalized();
};


struct GoochIlluminationModel {
    // Default values from original paper: http://artis.imag.fr/~Cyril.Soler/DEA/NonPhotoRealisticRendering/Papers/p447-gooch.pdf
    double b = 0.4;
    double y = 0.4;
    double alpha = 0.2;
    double beta = 0.6;
};


class Scene
{
private:
    std::vector<std::unique_ptr<Object>> objects;
    std::vector<std::unique_ptr<Light>> lights;
    Mode mode;
    int near, far;
    int maxIterations;

public:

    unsigned int superSamplingFactor;
    Camera camera;
    bool SoftShadows = false;
    GoochIlluminationModel goochIlluminationModel;

    unsigned int shadowEdgePrecision, shadowShadePrecision;

    Color trace(const Ray &ray, int iterations);
    Color traceZBuf(const Ray &ray);
    Color traceNormals(const Ray &ray);
    Color traceTextures(const Ray &ray);
    Image render();
    void addObject(std::unique_ptr<Object>&& o);
    void addLight(std::unique_ptr<Light>&& l);
    void setMode(Mode mode);
    void setMaxIterations(int iterations);
    void setNear(int near);
    void setFar(int far);
    void setCamera(Camera c);
    unsigned int getNumObjects() { return objects.size(); }
    unsigned int getNumLights() { return lights.size(); }

private:
    std::unique_ptr<Object>& getObjectHitBy(const Ray&);
    std::unique_ptr<Object>& getObjectHitBy(const Ray&, const std::unique_ptr<Object> &object_ignored);
    float getLightFactorFor(const std::unique_ptr<Light> &light, const Hit &hit, const std::unique_ptr<Object> &object_hit);

    typedef unsigned char IlluminationType;

    const IlluminationType ambient = 0x01;
    const IlluminationType diffuse = 0x02;
    const IlluminationType specular = 0x04;
    const IlluminationType all = ambient | diffuse | specular;

    Color computeReflection(const Hit &, const Material &, int iterations);
    Color computeRefraction(const Hit &current_hit, const Material &, int iterations);
    Color computeIllumination(const Hit &, Scene::IlluminationType, const std::unique_ptr<Object> &object_hit);

    Color computePhong(const Hit &current_hit, Scene::IlluminationType illumination, const std::unique_ptr<Object> &object_hit);
    Color computeGooch(const Hit &, Scene::IlluminationType, const std::unique_ptr<Object> &object_hit);
};

#endif /* end of include guard: SCENE_H_KNBLQLP6 */

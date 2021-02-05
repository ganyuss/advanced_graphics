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
#include "triple.h"
#include "image.h"
#include "yaml/yaml.h"
#include "commongeometry.h"
#include "object.h"

class Light;
class Hit;
class Ray;
class Material;

enum Mode {PHONG, GOOCH, ZBUFFER, NORMAL, TEXTURE};

struct Camera {
    Point Eye;
    Point Center = {200,200,-200};
    Vector Up = {0, 1, 0};
    std::array<unsigned int, 2> ViewSize = {400, 400};
    // float ApertureRadius;
    // float ApertureSamples;

    inline Point ViewDirection(int x, int y, double dx, double dy, int maxX, int maxY) const {
        double scale = Up.norm();
        Vector directionComponentX = (x - maxY / 2 + dx) * 1 * getThirdOrthogonalVector(Center - Eye, Up).normalized() * scale;
        Vector directionComponentY = (maxX / 2 - y + dy) * 1 * Up;
        return (Center - Eye).normalized() + directionComponentX/1000 + directionComponentY/1000;
    }
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
    void render(Image &img);
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

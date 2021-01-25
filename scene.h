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
#include "light.h"
#include "object.h"
#include "image.h"
#include "yaml/yaml.h"
#include "commongeometry.h"

enum Mode {PHONG, GOOCH, ZBUFFER, NORMAL};
void operator>>(const YAML::Node &node, Mode &mode);

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
        return Center - Eye + directionComponentX + directionComponentY;
    }
};
void operator>>(const YAML::Node &node, Camera &camera);


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

    Color trace(const Ray &ray, int iterations);
    Color traceZBuf(const Ray &ray);
    Color traceNormals(const Ray &ray);
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
    float getLightFactorFor(const std::unique_ptr<Light> &light, const Hit &hit);

    typedef unsigned char IlluminationType;

    IlluminationType ambient = 0x01;
    IlluminationType diffuse = 0x02;
    IlluminationType specular = 0x04;
    IlluminationType all = ambient | diffuse | specular;

    Color computeReflection(const Hit &, const Material &, int iterations);
    Color computeRefraction(const Hit &current_hit, const Material &, int iterations);
    Color computeIllumination(const Hit &, Scene::IlluminationType, const std::unique_ptr<Object> &object_hit);

    Color computePhong(const Hit &current_hit, Scene::IlluminationType illumination, const std::unique_ptr<Object> &object_hit);
    Color computeGooch(const Hit &, Scene::IlluminationType, const std::unique_ptr<Object> &object_hit);
};

#endif /* end of include guard: SCENE_H_KNBLQLP6 */

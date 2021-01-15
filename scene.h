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

enum Mode {PHONG, ZBUFFER, NORMAL};
void operator>>(const YAML::Node &node, Mode &mode);

struct Camera {
    Point Eye;
    Point Center;
    Vector Up;
    unsigned int ViewSize[2];
    // float ApertureRadius;
    // float ApertureSamples;

    inline Vector ViewDirection() const { return (Center - Eye).normalized(); }
};
void operator>>(const YAML::Node &node, Camera &camera);




class Scene
{
private:
    std::vector<std::unique_ptr<Object>> objects;
    std::vector<std::unique_ptr<Light>> lights;
    Camera camera;
    Mode mode;
    int near, far;
    int maxIterations;

public:

    unsigned int superSamplingFactor;
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
    float getLightFactorFor(const std::unique_ptr<Light> &light, const Vector &deltaLightPosition, const Vector& hitPoint);

    typedef unsigned char PhongColor;

    PhongColor phong_ambient = 0x01;
    PhongColor phong_diffuse = 0x02;
    PhongColor phong_specular = 0x04;
    PhongColor phong_all = phong_ambient | phong_diffuse | phong_specular;

    Color computeReflection(const Hit &, const Material &, int iterations);
    Color computeRefraction(const Hit &current_hit, const Material &, int iterations);
    Color computePhong(const Hit &, Scene::PhongColor illumination, const Material &);
};

#endif /* end of include guard: SCENE_H_KNBLQLP6 */

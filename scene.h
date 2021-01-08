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

enum Mode {PHONG, ZBUFFER, NORMAL};
void operator>>(const YAML::Node &node, Mode &mode);

class Scene
{
private:
    std::vector<std::unique_ptr<Object>> objects;
    std::vector<std::unique_ptr<Light>> lights;
    Point eye;
    Mode mode;
    int near, far;

public:
    Color trace(const Ray &ray);
    Color traceZBuf(const Ray &ray);
    Color traceNormals(const Ray &ray);
    void render(Image &img);
    void addObject(std::unique_ptr<Object>&& o);
    void addLight(std::unique_ptr<Light>&& l);
    void setMode(Mode mode);
    void setNear(int near);
    void setFar(int far);
    void setEye(Point e);
    unsigned int getNumObjects() { return objects.size(); }
    unsigned int getNumLights() { return lights.size(); }
};

#endif /* end of include guard: SCENE_H_KNBLQLP6 */

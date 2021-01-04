//
//  Framework for a raytracer
//  File: scene.cpp
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

#include <algorithm>
#include <functional>
#include "scene.h"
#include "material.h"

void operator>>(const YAML::Node &node, Mode &mode){
    auto s = node.Read<std::string>();
    std::map<std::string, Mode> map{{"ZBUFFER", Mode::ZBUFFER}, {"PHONG", Mode::PHONG}, {"NORMAL", Mode::NORMAL}};
    mode = map[s];
}
template<typename Iterator, typename Operator>
Iterator optimized_min_element(const Iterator& begin, const Iterator& end, Operator ope) {
    std::vector<decltype(ope(*begin))> values;
    values.reserve(std::distance(begin, end));

    for (auto it = begin; it != end; ++it)
        values.push_back(ope(*it));

    auto minValueIter = std::min_element(values.begin(), values.end());
    return begin + std::distance(values.begin(), minValueIter);
}


Color Scene::trace(const Ray &ray)
{

    auto hit_iterator = optimized_min_element(
            std::begin(objects), std::end(objects),
            [&ray](const std::unique_ptr<Object>& o) { return Hit(o->intersect(ray)).Distance; }
    );

    // No hit? Return background color.
    Hit current_hit = (*hit_iterator)->intersect(ray);
    if (current_hit == Hit::NO_HIT()) return Color(0.0, 0.0, 0.0);

    std::unique_ptr<Object>& obj = *hit_iterator;

    Material& material = obj->material;            //the hit objects material
    Point hit = ray.at(current_hit.Distance);                 //the hit point
    Vector N = current_hit.Normal;                          //the normal at hit point
    Vector V = -ray.Direction;                             //the view vector


    /****************************************************
    * This is where you should insert the color
    * calculation (Phong model).
    *
    * Given: material, hit, N, V, lights[]
    * Sought: color
    *
    * Hints: (see triple.h)
    *        Triple.dot(Vector) dot product
    *        Vector+Vector      vector sum
    *        Vector-Vector      vector difference
    *        Point-Point        yields vector
    *        Vector.normalize() normalizes vector, returns length
    *        double*Color        scales each color component (r,g,b)
    *        Color*Color        dito
    *        pow(a,b)           a to the power of b
    ****************************************************/

    Color output{};

    for (std::unique_ptr<Light> & light_source : lights) {
        output += light_source->computeColorAt(current_hit, material);
    }

    return output;
}

Color Scene::traceZBuf(const Ray &ray)
{
    auto const distMin = 0;
    auto const distMax = 10000;

    auto hit_iterator = optimized_min_element(
            std::begin(objects), std::end(objects),
            [&ray](const std::unique_ptr<Object>& o) { return Hit(o->intersect(ray)).Distance; }
    );

    // No hit? Return background color.
    Hit current_hit = (*hit_iterator)->intersect(ray);
    if (current_hit == Hit::NO_HIT()) return Color(0.0, 0.0, 0.0);

    std::unique_ptr<Object>& obj = *hit_iterator;

    Material& material = obj->material;            //the hit objects material
    Point hit = ray.at(current_hit.Distance);                 //the hit point
    Vector N = current_hit.Normal;                          //the normal at hit point
    Vector V = -ray.Direction;                             //the view vector


    /****************************************************
    * This is where you should insert the color
    * calculation (Phong model).
    *
    * Given: material, hit, N, V, lights[]
    * Sought: color
    *
    * Hints: (see triple.h)
    *        Triple.dot(Vector) dot product
    *        Vector+Vector      vector sum
    *        Vector-Vector      vector difference
    *        Point-Point        yields vector
    *        Vector.normalize() normalizes vector, returns length
    *        double*Color        scales each color component (r,g,b)
    *        Color*Color        dito
    *        pow(a,b)           a to the power of b
    ****************************************************/

    Color output{};

    if (current_hit.Distance < distMax && current_hit.Distance > distMin) {
        auto normalizedDistance = 1.0 - (current_hit.Distance - distMin) / (distMax - distMin);
        output.set(normalizedDistance,normalizedDistance,normalizedDistance);
    } else {
        output.set(0,0,0);
    }

    return output;
}
Color Scene::traceNormals(const Ray &ray)
{
    auto const distMin = 0;
    auto const distMax = 10000;

    auto hit_iterator = optimized_min_element(
            std::begin(objects), std::end(objects),
            [&ray](const std::unique_ptr<Object>& o) { return Hit(o->intersect(ray)).Distance; }
    );

    // No hit? Return background color.
    Hit current_hit = (*hit_iterator)->intersect(ray);
    if (current_hit == Hit::NO_HIT()) return Color(0.0, 0.0, 0.0);

    std::unique_ptr<Object>& obj = *hit_iterator;

    Material& material = obj->material;            //the hit objects material
    Point hit = ray.at(current_hit.Distance);                 //the hit point
    Vector N = current_hit.Normal;                          //the normal at hit point
    Vector V = -ray.Direction;                             //the view vector


    /****************************************************
    * This is where you should insert the color
    * calculation (Phong model).
    *
    * Given: material, hit, N, V, lights[]
    * Sought: color
    *
    * Hints: (see triple.h)
    *        Triple.dot(Vector) dot product
    *        Vector+Vector      vector sum
    *        Vector-Vector      vector difference
    *        Point-Point        yields vector
    *        Vector.normalize() normalizes vector, returns length
    *        double*Color        scales each color component (r,g,b)
    *        Color*Color        dito
    *        pow(a,b)           a to the power of b
    ****************************************************/

    Color output{};

    output.set((current_hit.Normal.X() + 1 )/ 2,(current_hit.Normal.Y() + 1 )/ 2,(current_hit.Normal.Z() + 1 )/ 2);

    return output;
}

void Scene::render(Image &img)
{
    if (mode == Mode::PHONG) {
        int w = img.width();
        int h = img.height();
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                Point pixel(x + 0.5, h - 1 - y + 0.5, 0);
                Ray ray(eye, (pixel - eye).normalized());
                Color col = trace(ray);
                img(x, y) = col;
            }
        }
    } else if (mode == Mode::ZBUFFER){
        int w = img.width();
        int h = img.height();
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                Point pixel(x+0.5, h-1-y+0.5, 0);
                Ray ray(eye, (pixel-eye).normalized());
                Color col = traceZBuf(ray);
                img(x,y) = col;
            }
        }
    } else {
        int w = img.width();
        int h = img.height();
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                Point pixel(x+0.5, h-1-y+0.5, 0);
                Ray ray(eye, (pixel-eye).normalized());
                Color col = traceNormals(ray);
                img(x,y) = col;
            }
        }
    }
}

void Scene::addObject(std::unique_ptr<Object>&& o)
{
    objects.push_back(std::move(o));
}

void Scene::addLight(std::unique_ptr<Light>&& l)
{
    lights.push_back(std::move(l));
}

void Scene::setEye(Point e)
{
    eye = e;
}

void Scene::setMode(Mode m)
{
    mode = m;
}


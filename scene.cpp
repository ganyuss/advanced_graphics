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

    std::unique_ptr<Object>& object = *hit_iterator;

    Color output = object->material.color * object->material.ka;

    for (std::unique_ptr<Light> & light_source : lights) {
        Vector dPosition = current_hit.Position + current_hit.Normal * 0.1;
        Ray newRay = Ray(dPosition, light_source->Position - dPosition);

        auto hit_iterator_new = optimized_min_element(
                std::begin(objects), std::end(objects),
                [&newRay](const std::unique_ptr<Object>& o) { return Hit(o->intersect(newRay)).Distance; }
        );
        Hit current_hit_new = (*hit_iterator_new)->intersect(newRay);

        if (current_hit_new.Distance >= (light_source->Position - dPosition).norm()){
            output += light_source->computeColorAt(current_hit, object->material);
        }
    }

    return output;
}

Color Scene::traceZBuf(const Ray &ray)
{
    auto hit_iterator = optimized_min_element(
            std::begin(objects), std::end(objects),
            [&ray](const std::unique_ptr<Object>& o) { return Hit(o->intersect(ray)).Distance; }
    );

    // No hit? Return background color.
    Hit current_hit = (*hit_iterator)->intersect(ray);
    if (current_hit == Hit::NO_HIT()) return Color(0.0, 0.0, 0.0);

    std::unique_ptr<Object>& obj = *hit_iterator;

    Color output{};

    if (current_hit.Distance < far && current_hit.Distance > near) {
        auto normalizedDistance = 1.0 - (current_hit.Distance - near) / (far - near);
        output.set(normalizedDistance,normalizedDistance,normalizedDistance);
    } else {
        output.set(0,0,0);
    }

    return output;
}
Color Scene::traceNormals(const Ray &ray)
{
    auto hit_iterator = optimized_min_element(
            std::begin(objects), std::end(objects),
            [&ray](const std::unique_ptr<Object>& o) { return Hit(o->intersect(ray)).Distance; }
    );

    // No hit? Return background color.
    Hit current_hit = (*hit_iterator)->intersect(ray);
    if (current_hit == Hit::NO_HIT()) return Color(0.0, 0.0, 0.0);

    std::unique_ptr<Object>& obj = *hit_iterator;

    Color output{};

    output = Color{(current_hit + Vector{1, 1, 1}) / 2};

    return output;
}

void Scene::render(Image &img)
{
    Color (Scene::*traceFunction)(const Ray&) = nullptr;

    switch (mode) {
        case Mode::PHONG:
            traceFunction = &Scene::trace;
            break;
        case ZBUFFER:
            traceFunction = &Scene::traceZBuf;
            break;
        case NORMAL:
            traceFunction = &Scene::traceNormals;
            break;
    }

    int w = img.width();
    int h = img.height();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Point pixel(x + 0.5, h - 1 - y + 0.5, 0);
            Ray ray(eye, (pixel - eye).normalized());
            Color col = (this->*traceFunction)(ray);
            img(x, y) = col;
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

void Scene::setNear(int n){
    near = n;
}
void Scene::setFar(int f){
    far = f;
}

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
#include <stdexcept>
#include "scene.h"
#include "commongeometry.h"
#include <cmath>

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


Color Scene::trace(const Ray &ray, int iterations)
{
    std::unique_ptr<Object>& object = getObjectHitBy(ray);

    // No hit? Return background color.
    Hit current_hit = object->intersect(ray);
    if (current_hit == Hit::NO_HIT()) return Color(0.0, 0.0, 0.0);

    Color output{};

    if (object->material.type == MaterialType::REFRACTION && iterations > 0) {
        for (std::unique_ptr<Light> &light_source : lights) {

            float lightFactor = getLightFactorFor(light_source, current_hit);

            if (lightFactor > 0) {
                output += lightFactor * light_source->computeSpecularColorAt(current_hit, object->material);
            }
        }

        double oldAngle = angleBetween(ray.Direction, current_hit.Normal);

        double indexRatio;
        if (ray.Direction.dot(current_hit.Normal) > 0) indexRatio = object->material.index;
        else indexRatio = 1 / object->material.index;

        double newAngle = std::asin(indexRatio * std::sin(oldAngle * M_PI / 180)) * 180 / M_PI;

        Vector refractedDirection = rotateAround(
                sign(ray.Direction.dot(current_hit.Normal)) * current_hit.Normal,
                getThirdOrthogonalVector(ray.Direction, current_hit.Normal),
                newAngle
        );

        output += trace(Ray{current_hit.Position + refractedDirection * 0.1, refractedDirection }, iterations-1);
    }
    else {
        output = object->material.color * object->material.ka;

        for (std::unique_ptr<Light> &light_source : lights) {

            float lightFactor = getLightFactorFor(light_source, current_hit);

            if (lightFactor > 0) {
                output += lightFactor * (
                        light_source->computeDiffuseColorAt(current_hit, object->material)
                        + light_source->computeSpecularColorAt(current_hit, object->material)
                );
            }
        }

        if (iterations != 0 && object->material.type == MaterialType::REFLECTION) {
            Vector dir = -rotateAround(ray.Direction, current_hit.Normal, 180);
            Ray reflected{current_hit.Position + dir * 0.1, dir};
            output += trace(reflected, iterations - 1) * object->material.ks;
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

    output = Color{(current_hit.Normal + Vector{1, 1, 1}) / 2};

    return output;
}

void Scene::render(Image &img)
{
    Color (*traceFunction)(Scene*, const Ray&) = nullptr;

    switch (mode) {
        case Mode::PHONG:
            traceFunction = [] (Scene* scene, const Ray& ray) { return scene->trace(ray, 3); };
            break;
        case Mode::ZBUFFER:
            traceFunction = [] (Scene* scene, const Ray& ray) { return scene->traceZBuf(ray); };
            break;
        case Mode::NORMAL:
            traceFunction = [] (Scene* scene, const Ray& ray) { return scene->traceNormals(ray); };
            break;
    }

    if (! traceFunction) {
        throw std::invalid_argument("Invalid rendering mode : " + std::to_string(mode));
    }

    int w = img.width();
    int h = img.height();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Point pixel(x + 0.5, h - 1 - y + 0.5, 0);
            Ray ray(eye, (pixel - eye).normalized());
            Color col = traceFunction(this, ray);

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

std::unique_ptr<Object>& Scene::getObjectHitBy(const Ray& ray) {
    auto hit_iterator = optimized_min_element(
            std::begin(objects), std::end(objects),
            [&ray](const std::unique_ptr<Object>& o) { return o->intersect(ray).Distance; }
    );

    return *hit_iterator;
}

float Scene::getLightFactorFor(const std::unique_ptr<Light>& light, const Hit& hit) {

    const int lightSampleNumber = 8;

    Vector dPosition = hit.Position + hit.Normal * 0.1;
    Ray newRay = Ray(dPosition, light->Position - dPosition);

    std::unique_ptr<Object>& newObject = getObjectHitBy(newRay);
    Hit current_hit_new = newObject->intersect(newRay);

    if ((! SoftShadows) || light->Size == 0) {
        return current_hit_new.Distance >= (light->Position - dPosition).norm() ? 1 : 0;
    }

    // We try to get any vector that is not collinear with newRay.Direction
    Vector lightPositionDeltaDirection = Vector{1, 0, 0} - project(Vector{1, 0, 0}, newRay.Direction);

    if (lightPositionDeltaDirection == Vector{0, 0, 0})
        lightPositionDeltaDirection = Vector{0, 1, 0};
    else
        lightPositionDeltaDirection.normalize();
    Vector lightPositionDelta = lightPositionDeltaDirection * light->Size;

    float softShadowFactor = 0;

    for (std::size_t i = 0; i < lightSampleNumber; ++i) {
        Vector dLightPosition = rotateAround(lightPositionDelta, newRay.Direction, (360.f * i) / lightSampleNumber);

        float newSoftShadowFactor = getLightFactorFor(light, dLightPosition, dPosition);

        softShadowFactor += newSoftShadowFactor;
    }

    return softShadowFactor / static_cast<float>(lightSampleNumber);
}

float Scene::getLightFactorFor(const std::unique_ptr<Light>& light, const Vector& deltaLightPosition, const Vector& hitPoint) {

    const int amountOfSamples = 8;

    // Can't use reference here
    std::vector<std::unique_ptr<Object>*> objectsHit;

    auto isFactorShadowed = [&light, &deltaLightPosition, &hitPoint] (float factor, std::unique_ptr<Object>& object) {
        Vector lightPos = light->Position + (deltaLightPosition * factor);
        Ray newRay{hitPoint, lightPos - hitPoint};
        Hit newHit = object->intersect(newRay);
        return newHit.Distance < (newRay.Origin - lightPos).norm();
    };

    for (std::unique_ptr<Object>& object : objects) {
        if (isFactorShadowed(0, object) || isFactorShadowed(1, object)) {
            objectsHit.push_back(&object);
        }
    }

    float shadowFactor = 0;

    for (std::unique_ptr<Object>* object : objectsHit) {

        int ShadowedSamples = 0;

        for (std::size_t i = 0; i < amountOfSamples; ++i) {
            if (isFactorShadowed(i / static_cast<float>(amountOfSamples), *object))
                ShadowedSamples++;
        }

        shadowFactor += ShadowedSamples / static_cast<float>(amountOfSamples);
    }

    return 1 - shadowFactor;
}

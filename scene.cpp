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
#include <vector>
#include <cmath>
#include <cassert>

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

        output += computeIllumination(current_hit, specular, object);
        output += computeRefraction(current_hit, object->material, iterations) * object->getColorOnPosition(current_hit.Position);
    }
    else {

        output += computeIllumination(current_hit, all, object);

        if (iterations != 0 && object->material.type == MaterialType::REFLECTION) {
            output += computeReflection(current_hit, object->material, iterations);
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

Color Scene::traceTextures(const Ray &ray)
{
    auto hit_iterator = optimized_min_element(
            std::begin(objects), std::end(objects),
            [&ray](const std::unique_ptr<Object>& o) { return Hit(o->intersect(ray)).Distance; }
    );

    // No hit? Return background color.
    Hit current_hit = (*hit_iterator)->intersect(ray);
    if (current_hit == Hit::NO_HIT()) return Color(0.0, 0.0, 0.0);

    std::unique_ptr<Object>& obj = *hit_iterator;

    auto uv = obj->getTextureCoordinatesFor(current_hit.Position);

    // to better fit the UV repeating system of the Image class
    for (std::size_t i = 0; i < 2; ++i) {
        if (uv[i] > 0)
            uv[i] = std::abs(uv[i] - static_cast<int>(uv[i]));
        else
            uv[i] = std::abs(1 + uv[i] - static_cast<int>(uv[i]));
    }

    return Color{uv[0], uv[1], 0};
}

Image Scene::render()
{
    Image img(camera.ViewSize[0], camera.ViewSize[1]);

    Color (*traceFunction)(Scene*, const Ray&) = nullptr;

    switch (mode) {
        case Mode::GOOCH:
        case Mode::PHONG:
            traceFunction = [] (Scene* scene, const Ray& ray) { return scene->trace(ray, scene->maxIterations); };
            break;
        case Mode::ZBUFFER:
            traceFunction = [] (Scene* scene, const Ray& ray) { return scene->traceZBuf(ray); };
            break;
        case Mode::NORMAL:
            traceFunction = [] (Scene* scene, const Ray& ray) { return scene->traceNormals(ray); };
            break;
        case Mode::TEXTURE:
            traceFunction = [] (Scene* scene, const Ray& ray) { return scene->traceTextures(ray); };
            break;
    }

    if (! traceFunction) {
        throw std::invalid_argument("Invalid rendering mode : " + std::to_string(mode));
    }

    if (refractedShadows.has_value() && (mode == GOOCH || mode == PHONG)) {
        computeRefractedShadows();
        std::cout << "refracted shadows computed" << std::endl;
    }

    int w = img.width();
    int h = img.height();
    double delta = 1.0 / (superSamplingFactor+1);
    unsigned int rayPerPixel = superSamplingFactor * superSamplingFactor;
    #pragma omp parallel for default(none) shared(traceFunction, img, h, w, delta, rayPerPixel)
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Color pixelColor{};

            for (int i = 0; i < superSamplingFactor; i++) {
                for (int j = 0; j < superSamplingFactor; j++) {
                    Ray ray(camera.Eye(), (camera.ViewDirection(x, y, delta*(i+1), delta*(j+1))).normalized());
                    pixelColor += traceFunction(this, ray) / rayPerPixel;
                }
            }

            #pragma omp critical
            img(x, y) = pixelColor;
        }
    }

    return img;
}

void Scene::addObject(std::unique_ptr<Object>&& o)
{
    objects.push_back(std::move(o));
}

void Scene::addLight(std::unique_ptr<Light>&& l)
{
    lights.push_back(std::move(l));
}

void Scene::setCamera(Camera c)
{
    camera = c;
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

std::unique_ptr<Object>& Scene::getObjectHitBy(const Ray& ray, const std::unique_ptr<Object> &object_ignored) {
    auto hit_iterator = optimized_min_element(
            std::begin(objects), std::end(objects),
            [&ray, &object_ignored](const std::unique_ptr<Object>& o) {
                if (o == object_ignored) return Hit::NO_HIT().Distance;
                return o->intersect(ray).Distance;
            }
    );

    return *hit_iterator;
}

float Scene::getLightFactorFor(const std::unique_ptr<Light>& light, const Hit& hit, const std::unique_ptr<Object> &object_hit) {

    int lightSampleNumber = static_cast<int>(shadowEdgePrecision * shadowEdgePrecision);
    int lightSubSampleNumber = static_cast<int>(shadowShadePrecision);

    Vector positionToLight = (light->Position - hit.Position).normalized();

    Vector dPosition = hit.Position + positionToLight * 0.1;
    Ray newRay = Ray(dPosition, positionToLight);

    std::unique_ptr<Object>& newObject = getObjectHitBy(newRay, object_hit);
    Hit current_hit_new = newObject->intersect(newRay);

    if (object_hit == newObject)
        current_hit_new = Hit::NO_HIT();

    if ((! SoftShadows) || light->Size == 0) {
        return current_hit_new.Distance >= (light->Position - dPosition).norm() ? 1 : 0;
    }

    Vector lightPositionDeltaDirection = getAnyOrthogonalVector(newRay.Direction).normalized();
    Vector lightPositionDelta = lightPositionDeltaDirection * light->Size;

    float softLightFactor = 0;

    for (std::size_t i = 0; i < lightSampleNumber; ++i) {
        Vector dLightPosition = rotateAround(lightPositionDelta, newRay.Direction, (360.f * i) / lightSampleNumber);

        for (std::size_t j = 1; j <= lightSubSampleNumber; ++j) {

            Ray borderRay{dPosition, light->Position + (dLightPosition * (static_cast<float>(j) / lightSubSampleNumber)) - dPosition};
            std::unique_ptr<Object> &objectHit = getObjectHitBy(borderRay);

            if (objectHit->intersect(borderRay) == Hit::NO_HIT())
                softLightFactor++;
            else
                softLightFactor += objectHit->intersect(borderRay).Distance /
                                   (light->Position + dLightPosition - dPosition).norm();
        }
    }

    return softLightFactor / static_cast<float>(lightSampleNumber * lightSubSampleNumber);
}

Color Scene::computeReflection(const Hit& current_hit, const Material& material, int iterations) {

    Vector dir = -rotateAround(current_hit.Source.Direction, current_hit.Normal, 180);
    Ray reflected{current_hit.Position + dir * 0.1, dir};
    return trace(reflected, iterations - 1) * material.ks;
}


Vector getRefractedDirection(const Hit& current_hit, const Material& material) {
    // source: https://computergraphics.stackexchange.com/questions/4573/refraction-in-a-ray-tracer-what-do-with-an-intersection-within-the-medium

    double cosIncidence = current_hit.Source.Direction.dot(current_hit.Normal);
    double index1 = 1, index2 = material.index;
    Vector normal = current_hit.Normal;

    if (cosIncidence < 0) {
        cosIncidence = -cosIncidence;
    }
    else {
        std::swap(index1, index2);
        normal = -current_hit.Normal;
    }

    double indexRatio = index1 / index2;
    double k = 1 - indexRatio * indexRatio * (1 - cosIncidence * cosIncidence);

    if (k >= 0)
        return indexRatio * current_hit.Source.Direction + (indexRatio * cosIncidence - std::sqrt(k)) * normal;
    else
        return Vector{0, 0, 0};
}

Color Scene::computeRefraction(const Hit& current_hit, const Material& material, int iterations) {

    Color output{};

    Vector refractedDirection = getRefractedDirection(current_hit, material);

    if (refractedDirection != Vector{0, 0, 0}) {
        output += trace(Ray{current_hit.Position + refractedDirection * 0.1, refractedDirection}, iterations - 1);
    }
    else {
        output += computeReflection(current_hit, material, iterations);
    }

    return output;
}


Color Scene::computeIllumination(const Hit &hit, Scene::IlluminationType illumination, const std::unique_ptr<Object> &object_hit) {
    if (mode == Mode::PHONG)
        return computePhong(hit, illumination, object_hit);
    if (mode == Mode::GOOCH)
        return computeGooch(hit, illumination, object_hit);

    return Color{0, 0, 0};
}

Color Scene::computePhong(const Hit& current_hit, Scene::IlluminationType illumination, const std::unique_ptr<Object> &object_hit) {

    Color output{};
    Color colorOnHit = object_hit->getColorOnPosition(current_hit.Position);
    double specularOnHit = object_hit->getSpecularOnPosition(current_hit.Position);

    if (illumination & ambient)
        output += colorOnHit * object_hit->material.ka;

    if (illumination & diffuse || illumination & specular) {
        for (std::unique_ptr<Light> &light_source : lights) {

            float lightFactor = getLightFactorFor(light_source, current_hit, object_hit);

            if (lightFactor > 0) {
                if (illumination & diffuse)
                    output += lightFactor * light_source->computeDiffusePhongAt(current_hit, object_hit->material, colorOnHit);
                if (illumination & specular)
                    output += lightFactor * light_source->computeSpecularPhongAt(current_hit, object_hit->material, specularOnHit);
            }

            std::array<double, 3> additionalLightFactor = object_hit->getAdditionalLightFactor(*light_source, current_hit.Position);


            for (std::size_t i = 0; i < 3; ++i) {
                output[i] += light_source->color[i] * additionalLightFactor[i];
            }

        }
    }

    return output;
}

Color Scene::computeGooch(const Hit &current_hit, Scene::IlluminationType illumination, const std::unique_ptr<Object> &object_hit) {
    Color output{};
    Color colorOnHit = object_hit->getColorOnPosition(current_hit.Position);
    double specularOnHit = object_hit->getSpecularOnPosition(current_hit.Position);

    if (illumination & diffuse || illumination & specular) {
        for (std::unique_ptr<Light> &light_source : lights) {
            if (illumination & diffuse)
                output += light_source->computeDiffuseGoochAt(current_hit, object_hit->material, colorOnHit, goochIlluminationModel);
            if (illumination & specular)
                output += light_source->computeSpecularGoochAt(current_hit, object_hit->material, colorOnHit, specularOnHit);
        }
    }

    return output;
}

void Scene::setMaxIterations(int iterations) {
    maxIterations = iterations;
}

void Scene::computeRefractedShadows() {

    #pragma omp parallel for
    for (int i = 0; i < objects.size(); ++i) {
        const auto &target = objects[i];
        if (target->material.type == MaterialType::REFRACTION) {
            for (const auto &light : lights) {
                Vector direction = target->Position - light->Position;
                Vector deltaUp = getAnyOrthogonalVector(direction).normalized() * refractedShadows->precision;
                Vector deltaSide = getThirdOrthogonalVector(direction, deltaUp).normalized() * refractedShadows->precision;

                auto computeAt = [this, &target, &light, &direction, &deltaUp, &deltaSide] (int x, int y) {
                    return computeRefractedShadowsAt(target, light, direction, deltaUp, deltaSide, {x, y});
                };

                computeAt(0, 0);
                for (int dx : {1, -1}) {
                    int x = dx;
                    while (computeAt(x, 0)) {
                        for (int dy : {1, -1}) {
                            int y = dy;
                            while (computeAt(x, y)) {
                                y += dy;
                            }
                        }
                        x += dx;
                    }
                }

            }
        }
    }

    smoothenRefractedShadows();
}

bool Scene::computeRefractedShadowsAt(
        const std::unique_ptr<Object>& target,
        const std::unique_ptr<Light>& light,
        const Vector& direction, const Vector& deltaUp, const Vector& deltaSide,
        const std::array<int, 2>& currentCoordinates) {

    if (target->material.type != MaterialType::REFRACTION) return false;

    Vector currentDirection = direction
            + deltaUp*currentCoordinates[0]
            + deltaSide*currentCoordinates[1];

    Ray currentRay{light->Position, currentDirection};

    const std::unique_ptr<Object>& objectHit = getObjectHitBy(currentRay);

    // No hit? Return background color.
    Hit current_hit = objectHit->intersect(currentRay);
    if (current_hit == Hit::NO_HIT() || objectHit != target) return false;

    computeRefractedLightBeam(light, current_hit, objectHit, objectHit->getColorOnPosition(current_hit.Position));

    return true;
}


void Scene::computeRefractedLightBeam(const std::unique_ptr<Light> &light, const Hit &hit,
                                      const std::unique_ptr<Object> &objectHit, const Color &currentColor) {

    if (objectHit->material.type == MaterialType::REFRACTION) {
        Vector refractedDirection = getRefractedDirection(hit, objectHit->material);
        if (refractedDirection == Vector{0, 0, 0}) return;

        Ray nextRay{hit.Position + refractedDirection * 0.1, refractedDirection};
        const std::unique_ptr<Object>& nextObjectHit = getObjectHitBy(nextRay);
        Hit nextHit = nextObjectHit->intersect(nextRay);
        if (nextHit == Hit::NO_HIT()) return;

        computeRefractedLightBeam(light, nextHit, nextObjectHit,
                                  currentColor * nextObjectHit->getColorOnPosition(nextHit.Position));
    }
    else {
        if (objectHit->material.refractedLightMaps.find(*light) == objectHit->material.refractedLightMaps.end()) {
            objectHit->material.refractedLightMaps.emplace(std::make_pair(*light, BaseImage<std::optional<std::array<double, 3>>>(refractedShadows->textureSize, refractedShadows->textureSize)));
        }

        std::array<double, 2> uv = objectHit->getTextureCoordinatesFor(hit.Position);
        std::optional<std::array<double, 3>>& pixel = objectHit->material.refractedLightMaps[*light].colorAt(uv[0], uv[1]);

        if (! pixel.has_value())
            pixel.emplace(std::array<double, 3>{0, 0, 0});

        for (std::size_t i = 0; i < 3; ++i) {
            pixel.value()[i] += currentColor[i] * refractedShadows->precision * refractedShadows->intensityFactor;
        }
    }
}

void Scene::smoothenRefractedShadows() {

    for (auto & object : objects) {
        for (auto& pair : object->material.refractedLightMaps) {
            auto originalImage = pair.second;
            auto& smoothedImage = pair.second;
            auto distanceFunction = [] (int dx, int dy) { return dx * dx + dy * dy; };

            if (refractedShadows->smoothingFactor > 1) {

                #pragma omp parallel for
                for (int x = 0; x < originalImage.width(); ++x) {
                    for (std::size_t y = 0; y < originalImage.height(); ++y) {

                        if (originalImage(x, y).has_value()) {
                            const auto &origin = originalImage(x, y);

                            for (int dx = -10; dx < 10; ++dx) {
                                int xpos = x + dx;
                                if (xpos < 0 || xpos >= originalImage.width()) continue;

                                for (int dy = -10; dy < 10; ++dy) {

                                    int ypos = y + dy;
                                    if (ypos < 0 || ypos >= originalImage.height()) continue;

                                    double distance = distanceFunction(dx, dy);
                                    if (distance >= refractedShadows->smoothingFactor) continue;

                                    auto &destination = smoothedImage(xpos, ypos);
                                    if (!destination.has_value())
                                        destination.emplace(std::array<double, 3>{0, 0, 0});

                                    for (std::size_t i = 0; i < destination.value().size(); ++i) {
                                        (*destination)[i] = std::max((*destination)[i],
                                                                     (*origin)[i] - distance / refractedShadows->smoothingFactor);
                                    }
                                }
                            }
                        }
                    }
                }
            }


            for (std::size_t x = 0; x < smoothedImage.width(); ++x) {
                for (std::size_t y = 0; y < smoothedImage.height(); ++y) {
                    if (! smoothedImage(x, y).has_value())
                        smoothedImage(x, y).emplace(std::array<double, 3>{0, 0, 0});
                }
            }
        }
    }
}

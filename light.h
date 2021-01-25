//
//  Framework for a raytracer
//  File: light.h
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Maarten Everts
//    Jasper van de Gronde
//    adjustment of Hit class by Olivier Wavrin
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#ifndef LIGHT_H_PG2BAJRA
#define LIGHT_H_PG2BAJRA

#include <iostream>
#include <limits>
#include <algorithm>
#include "triple.h"
#include "material.h"
#include "commongeometry.h"


class Ray
{
public:
    Point Origin;
    Vector Direction;

    Ray(const Point &from, const Vector &dir)
            : Origin(from), Direction(dir.normalized())
    { }

    Point at(double t) const
    { return Origin + t * Direction; }
};

bool inline operator==(const Ray &r1, const Ray &r2) {
    return r1.Origin == r2.Origin && r1.Direction == r2.Direction;
}

bool inline operator!=(const Ray &r1, const Ray &r2) {
    return ! (r1 == r2);
}

class Hit
{
public:
    double Distance;
    Point Position;
    Vector Normal;
    Ray Source;

    Hit(double Distance, Point Position, Vector Normal, Ray Source)
            : Distance(Distance), Position(Position), Normal(Normal), Source(Source)
    { }

    static const Hit& NO_HIT() {
        static Hit no_hit(
                std::numeric_limits<double>::infinity(),
                Point{},
                Vector{},
                Ray{Point{}, Vector{}}
        );
        return no_hit;
    }
};

bool inline operator==(const Hit& h1, const Hit& h2) {
    if (h1.Distance == std::numeric_limits<double>::infinity()
            || h2.Distance  == std::numeric_limits<double>::infinity())
        return h1.Distance == h2.Distance;
    return h1.Distance == h2.Distance
           && h1.Position == h2.Position
           && h1.Normal == h2.Normal
           && h1.Source == h2.Source;
}

bool inline operator!=(const Hit& h1, const Hit& h2) {
    return ! (h1 == h2);
}


class Light
{
public:
    Light(Point Position, Color c, float size) : Position(Position), color(c), Size(size)
    { }

    [[nodiscard]] virtual Color computeSpecularPhongAt(const Hit& hit_point, const Material& material) const {
        Vector ray_reflection = -rotateAround(hit_point.Source.Direction, hit_point.Normal, 180);
        ray_reflection.normalize();
        Vector lightIncidence = Position - hit_point.Position;
        lightIncidence.normalize();

        double specularFactor = ray_reflection.dot(lightIncidence);
        if (specularFactor < 0){
            specularFactor = 0;
        }
        Color specularColor = color * material.ks * pow(specularFactor,material.n);

        return specularColor;

    }

    [[nodiscard]] virtual Color computeDiffusePhongAt(const Hit& hit_point, const Material& material, const Color& colorOnHit) const {

        Vector lightIncidence = Position - hit_point.Position;
        lightIncidence.normalize();
        double diffuseFactor = hit_point.Normal.dot(lightIncidence);
        Color diffuseColor{};
        if (diffuseFactor <= 0){
            diffuseColor.set(0,0,0);
        }else {
            diffuseColor = color * colorOnHit * material.kd * diffuseFactor;
        }

        return diffuseColor;

    }

    [[nodiscard]] virtual Color computeDiffuseGoochAt(const Hit& hit_point, const Material& material, const Color& colorOnHit) const {
        // source (adapted): https://rendermeapangolin.wordpress.com/2015/05/07/gooch-shading/
        float a = 0.3;
        float b = 0.6;

        Vector lightIncidence = Position - hit_point.Position;
        lightIncidence.normalize();
        double diffuseFactor = hit_point.Normal.dot(lightIncidence);
        diffuseFactor = (diffuseFactor + 1) / 2;
        Color diffuseColor = (1 - diffuseFactor) * (Color{0, 0, a})
                        + diffuseFactor * b * (color * colorOnHit * material.kd);

        // Highlights
        Vector ray_reflection = -rotateAround(hit_point.Source.Direction, hit_point.Normal, 180);
        float ER = std::clamp(lightIncidence.dot(ray_reflection.normalized()), 0.0, 1.0);

        Color specular = color * std::pow(ER, material.n);

        return diffuseColor; // + specular;
    }

    [[nodiscard]] virtual Color computeSpecularGoochAt(const Hit& hit_point, const Material& material, const Color& colorOnHit) const {
        Vector ray_reflection = -rotateAround(hit_point.Source.Direction, hit_point.Normal, 180);
        ray_reflection.normalize();
        Vector lightIncidence = Position - hit_point.Position;
        lightIncidence.normalize();

        double specularFactor = ray_reflection.dot(lightIncidence);
        if (specularFactor < 0) {
            specularFactor = 0;
        }
        Color specularColor = color * material.ks * pow(specularFactor,material.n);

        return specularColor;
    }

    Point Position;
    Color color;
    float Size;
};

#endif /* end of include guard: LIGHT_H_PG2BAJRA */

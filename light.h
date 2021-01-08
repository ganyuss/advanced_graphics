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
#include "triple.h"
#include "material.h"



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
    return h1.Distance == h2.Distance
           && h1.Position == h2.Position
           && h1.Normal == h2.Normal
           && h1.Source == h2.Source;
}


class Light
{
public:
    Light(Point Position, Color c) : Position(Position), color(c)
    { }

    virtual Color computeColorAt(const Hit& hit_point, const Material& material) const {
        Vector ray_reflection = hit_point.Source.Direction
                - 2 * hit_point.Source.Direction.dot(hit_point.Normal.normalized())
                        * hit_point.Normal;
        ray_reflection.normalize();
        Vector lightIncidence = Position - hit_point.Position;
        lightIncidence.normalize();
        double diffuseFactor = hit_point.Normal.dot(lightIncidence);
        Color diffuseColor{};
        if (diffuseFactor <= 0){
            diffuseColor.set(0,0,0);
        }else {
            diffuseColor = color * material.color * material.kd * diffuseFactor;
        }

        double specularFactor = ray_reflection.dot(lightIncidence);
        if (specularFactor < 0){
            specularFactor = 0;
        }
        Color specularColor = color * material.ks * pow(specularFactor,material.n);

        return diffuseColor + specularColor;

    }

    Point Position;
    Color color;
};

#endif /* end of include guard: LIGHT_H_PG2BAJRA */

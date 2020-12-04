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

class Light
{
public:
    Light(Point pos,Color c) : position(pos), color(c)
    { }

    Point position;
    Color color;
};

class Ray
{
public:
    Point Origin;
    Vector Direction;

    Ray(const Point &from, const Vector &dir)
        : Origin(from), Direction(dir)
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

#endif /* end of include guard: LIGHT_H_PG2BAJRA */

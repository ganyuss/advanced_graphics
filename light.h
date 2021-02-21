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
#include "scene.h"
#include "material.h"
#include "triple.h"
#include "commongeometry.h"


struct GoochIlluminationModel;

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
            : Distance(Distance), Position(Position), Normal(Normal.normalized()), Source(Source)
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

    [[nodiscard]] virtual Color computeSpecularPhongAt(const Hit& hit_point, const Material& material, double specularValueOnHit) const {
        Vector ray_reflection = -rotateAround(hit_point.Source.Direction, hit_point.Normal, 180);
        ray_reflection.normalize();
        Vector lightIncidence = Position - hit_point.Position;
        lightIncidence.normalize();

        double specularFactor = ray_reflection.dot(lightIncidence);
        if (specularFactor < 0){
            specularFactor = 0;
        }
        Color specularColor = color * specularValueOnHit * pow(specularFactor,material.n);

        return specularColor;

    }

    [[nodiscard]] virtual Color computeDiffusePhongAt(const Hit&, const Material&, const Color& colorOnHit) const;
    [[nodiscard]] virtual Color computeDiffuseGoochAt(const Hit&, const Material&, const Color& colorOnHit, GoochIlluminationModel illuminationModel) const;
    [[nodiscard]] virtual Color computeSpecularGoochAt(const Hit&, const Material&, const Color& colorOnHit, double specularValueOnHit) const;

    Point Position;
    Color color;
    float Size;
};


namespace std {
    template<>
    struct hash<Light> {
        std::size_t operator()(Light const &light) const noexcept {
            return hash_combine<double>(154945235,
                                        hash<Vector>{}(light.Position),
                                        hash<Color>{}(light.color),
                                        light.Size
                                        );
        }
    };
}


#endif /* end of include guard: LIGHT_H_PG2BAJRA */

//
//  Framework for a raytracer
//  File: object.h
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

#ifndef OBJECT_H_AXKLE0OF
#define OBJECT_H_AXKLE0OF

#include "triple.h"
#include "material.h"
#include <array>
#include "Quaternion.h"

class Hit;
class Ray;

class Object {
public:
    Material material;

    virtual Hit intersect(const Ray &ray) = 0;
    virtual std::array<double, 2> getTextureCoordinatesFor(Point) = 0;

    inline Color getColorOnPosition(const Point& position) {
        if (material.isTextured) {
            std::array<double, 2> uv = getTextureCoordinatesFor(position);
            return material.texture.colorAt(uv[0], uv[1]);
        }
        else {
            return material.color;
        }
    }

    virtual ~Object() = default;
};

#endif /* end of include guard: OBJECT_H_AXKLE0OF */

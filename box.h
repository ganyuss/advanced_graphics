//
// Created by Enzo on 05/01/2021.
//

#ifndef QUAD_H_115209AE
#define QUAD_H_115209AE

#include "object.h"

class Box : public Object {
public:
    Box(Point Position, float width, float depth, float height) : Position(Position), width(width), depth(depth), height(height)  { }

    Hit intersect(const Ray &ray) const override;

    const Point Position;
    const float width;
    const float depth;
    const float height;
};


#endif

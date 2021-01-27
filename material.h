//
//  Framework for a raytracer
//  File: material.h
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Author: Maarten Everts
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#ifndef MATERIAL_H_TWMNT2EJ
#define MATERIAL_H_TWMNT2EJ

#include <iostream>
#include "triple.h"
#include "yaml/node.h"
#include "image.h"

enum MaterialType { DEFAULT, REFLECTION, REFRACTION };

struct Material
{

    Color color;
    std::optional<Image> texture;
    std::optional<Image> specularMap;


    double ka;          // ambient intensity
    double kd;          // diffuse intensity
    double ks;          // specular intensity 
    double n;           // exponent for specular highlight size
    double index;
    MaterialType type;

    Material() : ka{}, kd{}, ks{}, n{}, type{}
    { }
};

void operator>>(const YAML::Node &node, MaterialType &mode);


#endif /* end of include guard: MATERIAL_H_TWMNT2EJ */

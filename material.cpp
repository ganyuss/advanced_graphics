//
//  Framework for a raytracer
//  File: material.cpp
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

#include "material.h"

std::size_t CustomLightHash::operator()(Light const &light) const noexcept {
    return hash_combine<double>(154945235,
                                std::hash<Vector>{}(light.Position),
                                std::hash<Color>{}(light.color),
                                light.Size
                                );
}

void operator>>(const YAML::Node &node, MaterialType &type) {
    auto s = node.Read<std::string>();
    std::map<std::string, MaterialType> map{
        {"default", MaterialType::DEFAULT},
        {"reflection", MaterialType::REFLECTION},
        {"refraction", MaterialType::REFRACTION}
    };
    type = map[s];
}
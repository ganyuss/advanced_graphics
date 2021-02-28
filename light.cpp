//
//  Framework for a raytracer
//  File: light.cpp
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

#include "light.h"
#include "scene.h"


Color Light::computeDiffusePhongAt(const Hit& hit_point, const Material& material, const Color& colorOnHit) const {

    Vector ray_reflection = -rotateAround(hit_point.Source.Direction, hit_point.Normal, 180);
    ray_reflection.normalize();
    Vector lightIncidence = Position - hit_point.Position;
    lightIncidence.normalize();
    double diffuseFactor = hit_point.Normal.dot(lightIncidence);
    Color diffuseColor{};
    if (diffuseFactor <= 0) {
        diffuseColor.set(0,0,0);
    }else {
        diffuseColor = color * colorOnHit * material.kd * diffuseFactor;
    }

    return diffuseColor;
}

Color Light::computeDiffuseGoochAt(const Hit& hit_point, const Material& material, const Color& colorOnHit, GoochIlluminationModel illuminationModel) const {
    // source: http://artis.imag.fr/~Cyril.Soler/DEA/NonPhotoRealisticRendering/Papers/p447-gooch.pdf

    Color kCool = Color{0, 0, illuminationModel.b} + illuminationModel.alpha * material.kd * colorOnHit;
    Color kWarm = Color{illuminationModel.y, illuminationModel.y, 0} + illuminationModel.beta * material.kd * colorOnHit;

    Vector lightIncidence = Position - hit_point.Position;
    lightIncidence.normalize();
    double diffuseFactor = hit_point.Normal.dot(lightIncidence);
    diffuseFactor = (diffuseFactor + 1) / 2;
    Color diffuseColor = (1 - diffuseFactor) * (kCool)
                         + diffuseFactor * kWarm;

    // Highlights
    Vector ray_reflection = -rotateAround(hit_point.Source.Direction, hit_point.Normal, 180);
    float ER = std::clamp(lightIncidence.dot(ray_reflection.normalized()), 0.0, 1.0);

    Color specular = color * std::pow(ER, material.n);

    return diffuseColor; // + specular;
}

Color Light::computeSpecularGoochAt(const Hit& hit_point, const Material& material, const Color& colorOnHit, double specularValueOnHit) const {
    Vector ray_reflection = -rotateAround(hit_point.Source.Direction, hit_point.Normal, 180);
    ray_reflection.normalize();
    Vector lightIncidence = Position - hit_point.Position;
    lightIncidence.normalize();

    double specularFactor = ray_reflection.dot(lightIncidence);
    if (specularFactor < 0) {
        specularFactor = 0;
    }
    Color specularColor = color * specularValueOnHit * pow(specularFactor,material.n);

    return specularColor;
}

Color Light::computeSpecularPhongAt(const Hit &hit_point, const Material &material, double specularValueOnHit) const {
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

bool operator==(const Light &l1, const Light &l2) {
    return l1.Position == l2.Position
            && l1.color == l2.color
            && l1.Size == l2.Size;
}
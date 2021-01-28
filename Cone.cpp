//
// Created by cleme on 06/01/2021.
//

#include "Cone.h"
#include "commongeometry.h"
#include <cmath>

Hit Cone::intersect(const Ray &ray) {

    Hit slopeHit = getHitOnSlope(ray);

    Hit planHit = DiskPlan.intersect(ray);

    if ((planHit.Position - Position).norm() < Radius) {

        double hitDistance = (planHit.Position - ray.Origin).norm();
        if (hitDistance > slopeHit.Distance)
            return slopeHit;

        // applying normal map
        planHit.Normal = applyNormalMap(planHit.Position, planHit.Normal, Side);
        return planHit;
    }

    return slopeHit;
}

Vector Cone::getNormalAt(const Point &p) {

    Point Tip = (Position + Up);
    Vector NormalDirection = rotateAround(p - Tip, getThirdOrthogonalVector(Up, p - Tip), 90)
            .normalized();

    Vector normalUp = Plane{p, NormalDirection}.projectOn(Vector{0, 1, 0});
    Vector normal = applyNormalMap(p, NormalDirection, normalUp);
    return normal;
}

bool Cone::isInShadowCone(const Point &p) {

    Vector baseToP = p - Position;
    Vector TipToP = p - (Position + Up);

    return baseToP.dot(Up) < 0
            || TipToP.dot(-Up) < 0;
}

Hit Cone::getHitOnSlope(const Ray& ray) {
    // source: http://lousodrome.net/blog/light/2017/01/03/intersection-of-a-ray-and-a-cone/
    // Could be optimized, especially with the computation that is not function of the ray
    Vector co = Vector(ray.Origin - (Position + Up));

    double a = std::pow(ray.Direction.dot(v), 2) - cosSquaredTheta;
    double b = 2 * (ray.Direction.dot(v) * co.dot(v) - ray.Direction.dot(co)*cosSquaredTheta);
    double c = std::pow(co.dot(v), 2) - co.dot(co)*cosSquaredTheta;

    double delta = b*b -4*a*c;

    if (delta < 0) {
        return Hit::NO_HIT();
    }
    else if (delta == 0) {
        double t = -b / (2*a);

        if (t < 0)
            return Hit::NO_HIT();

        return Hit(
                t * ray.Direction.norm(),
                ray.at(t),
                getNormalAt(ray.Origin + t * ray.Direction),
                ray
        );
    }
    else {
        double t1 = (-b + std::sqrt(delta)) / (2*a);
        double t2 = (-b - std::sqrt(delta)) / (2*a);
        Point p1 = ray.Origin + t1 * ray.Direction;
        Point p2 = ray.Origin + t2 * ray.Direction;


        bool t1IsInvalid = isInShadowCone(p1) || t1 <= 0;
        bool t2IsInvalid = isInShadowCone(p2) || t2 <= 0;

        if (t1IsInvalid && t2IsInvalid)
            return Hit::NO_HIT();

        Hit hit1{
                t1 * ray.Direction.norm(),
                ray.at(t1),
                getNormalAt(ray.Origin + t1 * ray.Direction),
                ray
        };

        Hit hit2{
                t2 * ray.Direction.norm(),
                ray.at(t2),
                getNormalAt(ray.Origin + t2 * ray.Direction),
                ray
        };



        if (t1IsInvalid)
            return hit2;
        else if (t2IsInvalid)
            return hit1;
        else
            return (hit1.Distance < hit2.Distance) ? hit1 : hit2;
    }
}

std::array<double, 2> Cone::getTextureCoordinatesFor(const Point &point) {

    if (isOnDisk(point)) {
        static Vector sideDirection = getThirdOrthogonalVector(Side, Up).normalized();

        Vector centerToPoint = point - Position;
        Vector upComponent = project(centerToPoint, Side);
        Vector sideComponent = centerToPoint - upComponent;
        double upValue = upComponent.norm() / Radius;
        double sideValue = sideComponent.norm() / Radius;

        if (sideDirection.dot(sideComponent) < 0)
            sideValue = - sideValue;
        if (Side.dot(upComponent) < 0)
            upValue = - upValue;

        return {(sideValue+1)/2, (upValue+1)/4 };
    }
    else {
        constexpr double twoPi = M_PI * 2;
        constexpr double oneOverTwoPi = 1 / twoPi;

        double Y = project(point, Up).norm() / Up.norm();

        Point pointOnDisk = DiskPlan.projectOn(point);
        pointOnDisk.normalize();
        Vector pointOnDiskXComponent = project(pointOnDisk, Side);
        Vector pointOnDiskZComponent = pointOnDisk - pointOnDiskXComponent;

        static Vector pointOnDiskZDirection = getThirdOrthogonalVector(Side, Up).normalized();

        double pointTheta;

        if (pointOnDiskZDirection.dot(pointOnDiskZComponent) > 0)
            pointTheta = twoPi - std::acos(pointOnDiskXComponent.norm());
        else
            pointTheta = std::acos(pointOnDiskXComponent.norm());

        double X = pointTheta * oneOverTwoPi;
        return {X, Y / 2};
    }
}

bool Cone::isOnDisk(const Point& point) {
    Vector centerToPoint = point - Position;
    return DiskPlan.projectOn(centerToPoint) == centerToPoint;
}

//
// Created by Enzo on 05/01/2021.
//

#include "box.h"

Hit Box::intersect(const Ray &ray){

    bool x = false;
    if(ray.Origin.X() < Position.X()){
        if(ray.Direction.X() > 0)
            x = true;
    }

    if(ray.Origin.X() > Position.X() + width){
        if(ray.Direction.X() < 0)
            x = true;
    }

    bool y = false;
    if(ray.Origin.Y() < Position.Y()){
        if(ray.Direction.Y() > 0)
            y = true;
    }

    if(ray.Origin.Y() > Position.Y() + height){
        if(ray.Direction.Y() < 0)
            y = true;
    }

    bool z = false;
    if(ray.Origin.Z() < Position.Z()){
        if(ray.Direction.Z() > 0)
            z = true;
    }

    if(ray.Origin.Z() > Position.Z() + depth){
        if(ray.Direction.Z() < 0)
            z = true;
    }

    if(x && y && z){

    }
    else {
        return Hit::NO_HIT();
    }

}

//
//  Framework for a raytracer
//  File: raytracer.cpp
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

#include "raytracer.h"
#include "object.h"
#include "sphere.h"
#include "material.h"
#include "light.h"
#include "image.h"
#include "yaml/yaml.h"
#include "Cone.h"
#include <ctype.h>
#include <fstream>
#include <assert.h>

// Functions to ease reading from YAML input
template <typename Triple, class = typename std::enable_if<is_triple_v<Triple>, bool>::type>
void operator >> (const YAML::Node& node, Triple& t);
template <typename Triple, class = typename std::enable_if<is_triple_v<Triple>, bool>::type>
Triple parseTriple(const YAML::Node& node);

void operator >> (const YAML::Node& node, Color& t)
{
    assert(node.size()==3);
    node[0] >> t.Red();
    node[1] >> t.Green();
    node[2] >> t.Blue();
}
void operator >> (const YAML::Node& node, Vector& t)
{
    assert(node.size()==3);
    node[0] >> t.X();
    node[1] >> t.Y();
    node[2] >> t.Z();
}

Material Raytracer::parseMaterial(const YAML::Node& node)
{
    Material m;
    node["color"] >> m.color;
    node["ka"] >> m.ka;
    node["kd"] >> m.kd;
    node["ks"] >> m.ks;
    node["n"] >> m.n;
    node["index"] >> m.index;
    node["type"] >> m.type;
    return m;
}

std::unique_ptr<Object> Raytracer::parseObject(const YAML::Node& node)
{
    std::unique_ptr<Object> returnObject;
    std::string objectType;
    node["type"] >> objectType;

    if (objectType == "sphere") {
        Point pos{};
        node["position"] >> pos;
        double r;
        node["radius"] >> r;
        returnObject = std::make_unique<Sphere>(pos, r);
    }
    else if (objectType == "cone") {
        Point pos{};
        node["position"] >> pos;
        double r;
        node["radius"] >> r;
        Vector up{};
        node["up"] >> up;
        returnObject = std::make_unique<Cone>(pos, r, up);
    }
    else if (objectType == "plane") {
        Point pos{};
        node["position"] >> pos;
        Vector norm{};
        node["normal"] >> norm;
        returnObject = std::make_unique<Plane>(pos, norm);
    }

    if (returnObject) {
        // read the material and attach to object
        returnObject->material = parseMaterial(node["material"]);
    }

    return returnObject;
}

std::unique_ptr<Light> Raytracer::parseLight(const YAML::Node& node)
{
    Point position;
    node["position"] >> position;
    Color color;
    node["color"] >> color;
    float size = 0;
    node["size"] >> size;

    return std::make_unique<Light>(position, color, size);
}

/*
* Read a scene from file
*/

bool Raytracer::readScene(const std::string& inputFilename)
{
    // Open file stream for reading and have the YAML module parse it
    std::ifstream fin(inputFilename.c_str());
    if (!fin) {
        std::cerr << "Error: unable to open " << inputFilename << " for reading." << std::endl;
        return false;
    }
    try {
        YAML::Parser parser(fin);
        if (parser) {
            YAML::Node doc;
            parser.GetNextDocument(doc);

            // Read scene configuration options
            Mode mode;
            doc["RenderMode"] >> mode;
            scene.setMode(mode);
            int maxIterations;
            doc["MaxIterations"] >> maxIterations;
            scene.setMaxIterations(maxIterations);

            int distmin, distmax;
            doc["DistMin"] >> distmin;
            doc["DistMax"] >> distmax;
            scene.setNear(distmin);
            scene.setFar(distmax);

            bool renderSoftShadows = true;
            doc["SoftShadows"] >> renderSoftShadows;
            scene.SoftShadows = renderSoftShadows;

            Camera camera{};
            try {
                doc["Camera"] >> camera;
                scene.camera = camera;
            }
            catch (YAML::Exception e) {
                Camera cameraDefault{};
                doc["Eye"] >> cameraDefault.Eye;
                scene.camera = cameraDefault;
            }

            doc["SuperSampling"]["factor"] >> scene.superSamplingFactor;


            // Read and parse the scene objects
            const YAML::Node& sceneObjects = doc["Objects"];
            if (sceneObjects.GetType() != YAML::CT_SEQUENCE) {
                std::cerr << "Error: expected a sequence of objects." << std::endl;
                return false;
            }
            for(YAML::Iterator it=sceneObjects.begin();it!=sceneObjects.end();++it) {
                std::unique_ptr<Object> obj = parseObject(*it);
                // Only add object if it is recognized
                if (obj) {
                    scene.addObject(std::move(obj));
                } else {
                    std::cerr << "Warning: found object of unknown type, ignored." << std::endl;
                }
            }

            // Read and parse light definitions
            const YAML::Node& sceneLights = doc["Lights"];
            if (sceneObjects.GetType() != YAML::CT_SEQUENCE) {
                std::cerr << "Error: expected a sequence of lights." << std::endl;
                return false;
            }
            for(YAML::Iterator it=sceneLights.begin(); it!=sceneLights.end(); ++it) {
                scene.addLight(parseLight(*it));
            }
        }
        if (parser) {
            std::cerr << "Warning: unexpected YAML document, ignored." << std::endl;
        }
    } catch(YAML::ParserException& e) {
        std::cerr << "Error at line " << e.mark.line + 1 << ", col " << e.mark.column + 1 << ": " << e.msg << std::endl;
        return false;
    }

    std::cout << "YAML parsing results: " << scene.getNumObjects() << " objects read." << std::endl;
    return true;
}

void Raytracer::renderToFile(const std::string& outputFilename)
{
    Image img(scene.camera.ViewSize[0],scene.camera.ViewSize[1]);
    std::cout << "Tracing..." << std::endl;
    scene.render(img);
    std::cout << "Writing image to " << outputFilename << "..." << std::endl;
    img.write_png(outputFilename.c_str());
    std::cout << "Done." << std::endl;
}

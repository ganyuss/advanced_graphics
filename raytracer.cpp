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
#include "sphere.h"
#include "Cone.h"
#include "TriangleAggregate.h"
#include "box.h"
#include <fstream>

template <typename VariableType>
bool tryRead(const YAML::Node &node, VariableType &variable, const VariableType& defaultValue = VariableType{}) {
    try{
        node >> variable;
        return true;
    } catch(YAML::Exception &e) {
        variable = defaultValue;
    }

    return false;
}

template <typename VariableType>
bool tryRead(const YAML::Node &node, const std::string& key, VariableType &variable, const VariableType& defaultValue = VariableType{});


template <>
bool tryRead<Mode>(const YAML::Node &node, Mode &variable, const Mode& defaultValue) {
    try {
        auto modeString = node.Read<std::string>();

        std::map<std::string, Mode> map{{"ZBUFFER", Mode::ZBUFFER}, {"PHONG", Mode::PHONG}, {"GOOCH", Mode::GOOCH}, {"NORMAL", Mode::NORMAL}, {"TEXTURE", Mode::TEXTURE}};
        if (map.find(modeString) != map.end())
            variable = map[modeString];
        else
            variable = defaultValue;
    }
    catch(YAML::Exception& e) {
        variable = defaultValue;
    }

    return true;
}

template <>
bool tryRead<RefractedShadowsParameters>(const YAML::Node &node, RefractedShadowsParameters &variable, const RefractedShadowsParameters& defaultValue) {

    double precisionFactor;

    bool isOk1 = tryRead(node, "TextureSize", variable.textureSize, defaultValue.textureSize);
    bool isOk2 = tryRead(node, "SmoothingFactor", variable.smoothingFactor, defaultValue.smoothingFactor);
    bool isOk3 = tryRead(node, "PrecisionFactor", precisionFactor, 1 / defaultValue.precision);
    bool isOk4 = tryRead(node, "IntensityFactor", variable.intensityFactor, defaultValue.intensityFactor);

    variable.precision = 1 / precisionFactor;

    return isOk1 || isOk2 || isOk3 || isOk4;
}

template <>
bool tryRead<Vector>(const YAML::Node &node, Vector &variable, const Vector& defaultValue) {

    if (node.size() != 3) {
        variable = defaultValue;
        return false;
    }

    try {
        node[0] >> variable.X();
        node[1] >> variable.Y();
        node[2] >> variable.Z();
    }
    catch (YAML::Exception & e) {
        variable = defaultValue;
        return false;
    }

    return true;
}

template <>
bool tryRead<Color>(const YAML::Node &node, Color &variable, const Color& defaultValue) {

    if (node.size() != 3) {
        variable = defaultValue;
        return false;
    }

    try {
        node[0] >> variable.Red();
        node[1] >> variable.Green();
        node[2] >> variable.Blue();
    }
    catch (YAML::Exception & e) {
        variable = defaultValue;
        return false;
    }

    return true;
}

template <>
bool tryRead<std::array<unsigned int, 2>>(const YAML::Node &node, std::array<unsigned int, 2>& variable, std::array<unsigned int, 2>const& defaultValue) {

    bool everythingOK = node.size() != 2;

    if (everythingOK) {
        try {
            for (std::size_t i = 0; i < 2; ++i)
                node[i] >> variable[i];
        }
        catch (YAML::Exception &e) {
            everythingOK = false;
        }
    }

    if (! everythingOK) {
        variable = defaultValue;
    }

    return everythingOK;
}

template <>
bool tryRead<Camera>(const YAML::Node &node, Camera& variable, const Camera& defaultValue) {

    Point eye{}, center{};
    Vector up{};
    std::array<unsigned int, 2> ViewSize{};

    bool everythingOK = tryRead(node, "eye", eye);

    tryRead(node, "center", center, {200,200,-200});
    tryRead(node, "up", up, Vector{0, 1, 0});
    tryRead(node, "viewSize", ViewSize, {400, 400});

    if (! everythingOK) {
        variable = defaultValue;
    }
    else {
        variable = Camera{eye, center, up};
        variable.ViewSize = ViewSize;
    }

    return everythingOK;
}

template <>
bool tryRead<Material>(const YAML::Node &node, Material &variable, const Material& defaultValue) {
    bool everythingOK = true;

    std::string texture;
    std::string specularMap;
    std::string normalMap;
    if (tryRead(node, "texture", texture)) {
        variable.texture = Image(texture.c_str());
        if (variable.texture.value().size() == 0)
            throw std::runtime_error("File not found: " + texture);
    }
    else {
        everythingOK = tryRead(node, "color", variable.color, defaultValue.color);
    }

    if (tryRead(node, "specularMap", specularMap)) {
        variable.specularMap = Image(specularMap.c_str());
        if (variable.specularMap.value().size() == 0)
            throw std::runtime_error("File not found: " + texture);
    }
    else {
        everythingOK = everythingOK && tryRead(node, "ks", variable.ks, defaultValue.ks);
    }

    if (tryRead(node, "normalMap", normalMap)) {
        variable.normalMap = Image(normalMap.c_str());

    }


    everythingOK = everythingOK
            && tryRead(node, "ka", variable.ka, defaultValue.ka)
            && tryRead(node, "kd", variable.kd, defaultValue.kd)
            && tryRead(node, "n", variable.n, defaultValue.n);

    tryRead(node, "index", variable.index, 1.);
    tryRead(node, "type", variable.type, MaterialType::DEFAULT);

    if (! everythingOK)
        variable = defaultValue;

    return everythingOK;
}

template <>
bool tryRead<std::unique_ptr<Object>>(const YAML::Node &node, std::unique_ptr<Object>& variable, const std::unique_ptr<Object>& defaultValueUnused) {
    std::string objectType;
    tryRead(node, "type", objectType);
    bool everythingOK = false;

    if (objectType == "sphere") {
        Point pos{};
        double r;
        Quaternion quaternion;

        everythingOK = tryRead(node, "position", pos)
            && tryRead(node, "radius", r);

        tryRead(node, "quaternion", quaternion, Quaternion(0,0,0,1));

        if (everythingOK)
            variable = std::make_unique<Sphere>(pos, r, quaternion);
    }
    else if (objectType == "cone") {
        Point pos{};
        Vector side{};
        double radius;
        Vector up{};

        everythingOK = tryRead(node, "position", pos)
            && tryRead(node, "up", up);

        if (everythingOK && ! tryRead(node, "side", side))
            if (tryRead(node, "radius", radius))
                side = getAnyOrthogonalVector(up).normalized() * radius;

        if (everythingOK)
            variable = std::make_unique<Cone>(pos, side, up);
    }
    else if (objectType == "plane") {
        Point pos{};
        Vector norm{};
        double scale;

        everythingOK = tryRead(node, "position", pos)
            && tryRead(node, "normal", norm);

        tryRead(node, "UV_scale", scale, 0.01);

        if (everythingOK)
            variable = std::make_unique<Plane>(pos, norm, scale);
    }
    else if (objectType == "triangle") {
        Point p1{}, p2{}, p3{};
        std::array<double, 2> UV1{}, UV2{}, UV3{};

        everythingOK = tryRead(node, "corner1", p1)
                && tryRead(node, "corner2", p2)
                && tryRead(node, "corner3", p3);


        tryRead(node, "UV1", UV1, {0, 0});
        tryRead(node, "UV2", UV2, {1, 0});
        tryRead(node, "UV3", UV3, {0, 1});

        Vector normal = getThirdOrthogonalVector(p2 - p1, p3 - p1);


        if (everythingOK)
            variable = std::make_unique<Triangle>(
                    Vertex{p1, normal, UV1},
                    Vertex{p2, normal, UV2},
                    Vertex{p3, normal, UV3}
                    );
    }
    else if (objectType == "triangleAggregate") {

        std::string fileName;

        everythingOK = tryRead(node, "fileName", fileName);
        if (everythingOK)
            variable = std::make_unique<TriangleAggregate>(fileName);
    }
    else if (objectType == "quadrilateral") {

        Point position{};
        Vector up{}, side{};

        everythingOK = tryRead(node, "position", position)
                       && tryRead(node, "up", up)
                       && tryRead(node, "side", side);

        if (everythingOK)
            variable = std::make_unique<Quadrilateral>(position, up, side);
    }
    else if (objectType == "box") {

        Point position{};
        Vector up{}, side{};
        double depth;

        everythingOK = tryRead(node, "position", position)
                       && tryRead(node, "up", up)
                       && tryRead(node, "side", side)
                       && tryRead(node, "depth", depth);

        if (everythingOK)
            variable = std::make_unique<Box>(position, up, side, depth);
    }


    if (variable && everythingOK) {
        // read the material and attach to object
        everythingOK = tryRead(node, "material", variable->material);
    }

    return everythingOK;
}

template<>
bool tryRead<std::array<double, 2>>(const YAML::Node &node, std::array<double, 2>& variable, const std::array<double, 2>& defaultValue){
    if (node.size() != 2) {
        variable = defaultValue;
        return false;
    }

    try {
        node[0] >> variable[0];
        node[1] >> variable[1];
    }
    catch (YAML::Exception & e) {
        variable = defaultValue;
        return false;
    }

    return true;
}

template<>
bool tryRead<Quaternion>(const YAML::Node &node, Quaternion& variable, const Quaternion& defaultValue){
    if (node.size() != 4) {
        variable = defaultValue;
        return false;
    }

    try {
        node[0] >> variable.x;
        node[1] >> variable.y;
        node[2] >> variable.z;
        node[3] >> variable.w;
    }
    catch (YAML::Exception & e) {
        variable = defaultValue;
        return false;
    }

    return true;
}
template <>
bool tryRead<std::unique_ptr<Light>>(const YAML::Node &node, std::unique_ptr<Light>& variable, const std::unique_ptr<Light>& defaultValueUnused)
{
    bool everythingOK;

    Point position;
    Color color{};
    float size;

    everythingOK = tryRead(node, "position", position);
    tryRead(node, "color", color, Color{1, 1, 1});
    tryRead(node, "size", size, 200.f);

    if (everythingOK)
        variable = std::make_unique<Light>(position, color, size);

    return everythingOK;
}

template <>
bool tryRead<GoochIlluminationModel>(const YAML::Node &node, GoochIlluminationModel& variable, const GoochIlluminationModel& defaultValue) {

    bool readB = tryRead(node, "b", variable.b, defaultValue.b);
    bool readY = tryRead(node, "y", variable.y, defaultValue.y);
    bool readAlpha = tryRead(node, "alpha", variable.alpha, defaultValue.alpha);
    bool readBeta = tryRead(node, "beta", variable.beta, defaultValue.beta);

    return readB && readY && readAlpha && readBeta;
}

template <typename VariableType>
bool tryRead(const YAML::Node &node, const std::string& key, VariableType &variable, const VariableType& defaultValue)
{
    try{
        node[key];
    } catch(YAML::Exception &e) {
        variable = defaultValue;
        return false;
    }

    return tryRead(node[key], variable, defaultValue);
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
            int maxIterations;
            int distmin, distmax;
            bool renderSoftShadows;

            tryRead(doc, "RenderMode", mode, Mode::PHONG);
            tryRead(doc, "MaxIterations", maxIterations, 0);
            tryRead(doc, "DistMin", distmin, 0);
            tryRead(doc, "DistMax", distmax, 10000);
            tryRead(doc, "SoftShadows", renderSoftShadows, false);

            scene.setMode(mode);
            scene.setMaxIterations(maxIterations);
            scene.setNear(distmin);
            scene.setFar(distmax);
            scene.SoftShadows = renderSoftShadows;

            if (! tryRead(doc, "GoochParameters", scene.goochIlluminationModel) && mode == Mode::GOOCH) {
                std::cerr << "Warning: problem reading the gooch model parameters, using the default values" << std::endl;
            }

            Camera camera{};

            if (tryRead(doc, "Camera", camera)) {
                scene.camera = camera;
            }
            else {
                Vector eye{};
                tryRead(doc, "Eye", eye, Vector{200, 200, 0});
                scene.camera = Camera{eye};
            }

            try {
                const YAML::Node& n = doc["SuperSampling"];
                tryRead<unsigned int>(n, "factor", scene.superSamplingFactor, 1);
            }
            catch (YAML::Exception & e) {
                scene.superSamplingFactor = 1;
            }

            try {
                const YAML::Node& n = doc["SoftShadowsPrecision"];
                tryRead<unsigned int>(n, "edge", scene.shadowEdgePrecision, 2);
                tryRead<unsigned int>(n, "shade", scene.shadowShadePrecision, 3);
            }
            catch (YAML::Exception & e) {
                scene.shadowEdgePrecision = 2;
                scene.shadowShadePrecision = 3;
            }

            bool shadowRefraction = false, refractedShadowsDefined = false;
            RefractedShadowsParameters refractedShadowsParameters;

            if (! tryRead(doc, "ShadowRefraction", shadowRefraction, false) || shadowRefraction) {
                refractedShadowsDefined = tryRead(doc, "RefractedShadows", refractedShadowsParameters);


                if (shadowRefraction || refractedShadowsDefined) {
                    scene.refractedShadows = refractedShadowsParameters;
                }
            }

            // Read and parse the scene objects
            const YAML::Node& sceneObjects = doc["Objects"];
            if (sceneObjects.GetType() != YAML::CT_SEQUENCE) {
                std::cerr << "Error: expected a sequence of objects." << std::endl;
                return false;
            }
            for(YAML::Iterator it = sceneObjects.begin(); it != sceneObjects.end(); ++it) {
                std::unique_ptr<Object> object;

                // Only add object if it is recognized
                if (tryRead(*it, object)) {
                    scene.addObject(std::move(object));
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
                std::unique_ptr<Light> light;

                // Only add object if it is recognized
                if (tryRead(*it, light)) {
                    scene.addLight(std::move(light));
                } else {
                    std::cerr << "Warning: found unreadable light, ignored." << std::endl;
                }
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
    std::cout << "Tracing..." << std::endl;
    Image img = scene.render();
    std::cout << "Writing image to " << outputFilename << "..." << std::endl;
    img.write_png(outputFilename.c_str());
    std::cout << "Done." << std::endl;
}

//
//  Framework for a raytracer
//  File: image.cpp
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

#include "image.h"
#include "lodepng.h"
#include <fstream>

/*
* Create a picture. Answer false if failed.
*/
bool Image::set_extent(int width, int height)
{
    _width = width;
    _height = height;
    delete[] _pixel;
    _pixel = size() > 0 ? new Color[size()] : nullptr;
    return _pixel != nullptr;
}


void Image::write_png(const char* filename) const
{
    std::vector<unsigned char> image;
    image.resize(_width * _height * 4);
    auto imageIterator = image.begin();
    Color *currentPixel = _pixel;
    while (imageIterator != image.end()) {
        *imageIterator = (unsigned char)(currentPixel->r * 255.0);
        imageIterator++;
        *imageIterator = (unsigned char)(currentPixel->g * 255.0);
        imageIterator++;
        *imageIterator = (unsigned char)(currentPixel->b * 255.0);
        imageIterator++;
        *imageIterator = 255;
        imageIterator++;
        currentPixel++;
    }
    LodePNG::encode(filename, image, _width, _height);
}


void Image::read_png(const char* filename)
{
    std::vector<unsigned char> buffer, image;
    //load the image file with given filename
    LodePNG::loadFile(buffer, filename);

    //decode the png
    LodePNG::Decoder decoder;
    decoder.decode(image, buffer.empty() ? nullptr : &buffer[0], (unsigned)buffer.size());
    std::cout << decoder.getChannels() << std::endl;
    std::cout << decoder.getBpp() << std::endl;

    if (decoder.getChannels()<3 || decoder.getBpp()<24) {
        std::cerr << "Error: only color (RGBA), 8 bit per channel png images are supported." << std::endl;
        std::cerr << "Either convert your image or change the sourcecode." << std::endl;
        exit(1);
    }
    int w = static_cast<int>(decoder.getWidth());
    int h = static_cast<int>(decoder.getHeight());
    set_extent(w,h);

    // now convert the image data
    auto imageIterator = image.begin();
    Color *currentPixel = _pixel;
    while (imageIterator != image.end()) {
        currentPixel->r = (*imageIterator)/255.0;
        imageIterator++;
        currentPixel->g = (*imageIterator)/255.0;
        imageIterator++;
        currentPixel->b = (*imageIterator)/255.0;
        imageIterator++;
        // Let's just ignore the alpha channel
        imageIterator++; 
        currentPixel++;
    }	
}

//
//  Framework for a raytracer
//  File: image.h
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

#ifndef IMAGE_H_IOLFQARK
#define IMAGE_H_IOLFQARK

#include "baseimage.h"

#include <iostream>
#include <cstring>
#include <vector>
#include "triple.h"
#include "lodepng.h"


class Image : public BaseImage<Color>
{
public:
    Image()
        : BaseImage()
    { }

    Image(int width, int height)
            : BaseImage(width, height)
    { }

    Image(const char *imageFilename)
        : BaseImage()
    {
        read_png(imageFilename);
    }

    void write_png(const char* filename) const {
        std::vector<unsigned char> image;
        image.resize(_width * _height * 4);
        auto imageIterator = image.begin();
        auto currentPixel = _pixel.begin();
        while (imageIterator != image.end()) {
            *imageIterator = (unsigned char)(currentPixel->Red() * 255.0);
            imageIterator++;
            *imageIterator = (unsigned char)(currentPixel->Green() * 255.0);
            imageIterator++;
            *imageIterator = (unsigned char)(currentPixel->Blue() * 255.0);
            imageIterator++;
            *imageIterator = 255;
            imageIterator++;
            currentPixel++;
        }
        LodePNG::encode(filename, image, _width, _height);
    }



    void read_png(const char* filename) {
        std::vector<unsigned char> buffer, image;
        //load the image file with given filename
        LodePNG::loadFile(buffer, filename);

        //decode the png
        LodePNG::Decoder decoder;
        decoder.decode(image, buffer.empty() ? nullptr : &buffer[0], (unsigned)buffer.size());

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
        auto currentPixel = _pixel.begin();
        while (imageIterator != image.end()) {
            currentPixel->Red() = (*imageIterator)/255.0;
            imageIterator++;
            currentPixel->Green() = (*imageIterator)/255.0;
            imageIterator++;
            currentPixel->Blue() = (*imageIterator)/255.0;
            imageIterator++;
            // Let's just ignore the alpha channel
            imageIterator++;
            currentPixel++;
        }
    }

};



#endif /* end of include guard: IMAGE_H_IOLFQARK */

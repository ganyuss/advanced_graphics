//
// Created by cleme on 21/02/2021.
//

#ifndef RAYTRACER_BASEIMAGE_H
#define RAYTRACER_BASEIMAGE_H

template<typename ValueType>
class BaseImage {
protected:
    std::vector<ValueType> _pixel;
    int _width;
    int _height;

public:

    BaseImage() : BaseImage(0, 0)
    { }

    BaseImage(int width, int height)
            : _pixel(), _width(0), _height(0)
    {
        set_extent(width, height);    //creates array
    }



    // Normal accessors
    inline void put_pixel(int x, int y, ValueType c) {
        (*this)(x, y) = c;
    }
    inline ValueType get_pixel(int x, int y) const {
        return (*this)(x, y);
    }

    // Handier accessors
    // Usage: color = img(x,y);
    //        img(x,y) = color;
    inline const ValueType& operator()(int x, int y) const {
        return _pixel[index(x, y)];
    }
    inline ValueType& operator()(int x, int y) {
        return _pixel[index(x, y)];
    }

    // Normalized accessors, interval is (0...1, 0...1)
    inline const ValueType& colorAt(float x, float y) const {
        return _pixel[findex(x, y)];
    }

    // Normalized accessors, interval is (0...1, 0...1)
    inline ValueType& colorAt(float x, float y) {
        return _pixel[findex(x, y)];
    }

    // Image parameters
    inline int width() const    { return _width; }
    inline int height() const   { return _height; }
    inline int size() const     { return _width * _height; }

protected:

    inline int index(int x, int y) const            //integer index
    { return y * _width + x; }

    inline int windex(int x, int y) const           //wrapped integer index
    { return index(x % _width, y % _height); }

    inline int findex(float x, float y) const       //float index
    {
        while (x > 1) x -= 1; while (x < 0) x += 1;
        while (y > 1) y -= 1; while (y < 0) y += 1;

        return index(int(x * (_width-1)), int(y * (_height-1)));
    }

    // Create a picture. Return false if failed.
    void set_extent(int width, int height) {
        _width = width;
        _height = height;
        _pixel.resize(size());
    }
};

#endif //RAYTRACER_BASEIMAGE_H

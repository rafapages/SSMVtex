#ifndef IMAGE_H
#define IMAGE_H

#include <FreeImagePlus.h>
#include <iostream>
#include <Math.h>

#include "Color.h"

typedef enum {BICUBIC, BILINEAR} InterpolateMode;

class Image {

public:

    Image();
    Image(const std::string& _fileName);
    Image(unsigned int _height, unsigned int _width, Color _background = Color(127,127,127,1)); // Images are set to grey if no other color is specified

    // Data access
    Color getColor (unsigned int _row, unsigned int _column) const;
    void setColor (const Color& _color, unsigned int _row, unsigned int _column);
    
    // gets the color of the specified position (_row, _column)
    // by interpolating its value through bicubic interpolation
    Color interpolate (float _row, float _column, InterpolateMode _mode = BICUBIC) const;
    // void replicateRow(unsigned int _from, unsigned int _to);

    unsigned int getWidth () const; // {return width;}
    unsigned int getHeight () const;//  {return height;}

    // I/0
    void save(const std::string& _fileName);

private:

    fipImage imageFile_;
    unsigned int width_, height_;

};

#endif
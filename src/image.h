/* 
 *  Copyright (c) 2015  Rafael Pagés (rps (at) gti.ssr.upm.es)
 *    and Universidad Politécnica de Madrid
 *
 *  This file is part of Multitex
 *
 *  Multitex is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Multitex is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#ifndef IMAGE_H
#define IMAGE_H

#include <FreeImagePlus.h>
#include <iostream>
#include <math.h>

#include "color.h"

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

    unsigned int getWidth () const; // {return width;}
    unsigned int getHeight () const;//  {return height;}

    // I/0
    void save(const std::string& _fileName);

private:

    fipImage imageFile_;
    unsigned int width_, height_;

};

#endif

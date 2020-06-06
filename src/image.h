/* 
 *  Copyright (c) 2017  Rafael Pagés
 *
 *  This file is part of SSMVtex
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is furnished to do
 *  so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 *  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 *  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 *  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

    inline unsigned int getWidth () const {
        return width_;
    }
    inline unsigned int getHeight () const {
        return height_;
    }

    // I/0
    void save(const std::string& _fileName);

private:

    fipImage imageFile_;
    unsigned int width_, height_;
    std::string name_;

};

#endif

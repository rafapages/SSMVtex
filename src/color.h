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

#ifndef COLOR_H
#define COLOR_H

#include <iostream>

class Color {

public:

	Color (unsigned char *_p);
	Color ();
    Color (float _red, float _green, float _blue, float _alpha = 1.0);

    // Data access
    float getRed() const;
    float getGreen() const;
    float getBlue() const;
    float getAlpha() const;
    void setRed(float _r);
    void setGreen(float _r);
    void setBlue(float _r);
    void setAlpha(float _alpha);

    // Operators (They do not modify the alpha channel)
	void operator+= (const Color& _c);
	Color operator+ (const Color& _c) const;
	Color operator- (const Color& _c) const;
	Color operator* (float _f) const;
	Color operator/ (float _f) const;
	bool operator== (const Color& _c) const;

    bool equals(const Color& _c) const;

    static Color hsv2rgb(float _h, float _s, float _v);


private:

    float r_, g_, b_; // 0.0 - 255.0
    float alpha_; // 0.0 - 1.0



};


#endif

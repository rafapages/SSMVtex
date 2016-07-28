/* 
 *  Copyright (c) 2015-2016  Rafael Pagés (rps (at) gti.ssr.upm.es)
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

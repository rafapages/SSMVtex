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

private:

    float r_, g_, b_; // 0.0 - 255.0
    float alpha_; // 0.0 - 1.0



};


#endif

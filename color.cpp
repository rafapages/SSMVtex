#include "color.h"

Color::Color (unsigned char *_p){

}

Color::Color (){
	r_ = g_ = b_ = 0.0;
	alpha_ = 1.0;
}

Color::Color (float _red, float _green, float _blue, float _alpha){
	r_ = _red > 255 ? 255 : _red;
	r_ = _red < 0 ? 0 : _red;

	g_ = _green  > 255 ? 255 : _green;
	g_ = _green < 0 ? 0 : _green;

	b_ = _blue  > 255 ? 255 : _blue;
	b_ = _blue < 0 ? 0 : _blue;

	alpha_ = _alpha  > 1 ? 1 : _alpha;
	alpha_ = _alpha < 0 ? 0 : _alpha;

}

float Color::getRed() const {
	return r_;
}

float Color::getGreen() const {
	return g_;
}

float Color::getBlue() const {
	return b_;
}

float Color::getAlpha() const {
	return alpha_;
}

void Color::setRed(float _r){
	if (_r > 255.0){
		std::cerr << "Wrong red value: over 255" << std::endl;
		r_ = 255.0;
	} else if (_r < 0.0){
		std::cerr << "Wrong red value: under 0" << std::endl;
		r_ = 0.0;
	} else {
		r_ = _r;
	}
}

void Color::setGreen(float _g){
	if (_g > 255.0){
		std::cerr << "Wrong green value: over 255" << std::endl;
		g_ = 255.0;
	} else if (_g < 0.0){
		std::cerr << "Wrong green value: under 0" << std::endl;
		g_ = 0.0;
	} else {
		g_ = _g;
	}
}

void Color::setBlue(float _b){
	if (_b > 255.0){
		std::cerr << "Wrong blue value: over 255" << std::endl;
		b_ = 255.0;
	} else if (_b < 0.0){
		std::cerr << "Wrong blue value: under 0" << std::endl;
		b_ = 0.0;
	} else {
		b_ = _b;
	}
}

void Color::setAlpha(float _alpha){
	if (_alpha > 1.0){
		std::cerr << "Wrong alpha value: over 1" << std::endl;
		alpha_ = 255.0;
	} else if (_alpha < 0.0){
		std::cerr << "Wrong alpha value: under 0" << std::endl;
		alpha_ = 0.0;
	} else {
		alpha_ = _alpha;
	}
}

void Color::operator+= (const Color& _c) {
	r_ += _c.getRed();
	g_ += _c.getGreen();
	b_ += _c.getBlue();
}

Color Color::operator+ (const Color& _c) const {
	Color csum (r_ + _c.getRed(), g_ + _c.getGreen(), b_ + _c.getBlue(), alpha_);
	return csum;
}


Color Color::operator- (const Color& _c) const {
	Color cdiff (r_ - _c.getRed(), g_ - _c.getGreen(), b_ - _c.getBlue(), alpha_);
	return cdiff;
}


Color Color::operator* (float _f) const {
	Color cnew (r_ * _f, g_ * _f, b_ * _f, alpha_);
	return cnew;
}

Color Color::operator/ (float _f) const {
	Color cnew(r_ / _f, g_ / _f, b_ / _f, alpha_);
	return cnew;
}

bool Color::operator== (const Color& _c) const {

	return (r_ == _c.getRed()) && (g_ == _c.getGreen()) && (b_ == _c.getBlue()) && (alpha_ == _c.getAlpha());

}


bool Color::equals(const Color& _c) const {
	return *this == _c;
}
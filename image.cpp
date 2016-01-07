#include "Image.h"


Image::Image(){
}

Image::Image(const std::string& _fileName){

	if(!this->imageFile_.load(_fileName.c_str())){
		std::cerr << "Image " << _fileName << " could not be read" << std::endl;
		std::cerr << "Filename length " << _fileName.length() << std::endl;
	}

	width_ = imageFile_.getWidth();
	height_ = imageFile_.getHeight();
}

Image::Image(unsigned int _height, unsigned int _width, Color _background){

    width_ = _width;
    height_ = _height;

    fipImage tmp(FIT_BITMAP,width_,height_, 24);

    RGBQUAD color;
    color.rgbBlue = _background.getBlue();
    color.rgbGreen = _background.getGreen();
    color.rgbRed = _background.getRed();
    FreeImage_FillBackground(tmp, &color);

    imageFile_ = tmp;

}

Color Image::getColor (unsigned int _row, unsigned int _column) const{

    RGBQUAD col;
    imageFile_.getPixelColor(_column,_row, &col);
    Color color ((float)col.rgbRed, (float)col.rgbGreen, (float)col.rgbBlue);
    return color;

}

Color Image::interpolate (float _row, float _column, InterpolateMode _mode) const{
	
	const float c = _column - 0.5;
    const float r = _row - 0.5;
    const int r_base = (int)floor(r);
    const int c_base = (int)floor(c);
    const float delta1 = r - r_base;
    const float delta2 = c - c_base;

    Color final;
    
    // Bilinear

    if (_mode == BILINEAR){

    	Color A, B, C, D;
    	A = getColor(r_base, c_base);
    	B = getColor(r_base + 1, c_base);
    	C = getColor(r_base, c_base + 1);
    	D = getColor(r_base + 1, c_base + 1);

    	final = A + (B-A)*delta1 + (C-A)*delta2 + (A+D-B-C)*delta1*delta2;
    
    } else if (_mode == BICUBIC){
    // Bi-Cubic

    	Color M, N, O, P;
    	Color A, B, C;
    	Color row_temp [4];

    	for (int i = -1; i < 3; i++) {

    		M = getColor(r_base + i, c_base - 1);
    		N = getColor(r_base + i, c_base);
    		O = getColor(r_base + i, c_base + 1);
    		P = getColor(r_base + i, c_base + 2);

    		A = P - O;
    		B = N - M;
    		C = O - M;

    		row_temp[i+1] = N + ( ( (A+B) * delta2 - A - B - B) * delta2 + C) * delta2;
    	}

    	A = row_temp[3] - row_temp[2];
    	B = row_temp[1] - row_temp[0];
    	C = row_temp[2] - row_temp[0];

    	final = row_temp[1] + ( ( (A+B) * delta1 - A - B - B) * delta1 + C) * delta1;
    } else {
    	std::cerr << "Mode " << _mode << " is unknown" << std::endl;
    	final = Color(0,0,0);
    }
    return final;
}


void Image::setColor (const Color& _color, unsigned int _row, unsigned int _column){
    
    RGBQUAD col;
    col.rgbBlue = _color.getBlue();
    col.rgbGreen = _color.getGreen();
    col.rgbRed = _color.getRed();

    imageFile_.setPixelColor(_column,_row,&col);
}

void Image::save(const std::string& _fileName){

}

unsigned int Image::getWidth() const{
	return width_;
}

unsigned int Image::getHeight() const{
	return height_;
}

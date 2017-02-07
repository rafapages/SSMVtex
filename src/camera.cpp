/* 
 *  Copyright (c) 2015-2017  Rafael Pagés (rafael.pages (at) scss.tcd.ie)
 *
 *  This file is part of SSMVtex
 *
 *  SSMVtex is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  SSMVtex is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#include "camera.h"

Camera::Camera(){
    imWidth_ = imHeight_ = 0;
    k1_ = k2_ = 0.0; // If not distortion is specified, then it's set to 0
}

Camera::~Camera(){

}

void Camera::loadCameraParameters(const std::string &_textline){

    std::stringstream line(_textline);

    // Intrinsic parameters
    for (unsigned int i = 0; i < 3; i++){
        for (unsigned int j = 0; j < 3; j++){
            line >> K_(i,j);
        }
    }

    // Extrinsic parameters
    for (unsigned int i = 0; i < 3; i++){
        for (unsigned int j = 0; j < 3; j++){
            line >> R_(i,j);
        }
    }

    // Camera position
    for (unsigned int i = 0; i < 3; i++){
        line >> position_(i);
    }

    // Image dimensions
    line >> imWidth_ >> imHeight_;


    // Distortion parameters (they are optional)
    if (!line.eof()){
        line >> k1_ >> k2_;
    }

}

void Camera::loadBundlerCameraParameters(std::ifstream& _stream, const std::string& _imageName){

    std::string line;
    float focal;
    Vector3f t;
    Vector3f row;

    for (unsigned int cam_line = 0; cam_line < 5; cam_line++){
        std::getline(_stream, line);
        while (line.empty()) std::getline(_stream, line);

        if (cam_line == 0){ // First line is focal lenght and distortion parameters
            std::stringstream ss(line);
            ss >> focal >> k1_ >> k2_;

        } else if (cam_line == 4){ // Last line has the translation vector
            float v0,v1,v2;
            std::stringstream ss(line);
            ss >> v0 >> v1 >> v2;
            t = Vector3f(v0,-v1,-v2); // You can see an explanation of these '-' below

        } else { // The rest is R matrix
            float v0,v1,v2;
            std::stringstream ss(line);
            ss >> v0 >> v1 >> v2;
            row = Vector3f(v0,v1,v2);

            // Differently to Bundler, we use the notation where the camera "looks" to +z
            // so it is necessary to perform a 180 degrees rotation around x axis:
            //
            // | 1      0         0    |   | 1   0   0|
            // | 0  cos(180)  -sin(180)| = | 0  -1   0|
            // | 0  sin(180)   cos(180)|   | 0   0  -1|
            //
            // Which means we have to multiply y and z axes by -1:
            if (cam_line-1 == 0){
                R_.row(cam_line-1) = row;
            } else {
                R_.row(cam_line-1) = -row;
            }
        }
    }

    position_ = -R_.transpose() * t;
    setPosition(t);

    if (!loadImageDimensions(_imageName)){
        std::cerr << "Unable get " << _imageName << " dimensions!" << std::endl;
        exit(-1);
    }

    K_ << focal, 0, imWidth_*0.5,
          0, focal, imHeight_*0.5,
          0, 0, 1;
}

bool Camera::loadImageDimensions(const std::string& _imageName){

    fipImage input;
    char * name = new char[_imageName.length()];
    strcpy(name, _imageName.c_str());

    if (!input.load(name, FIF_LOAD_NOPIXELS)){
        std::cerr << "Image couldn't be loaded!" << std::endl;
        return false;
    }

    imHeight_ = input.getHeight();
    imWidth_ = input.getWidth();
    return true;
}

MatrixXf Camera::getXMatrix() const{

    MatrixXf X(4,4);
    const Vector3f t = getTranslationVector();
    for (unsigned int i = 0; i < 3; i++){
        for (unsigned int j = 0; j < 3; j++){
            X(i,j) = R_(i,j);
        }
    }

    for (unsigned int i = 0; i < 3; i++){
        X(i,3) = t(i);
        X(3,i) = 0;
    }

    X(3,3) = 1;

    return X;
}

MatrixXf Camera::getProjectiveMatrix() const {
    const MatrixXf X = getXMatrix();
    MatrixXf Kh(3,4);
    for (unsigned int i = 0; i < 3; i++){
        Kh.col(i) = K_.col(i);
        Kh(i,3) = 0;
    }

    return Kh*X;
}

Vector3f Camera::transform2CameraCoord(const Vector3f &_v) const {

    const Vector3f aux = _v - position_;
    return R_ * aux;

}

Vector3f Camera::transform2TextureCoord(const Vector3f &_v) const {

    const Vector3f aux = transform2CameraCoord(_v);
    return K_ * aux;

}

Vector2f Camera::transform2uvCoord(const Vector3f &_v) const {

    const Vector3f aux = transform2TextureCoord(_v);
    // u = p.x/p.z
    // v = p.y/p.z
    return Vector2f(aux(0)/aux(2), aux(1)/aux(2));
}

Vector3f Camera::get3Dpoint(const Vector2f &_p) const {

    Matrix3f Kinv = K_.inverse();
    Matrix3f Rinv = R_.inverse();
    Vector3f proj3D (_p[0], _p[1], 1);

    Vector3f p3D = Rinv * (Kinv * proj3D) + position_;
    return p3D;
}


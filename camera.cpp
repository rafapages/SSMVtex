#include "camera.h"

Camera::Camera(){
    imWidth_ = imHeight_ = 0;
}

Camera::~Camera(){

}

void Camera::loadCameraParameters(const std::string &_textline){

    std::stringstream line(_textline);

    // Intrinsic parameters
    for (unsigned int i = 0; i < 3; i++){
        for (unsigned int j = 0; j < 3; j++){
            float value;
            line >> value;
            this->K_(i,j) = value;
        }
    }

    // Extrinsic parameters
    for (unsigned int i = 0; i < 3; i++){
        for (unsigned int j = 0; j < 3; j++){
            float value;
            line >> value;
            this->R_(i,j) = value;
        }
    }

    // Camera position
    for (unsigned int i = 0; i < 3; i++){
        float value;
        line >> value;
        this->position_(i) = value;
    }

    // Image dimensions
    unsigned int uivalue;
    line >> uivalue;
    this->imWidth_ = uivalue;
    line >> uivalue;
    this->imHeight_ = uivalue;

}

Matrix3f Camera::getIntrinsicParam() const {
    return K_;
}

Matrix3f Camera::getExtrinsicParam() const {
    return R_;
}

Vector3f Camera::getPosition() const {
    return position_;
}

Vector2i Camera::getImageDim() const {
    const Vector2i dim(imWidth_, imHeight_);
    return dim;
}

Vector3f Camera::getTranslationVector() const{
    return -R_ * position_;
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




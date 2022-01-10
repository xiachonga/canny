#include "Canny.h"
#include "Mat.h"
#include <iostream>
#include "CImg-v.3.0.0/CImg.h"

using namespace std;
using namespace cimg_library;

const double lowThresh = 150;
const double highThresh = 170;
const double gaussianSigma = 0.5;

Mat<uchar> transformCimg2Mat(CImg<uchar> Gray_image) {
    int width = Gray_image._width;
    int height = Gray_image._height;
    Mat<uchar> mat = Mat<uchar>::zeros(width, height);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {    
            mat.ptr(i)[j] =  Gray_image(j, i, 0);
        }
    }
    return mat;
}
void transformMat2Cimg(CImg<uchar> &Gray_image, Mat<uchar> mat) {
    for (int i = 0; i < mat.getHeight(); ++i) {
        for (int j = 0; j < mat.getWidth(); ++j) {  
            Gray_image(j, i, 0) = mat.ptr(i)[j];
        }
    }
}
int main() {
    CImg<uchar> image("lena1.bmp");
    Canny *canny = new Canny();
    CImg<uchar> Gray_image = canny->getGrayImage(image);
    Mat<uchar> mat = transformCimg2Mat(Gray_image);
    Mat<uchar> gaussian_img = mat;
    //gaussian_img = canny->gaussianFilter(mat, gaussianSigma);
    transformMat2Cimg(Gray_image, gaussian_img);
    //Gray_image.display("lena_out");
    Mat<uchar> grad, theta, dst, res;
	canny->sobel(gaussian_img, grad, theta);
	canny->NMS(grad, theta, dst);
	canny->doubleThreshSelection(dst, res, lowThresh, highThresh);
    transformMat2Cimg(Gray_image, res);
    Gray_image.display("lena_out");
    Gray_image.save("lena_out.bmp");

    return 0;
}
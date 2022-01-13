#include "Canny.h"
#include "Mat.h"
#include <iostream>
#include <fstream>
#include "CImg-v.3.0.0/CImg.h"

using namespace std;
using namespace cimg_library;

const double lowThresh = 70;
const double highThresh = 200;
const double gaussianSigma = 1;

Mat<uchar> transformCimg2Mat(CImg<uchar> Gray_image) {
    int width = Gray_image._width;
    int height = Gray_image._height;
    cout << width << height <<endl;
    Mat<uchar> mat = Mat<uchar>::zeros(width, height);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {    
            mat.ptr(i)[j] =  Gray_image(j, i, 0);
        }
    }
    return mat;
}

void transformMat2Cimg(CImg<uchar> &Gray_image, Mat<uchar> mat) {
    cout << mat.getHeight() << mat.getWidth()<<endl;
    for (int i = 0; i < mat.getHeight(); ++i) {
        for (int j = 0; j < mat.getWidth(); ++j) {  
            Gray_image(j, i, 0) = mat.ptr(i)[j];
        }
    }
}

CImg<uchar> getGrayImage(CImg<uchar> image) {
    int width = image._width;
    int height = image._height;
    CImg<uchar> Gray_image(width, height, 1, 1, 0);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            double r = image(j, i, 0);
            double g = image(j, i, 1);
            double b = image(j, i, 2);
            Gray_image(j, i, 0) = 0.299*r + 0.587*g + 0.114*b;
        }
    }
    return Gray_image;

}
Mat<uchar> readData(int width, int height) {
    Mat<uchar> mat = Mat<uchar>::zeros(width, height);
    int **a;
    a = new int*[height];
    for (int i = 0; i < width; ++i) {
        a[i] = new int[width];
    }
    FILE *fp = fopen("data.txt", "r");
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            fscanf(fp, "%d", &a[i][j]);
            (mat.getData()+ (i * width))[j] = (uchar)a[i][j];
        }
        fscanf(fp, "\n");
    }
    cout <<"======"<<endl;
    return mat;
}
int main() {
    /*CImg<uchar> image("lena1.bmp");
    Canny *canny = new Canny();
    CImg<uchar> Gray_image = getGrayImage(image);
    Mat<uchar> mat = transformCimg2Mat(Gray_image);*/
    Mat<uchar> Gray_image = readData(512, 512);
    Mat<uchar> gaussian_img = Gray_image;
    Canny *canny = new Canny();
    //gaussian_img = canny->gaussianFilter(mat, gaussianSigma);
    //transformMat2Cimg(Gray_image, gaussian_img);
    //cout << Gray_image << endl;
    //Gray_image.display("lena_out");
    Mat<uchar> grad, theta, dst, res;
	canny->sobel(gaussian_img, grad, theta);
    cout <<"===sobel==="<<endl;
	canny->NMS(grad, theta, dst);
    cout <<"===nms==="<<endl;
	canny->doubleThreshSelection(dst, res, lowThresh, highThresh);
    cout <<"===double==="<<endl;
    CImg<uchar> resImage(512, 512, 1, 1, 0);
    transformMat2Cimg(resImage, res);
    resImage.display("lena_out");
    resImage.save("lena_out.bmp");

    return 0;
}
#include "Canny.h"
#include "Mat.h"
#include <iostream>
#include <fstream>

using namespace std;

const uint8_t lowThresh = 70;
const uint8_t highThresh = 200;
const double gaussianSigma = 1;
const int height = 512;
const int width = 512;

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
    return mat;
}

int main() {
    Mat<uchar> Gray_image = readData(height, width);
    Mat<uchar> gaussian_img = Gray_image;
    Canny *canny = new Canny();
    Mat<uchar> grad, theta, dst, res;
	canny->sobel(gaussian_img, grad, theta);
	canny->NMS(grad, theta, dst);
	canny->doubleThreshSelection(dst, res, lowThresh, highThresh);
    return 0;
}
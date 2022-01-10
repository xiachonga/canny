#include "Canny.h"
#include <math.h>

using namespace std;

const double  PI = 3.14159265358979323846;

CImg<uchar> Canny::getGrayImage(CImg<uchar> image) {
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

Mat<uchar> Canny::gaussianFilter(Mat<uchar> &image, double sigma) {
    int width = image.getWidth();
    int height = image.getHeight();
    Mat<uchar> mat(width, height);
    //根据sigma确定高斯核的大小
    int len = 1 + 2 * ceil(3 * sigma);
    int center = len / 2;

    Mat<double> gaussTemplate(len, len);
    double sum = 0.0;
    for (int i =0; i < len; ++i) {
        int x = i - len / 2;
        for (int j = 0; j < len; ++j) {
            int y = j - len/2;
            gaussTemplate.ptr(i)[j] = exp(-(pow(x, 2) + pow(y, 2)) / (2 * pow(sigma, 2)));
            sum += gaussTemplate.ptr(i)[j];
        }
    }

    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) {
            gaussTemplate.ptr(i)[j] /= sum;
        }
    }

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            for (int m = 0; m < len; ++m) {
                for (int n = 0; n < len; ++n) {
                    if (i - center + m >= 0 && i - center + m < height &&
                        j - center + n >= 0 && j - center + n < width) {
                        mat.ptr(i)[j] += (uchar)(gaussTemplate.ptr(m)[n] * image.ptr(i - center + m)[j - center + n]);
                    } else {
                        mat.ptr(i)[j] += (uchar)(gaussTemplate.ptr(m)[n] * image.ptr(i)[j]);
                    }
                }
            }
        }
    }
    return mat;
}

void Canny::sobel(Mat<uchar> &image, Mat<uchar> &grad, Mat<uchar> &theta) {
    int width = image.getWidth();
    int height = image.getHeight();
    grad = Mat<uchar>::zeros(width, height);
    theta = Mat<uchar>::zeros(width, height);

    for (int i = 1; i < height - 1; ++i) {
        for (int j = 1; j < width - 1; ++j) {
            double gradY = double(image.ptr(i - 1)[j - 1] + 2 * image.ptr(i - 1)[j] + image.ptr(i - 1)[j + 1]
                                  - image.ptr(i + 1)[j - 1] - 2 * image.ptr(i + 1)[j] - image.ptr(i + 1)[j + 1]);
            double gradX = double(image.ptr(i - 1)[j + 1] + 2 * image.ptr(i)[j + 1] + image.ptr(i + 1)[j + 1]
                                  - image.ptr(i - 1)[j - 1] - 2 * image.ptr(i)[j - 1] - image.ptr(i + 1)[j - 1]);
            grad.ptr(i)[j] = (uchar)sqrt(pow(gradX, 2) + pow(gradY, 2));
            theta.ptr(i)[j] = (uchar)atan(gradY / gradX);
        }
    }
}

void Canny::NMS(Mat<uchar> &grad, Mat<uchar> &theta, Mat<uchar> &dst) {
    dst = grad;
    int height = grad.getHeight();
    int width = grad.getWidth();
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            double gradValue = double(dst.ptr(i)[j]);
            if (gradValue == 0.0) {
                continue;
            }
            double currValue = double(theta.ptr(i)[j]);
            double connectValue1, connectValue2;
            if (currValue >= -PI / 8 && currValue < PI / 8) {
                //左右
                connectValue1 = double(dst.ptr(i)[j - 1]);
                connectValue2 = double(dst.ptr(i)[j + 1]);
            } else if (currValue >= PI / 8 && currValue < 3 * PI / 8) {
                //右上左下
                connectValue1 = double(dst.ptr(i + 1)[j - 1]);
                connectValue2 = double(dst.ptr(i - 1)[j + 1]);
            } else if (currValue >= -3 * PI / 8 && currValue < - PI / 8) {
                //左上右下
                connectValue1 = double(dst.ptr(i - 1)[j - 1]);
                connectValue2 = double(dst.ptr(i + 1)[j + 1]);
            } else {
                //上下
                connectValue1 = double(dst.ptr(i - 1)[j]);
                connectValue2 = double(dst.ptr(i + 1)[j]);
            }
            if (gradValue <= connectValue1 || gradValue <= connectValue2) {
                dst.ptr(i)[j] = (uchar)0;
            }
        }
    }

}
void Canny::doubleThreshSelection(Mat<uchar> &image, Mat<uchar> &dst, double lowThresh, double highThresh) {
    dst = image;
    int height = image.getHeight();
    int width = image.getWidth();

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            double gradValue = double(dst.ptr(i)[j]);
            if (gradValue > highThresh) {
                dst.ptr(i)[j] = (uchar)255;
            } else if (gradValue < lowThresh) {
                dst.ptr(i)[j] = (uchar)0;
            }
        }
    }

    this->edgeConnect(dst);
}
void Canny::edgeConnect(Mat<uchar> &image) {
    int height = image.getHeight();
    int width = image.getWidth();
    
    for (int i = 1; i < height - 1; ++i) {
        for (int j = 1; j < width - 1; ++j) {
            if (image.ptr(i)[j] == 255) {
                for (int m = -1; m < 1; ++m) {
                    for (int n = -1; n < 1; ++n) {
                        if (image.ptr(i + m)[j + n] != 0 && image.ptr(i +m)[j + n] != 255) {
                            image.ptr(i + m)[j + n] = 255;
                        }
                    }
                }
            }
        }
    }
    
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (image.ptr(i)[j] != 255) {
                image.ptr(i)[j] = 0;
            }
        }
    }

}
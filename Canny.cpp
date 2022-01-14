#include "Canny.h"
#include <math.h>

using namespace std;

const double  PI = 3.14159265358979323846;
const int height = 512;
const int width = 512;
#define ptr(i) i * width

Mat<uchar> Canny::gaussianFilter(Mat<uchar> &image, double sigma) {
    Mat<uchar> mat(width, height);
    //根据sigma确定高斯核的大小
    int len = 1 + 2 * ceil(3 * sigma);
    int center = len / 2;

    Mat<double> gaussTemplate(len, len);
    double sum = 0.0;
    uchar* data = mat.getData();
    uchar* resdata = mat.getData();
    uchar* gaussData = mat.getData();
    for (int i =0; i < len; ++i) {
        int x = i - len / 2;
        for (int j = 0; j < len; ++j) {
            int y = j - len/2;
            (gaussData + ptr(i))[j] = exp(-(pow(x, 2) + pow(y, 2)) / (2 * pow(sigma, 2)));
            sum += (gaussData + ptr(i))[j];
        }
    }

    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) {
            (gaussData + ptr(i))[j] /= sum;
        }
    }

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            for (int m = 0; m < len; ++m) {
                for (int n = 0; n < len; ++n) {
                    if (i - center + m >= 0 && i - center + m < height &&
                        j - center + n >= 0 && j - center + n < width) {
                        (resdata + ptr(i))[j] += (uchar)((gaussData + ptr(m))[n] * (data + ptr(i - center + m))[j - center + n]);
                    } else {
                        (resdata + ptr(i))[j] += (uchar)((gaussData + ptr(m))[n] * (data + ptr(i))[j]);
                    }
                }
            }
        }
    }
    return mat;
}

void Canny::sobel(Mat<uchar> &image, Mat<uchar> &grad, Mat<uchar> &theta) {
    grad = Mat<uchar>::zeros(width, height);
    theta = Mat<uchar>::zeros(width, height);
    uchar *data = image.getData();
    uchar *gradData = grad.getData();
    uchar *thetaData = theta.getData();
    for (int i = 1; i < height - 1; ++i) {
        for (int j = 1; j < width - 1; ++j) {
            double gradY = double((data + ptr(i - 1))[j - 1] + 2 * (data + ptr(i - 1))[j] + (data + ptr(i - 1))[j + 1]
                                  - (data + ptr(i + 1))[j - 1] - 2 * (data + ptr(i + 1))[j] - (data + ptr(i + 1))[j + 1]);
            double gradX = double((data + ptr(i - 1))[j + 1] + 2 * (data + ptr(i))[j + 1] + (data + ptr(i + 1))[j + 1]
                                  - (data + ptr(i - 1))[j - 1] - 2 * (data + ptr(i))[j - 1] - (data + ptr(i + 1))[j - 1]);
            (gradData + ptr(i))[j] = (uchar)sqrt(pow(gradX, 2) + pow(gradY, 2));
            (thetaData + ptr(i))[j] = (uchar)atan(gradY / gradX);
        }
    }
}

void Canny::NMS(Mat<uchar> &grad, Mat<uchar> &theta, Mat<uchar> &dst) {
    dst = grad;
    uchar *data = dst.getData();
    uchar *thetaData = theta.getData();
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            double gradValue = double((data + ptr(i))[j]);
            if (gradValue == 0.0) {
                continue;
            }
            double currValue = double((thetaData + ptr(i))[j]);
            double connectValue1, connectValue2;
            if (currValue >= -PI / 8 && currValue < PI / 8) {
                //左右
                connectValue1 = double((data + ptr(i))[j - 1]);
                connectValue2 = double((data + ptr(i))[j + 1]);
            } else if (currValue >= PI / 8 && currValue < 3 * PI / 8) {
                //右上左下
                connectValue1 = double((data + ptr(i + 1))[j - 1]);
                connectValue2 = double((data + ptr(i - 1))[j + 1]);
            } else if (currValue >= -3 * PI / 8 && currValue < - PI / 8) {
                //左上右下
                connectValue1 = double((data + ptr(i - 1))[j - 1]);
                connectValue2 = double((data + ptr(i + 1))[j + 1]);
            } else {
                //上下
                connectValue1 = double((data + ptr(i - 1))[j]);
                connectValue2 = double((data + ptr(i + 1))[j]);
            }
            if (gradValue <= connectValue1 || gradValue <= connectValue2) {
                (data + ptr(i))[j] = (uchar)0;
            }
        }
    }

}
void Canny::doubleThreshSelection(Mat<uchar> &image, Mat<uchar> &dst, double lowThresh, double highThresh) {
    dst = image;
    uchar *data = dst.getData();
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            double gradValue = double((data + ptr(i))[j]);
            if (gradValue > highThresh) {
                (data + ptr(i))[j] = (uchar)255;
            } else if (gradValue < lowThresh) {
                (data + ptr(i))[j] = (uchar)0;
            }
        }
    }

    this->edgeConnect(dst);
}
void Canny::edgeConnect(Mat<uchar> &image) {
    uchar *imageData = image.getData();
    for (int i = 1; i < height - 1; ++i) {
        for (int j = 1; j < width - 1; ++j) {
            if ((imageData + ptr(i))[j] == 255) {
                for (int m = -1; m <= 1; ++m) {
                    for (int n = -1; n <= 1; ++n) {
                        if ((imageData + ptr(i + m))[j + n] != 0 && (imageData + ptr(i +m))[j + n] != 255) {
                            (imageData + ptr(i + m))[j + n] = 255;
                        }
                    }
                }
            }
        }
    }
    
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if ((imageData + ptr(i))[j] != 255) {
                (imageData + ptr(i))[j] = 0;
            }
        }
    }

}
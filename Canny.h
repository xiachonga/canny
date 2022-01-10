#ifndef __CANNY__
#define __CANNY__
#include "Mat.h"
#include "common.h"
#include "CImg-v.3.0.0/CImg.h"

using namespace cimg_library;

class Canny {
    public:
        CImg<uchar> getGrayImage(CImg<uchar> image);

        Mat<uchar> gaussianFilter(Mat<uchar> &image, double sigma);

        void sobel(Mat<uchar> &image, Mat<uchar> &grad, Mat<uchar> &theta);
        
        void NMS(Mat<uchar> &grad, Mat<uchar> &theta, Mat<uchar> &dst);

        void doubleThreshSelection(Mat<uchar> &image, Mat<uchar> &dst, double lowThresh, double highThresh);

        void edgeConnect(Mat<uchar> &image);

};  
#endif
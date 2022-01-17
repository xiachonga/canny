#ifndef __CANNY__
#define __CANNY__
#include "Mat.h"
#include "common.h"


class Canny {
    public:
    
        Mat<uchar> gaussianFilter(Mat<uchar> &image, double sigma);

        void sobel(Mat<uchar> &image, Mat<uchar> &grad, Mat<uchar> &theta);
        
        void NMS(Mat<uchar> &grad, Mat<uchar> &theta, Mat<uchar> &dst);

        void doubleThreshSelection(Mat<uchar> &image, Mat<uchar> &dst, uint8_t lowThresh, uint8_t highThresh);

        void edgeConnect(Mat<uchar> &image);

};  
#endif
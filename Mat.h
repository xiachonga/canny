#ifndef __MAT__
#define __MAT__
#include <iostream>
#include <fstream>
#include "common.h"
using namespace std;

template <typename T>
class Mat {
    private:
        T* data;
        int width, height;
    public:
        Mat() : data(nullptr), width(0), height(0) {}
        Mat(int width_, int height_) : width(width_), height(height_) {
            data = new T[width * height];
        }
        Mat(T *data_, int width_, int height_) : data(data_), width(width_), height(height_) {}

        inline T* ptr(int n = 0) const{
            return data + (n * width);
        }

        T* getData() {
            return data;
        }
        
        int getWidth() {
            return width;
        }

        int getHeight() {
            return height;
        }

        static Mat zeros(int width, int height) {
            Mat res = Mat(width, height);
            for (int i = 0; i < height; ++i) {
                for (int j = 0; j < width; ++j) {
                    res.ptr(i)[j] = (T)0;
                }
            }
            return res;
        }

        friend ostream &operator<<(ostream &output, const Mat &mat) {
            output << mat.width << " " << mat.height << "\n";
            fstream fs;
            fs.open("data.txt", ios_base::out);
            for (int i = 0; i < mat.height; ++i) {
                for (int j = 0; j < mat.width; ++j) {
                    if (sizeof(T) == 1) { 
                        output << (int)mat.ptr(i)[j] << " ";
                        if (j == mat.width) {
                            fs<< (int)mat.ptr(i)[j];
                        } else {
                            fs<< (int)mat.ptr(i)[j] <<" ";
                        }
                    } else {
                        output << mat.ptr(i)[j] << " ";
                    }
                }
                output << "\n";
                fs<<"\n";
            }
            return output;
        }
};



#endif

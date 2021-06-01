//
// Created by Vitalik on 22.03.2021.
//

#ifndef LABA2_MATRIX_H
#define LABA2_MATRIX_H
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

template <class T>
class Matrix {
private:
    unsigned int _n = 0;
    unsigned int _m = 0;
    T * _mP;
public:
    Matrix() {
        _n = 0;
        _m = 0;
        _mP = nullptr;
    }

    Matrix(unsigned int n,unsigned int m) {
        _n = n ;
        _m = m;
        _mP = new T[_n*_m];
        for (int i = 0; i < _n*_m; ++i) {
            _mP[i] = 0;
        }
    }

    Matrix(unsigned int n,unsigned int m, int *c_matrix) {

        _n = n ;
        _m = m;
        _mP = new T[_n*_m];
        for (int i = 0; i < _n*_m; ++i) {
            _mP[i] = c_matrix[i];
        }
    }
    ~Matrix() {

        delete[] _mP;
    }

    Matrix(const Matrix &copy) {
        this->_n = copy._n;
        this->_m = copy._m;
        _mP = new T[this->_n*this->_m];
        for (int i = 0; i < _n*_m; ++i) {
            this->_mP[i] = copy._mP[i];
        }
    }

    Matrix&operator= (const Matrix &drop) {

        this->_n = drop._n;
        this->_m = drop._m;
        _mP = new T[this->_n*this->_m];
        for (int i = 0; i < _n*_m; ++i) {
            this->_mP[i] = drop._mP[i];
        }
    }


    Matrix operator+ (const Matrix &add) {

        if(this->_n != add._n | this->_m != add._m) {
            return Matrix(1,1);
        }
        Matrix<T> result(_n,_m);
        for (int i = 0; i < result._n; ++i) {
            for (int j = 0; j < result._m; ++j) {
                result._mP[i*result._m + j] = this->_mP[i*result._m + j]+ add._mP[i*result._m + j];
            }
        }

        return result;
    }

    Matrix<float> operator/ (const Matrix &det) {

        if(this->_n != det._n | this->_m != det._m) {
            return Matrix<float>(1,1);
        }
        Matrix<float> result(_n,_m);
        for (int i = 0; i < det._n; ++i) {
            for (int j = 0; j < det._m; ++j) {
                result[i][j] = 1.0*this->_mP[i*det._m + j]/det._mP[i*det._m + j];
            }
        }

        return result;
    }

    T* operator[](const unsigned int i) {

        return _mP+ i*_m;

    }

    T maxInColumn(int colNum){
        if(colNum >= _m )
            std::cout << "Error: wrong column index" << std::endl;

        T max = _mP[colNum];
        for (int i = 1; i < _n; ++i) {
            if(max < _mP[_m*i + colNum]) max = _mP[_m*i + colNum];
        }
        return max;
    }

    T minInColumn(int colNum){
        if(colNum >= _m )
            std::cout << "Error: wrong column index" << std::endl;

        T min = _mP[colNum];
        for (int i = 1; i < _n; ++i) {
            if(min > _mP[_m*i + colNum]) min = _mP[_m*i + colNum];
        }
        return min;
    }

    T max(){
        T max = _mP[0];
        for (int i = 1; i < _n*_m; ++i)
            if(max < _mP[i]) max = _mP[i];

        return max;
    }

    T min(){
        T min = _mP[0];
        for (int i = 1; i < _n*_m; ++i)
            if(min > _mP[i]) min = _mP[i];

        return min;
    }

    Matrix<int>  maxInColumnIndex(int colNum){
        if(colNum >= _m )
            std::cout << "Error: wrong column index" << std::endl;

        Matrix<int> index(1,2);
        index[0][0]=0;
        index[0][1]=0;

        int max = _mP[colNum];
        for (int i = 1; i < _n; ++i)
            if(max < _mP[_m*i + colNum])
            {
                max = _mP[_m*i + colNum];
                index[0][0]=i;
                index[0][1]=colNum;
            }

        return index;
    }

    Matrix<int> maxIndex(){
        T max = _mP[0];
        Matrix<int> index(1,2);
        index[0][0]=0;
        index[0][1]=0;

        for (int i = 0; i < this->_n; ++i)
            for (int j = 0; j < this->_m; ++j)
                if(max < _mP[i]) {
                    max = _mP[i];
                    index[0][0]=i;
                    index[0][1]=j;
                }

        return index;
    }

    Matrix<int> minIndex(){
        T max = _mP[0];
        Matrix<int> index(1,2);
        index[0][0]=0;
        index[0][1]=0;

        for (int i = 0; i < this->_n; ++i)
            for (int j = 0; j < this->_m; ++j)
                if(max > _mP[i]) {
                    max = _mP[i];
                    index[0][0]=i;
                    index[0][1]=j;
                }

        return index;
    }



    Matrix<T> addRaws() {
        Matrix<T> result(1,_m);

        for (int i = 0; i < _m; ++i) {
            for (int j = 0; j < _n; ++j) {
                result[0][i] += _mP[j*_m + i] ;
            }
        }
        return result;
    }


    Matrix<T> subMatrix(int n, int m) {
        if(this->_n < n | this->_m < m) {
            return Matrix(1,1);
        }
        Matrix<T> result(n,m);

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j <m; ++j) {
                result[i][j] = 1.0*this->_mP[i*this->_m + j];
            }
        }
        return result;
    }

    T fullSum() {
        T result = 0;
        for (int i = 0; i < this->_n; ++i) {
            for (int j = 0; j < this->_m; ++j) {
                result += this->_mP[i*this->_m + j];
            }
        }
        return result;
    }




    std::string out(){
        std::stringstream ss;
        ss.precision(3);

        std::cout << std::fixed;
        std::cout << std::setprecision(3);
        for (int i = 0; i < _n*_m; ++i) {
            if(i%_m == 0 && i != 0) ss << std::endl;
            ss << _mP[i] << '\t';
        }
        ss << std::endl;
        return ss.str();
    }

    std::string outWithNumerating(){
        std::stringstream ss;
        ss << " | ";
        for (int i = 0; i < _m; ++i) {
            if(i != 0)
                ss << "   ";
            ss << i+1 << '\t';
        }
        ss << std::endl;
        for (int i = 0; i < _m+(_m-1)*7+3; ++i) {
            ss << '-';
        }
        ss << std::endl;

        for (int i = 0; i < this->_n; ++i) {
            ss << i+1 << "| ";
            for (int j = 0; j < this->_m; ++j) {
                if(j != 0)
                    ss << "   ";
                ss << _mP[i*this->_m + j] << '\t';
            }
            ss << std::endl;
        }
        return ss.str();
    }

    unsigned int getHeight(){ return  _n;}
    unsigned int getWidth(){ return  _m;}



};

#endif //LABA2_MATRIX_H

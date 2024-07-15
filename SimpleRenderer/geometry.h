#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <iostream>
#include <cmath>
#include <vector>
#include <cassert>

template<size_t Dimcols, size_t DimRows, typename T> class mat;

//------------------NEW----------------------------
//general framweork for vectors from all dimensions
template<size_t DIM, typename T> struct vec {
    vec()
    { 
         for (size_t i = DIM; i--; data_[i] = T()); 
    }
    T& operator[](const size_t i) { assert(i < DIM); return data_[i]; }
    const T& operator[](const size_t i) const { assert(i < DIM); return data_[i]; }
private:
    T data_[DIM];
};

template<typename T> struct vec<2, T> {
   T x, y; 
   vec(): x (T()), y (T()){}
   vec(T _x, T _y) : x(_x), y(_y){}
   //instantiate with different class
   template <class U> vec<2, T>(const vec<2, U>& v);
   T& operator[](const int index) { return index == 0 ? x : y; }
   const T& operator[](const int index) const { return index == 0 ? x : y; }
};
template <typename T> struct vec<3, T> {
    vec() : x(T()), y(T()), z(T()) {}
    vec(T X, T Y, T Z) : x(X), y(Y), z(Z) {}
    template <class U> vec<3, T>(const vec<3, U>& v);
    T& operator[](const size_t i) { assert(i < 3); return i <= 0 ? x : (1 == i ? y : z); }
    const T& operator[](const size_t i) const { assert(i < 3); return i <= 0 ? x : (1 == i ? y : z); }
    float norm() { return std::sqrt(x * x + y * y + z * z); }
    vec<3, T>& normalize(T l = 1) { *this = (*this) * (l / norm()); return *this; }

    T x, y, z;
};
//---------------operators for generic vector struct-non member-----------------
//*operator-dot product
template<size_t DIM, typename T> T operator*(const vec<DIM, T>& left, const vec<DIM, T>& right) {
    T res = T();
    for (size_t i = 0; i<DIM;i++ )
    {
        res+=left[i] * right[i];
    }
    return res;
}
//*operator- simple multiplication but with different types
template<size_t DIM, typename T, typename U> vec<DIM, T> operator*(vec<DIM, T> left, const U& right) {

    for (size_t i = 0; i < DIM; i++)
    {
        left[i] *= right;
    }
    return left;
}
// divident operator/ simple division but with different types
template<size_t DIM, typename T, typename U> vec<DIM, T> operator/(vec<DIM, T> left, const U& right) {

    for (size_t i = 0; i < DIM; i++)
    {
        left[i] /= right;
    }
    return left;
}
//+operator
template<size_t DIM, typename T> vec<DIM, T> operator+(vec<DIM, T> left, const vec<DIM, T>& right) {
   
    for (size_t i = 0; i < DIM; i++)
    {
        left[i] += right[i];
    }
    return left;
}
//-operator
template<size_t DIM, typename T> vec<DIM, T> operator-(vec<DIM, T> left, const vec<DIM, T>& right) {

    for (size_t i = 0; i < DIM; i++)
    {
        left[i] -= right[i];
    }
    return left;
}
//embed : it extends a vector. 
template<size_t LEN, size_t DIM, typename T> vec<LEN, T> embed(const vec<DIM, T>& vector, T fillvalue = 1) {
    vec<LEN, T> res;
    for (size_t i = 0; i < LEN;i++){
        res[i] = i < DIM ? vector[i] : fillvalue;
    }
    return res;
}
//projection of one vector to another
template<size_t LEN, size_t DIM, typename T> vec<LEN, T> proj(const vec<DIM, T>& v) {
    vec<LEN, T> res;
    for (int i = LEN-1; i >= 0; i--) {
        res[i] = v[i];
    }
    return res;
}
//cross product from wiki
template <typename T> vec<3, T> cross(vec<3, T> v1, vec<3, T> v2) {
    return vec<3, T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}


template <size_t DIM, typename T> std::ostream& operator<<(std::ostream& out, vec<DIM, T>& v) {
    for (unsigned int i = 0; i < DIM; i++) {
        out << v[i] << " ";
    }
    return out;
}
//---------------operators for generic vector struct-non member-----------------

//---------------dt------------------------------
template<size_t DIM, typename T> struct dt {
    static T det(const mat<DIM, DIM, T>& src) {
        T ret = 0;
        for (size_t i = DIM; i--; ret += src[0][i] * src.cofactor(0, i));
        return ret;
    }
};

template<typename T> struct dt<1, T> {
    static T det(const mat<1, 1, T>& src) {
        return src[0][0];
    }
};
//---------------dt------------------------------

//----------------------matrix---------------------------------
template<size_t DimRows, size_t DimCols, typename T> class mat {
    vec<DimCols, T> rows[DimRows];
public:
    mat() {}

    vec<DimCols, T>& operator[] (const size_t idx) {
        assert(idx < DimRows);
        return rows[idx];
    }

    const vec<DimCols, T>& operator[] (const size_t idx) const {
        assert(idx < DimRows);
        return rows[idx];
    }

    vec<DimRows, T> col(const size_t idx) const {
        assert(idx < DimCols);
        vec<DimRows, T> ret;
        for (size_t i = DimRows; i--; ret[i] = rows[i][idx]);
        return ret;
    }

    void set_col(size_t idx, vec<DimRows, T> v) {
        assert(idx < DimCols);
        for (size_t i = DimRows; i--; rows[i][idx] = v[i]);
    }

    static mat<DimRows, DimCols, T> identity() {
        mat<DimRows, DimCols, T> ret;
        for (size_t i = DimRows; i--; )
            for (size_t j = DimCols; j--; ret[i][j] = (i == j));
        return ret;
    }

    T det() const {
        return dt<DimCols, T>::det(*this);
    }

    mat<DimRows - 1, DimCols - 1, T> get_minor(size_t row, size_t col) const {
        mat<DimRows - 1, DimCols - 1, T> ret;
        for (size_t i = DimRows - 1; i--; )
            for (size_t j = DimCols - 1; j--; ret[i][j] = rows[i < row ? i : i + 1][j < col ? j : j + 1]);
        return ret;
    }

    T cofactor(size_t row, size_t col) const {
        return get_minor(row, col).det() * ((row + col) % 2 ? -1 : 1);
    }

    mat<DimRows, DimCols, T> adjugate() const {
        mat<DimRows, DimCols, T> ret;
        for (size_t i = DimRows; i--; )
            for (size_t j = DimCols; j--; ret[i][j] = cofactor(i, j));
        return ret;
    }

    mat<DimRows, DimCols, T> invert_transpose() {
        mat<DimRows, DimCols, T> ret = adjugate();
        T tmp = ret[0] * rows[0];
        return ret / tmp;
    }
};

//matirx operations non member

template<size_t DimRows, size_t DimCols, typename T> vec<DimRows, T> operator*(const mat<DimRows, DimCols, T>& lhs, const vec<DimCols, T>& rhs) {
    vec<DimRows, T> ret;
    for (size_t i = DimRows; i--; ret[i] = lhs[i] * rhs);
    return ret;
}

template<size_t R1, size_t C1, size_t C2, typename T>mat<R1, C2, T> operator*(const mat<R1, C1, T>& lhs, const mat<C1, C2, T>& rhs) {
    mat<R1, C2, T> result;
    for (size_t i = R1; i--; )
        for (size_t j = C2; j--; result[i][j] = lhs[i] * rhs.col(j));
    return result;
}

template<size_t DimRows, size_t DimCols, typename T>mat<DimCols, DimRows, T> operator/(mat<DimRows, DimCols, T> lhs, const T& rhs) {
    for (size_t i = DimRows; i--; lhs[i] = lhs[i] / rhs);
    return lhs;
}

template <size_t DimRows, size_t DimCols, class T> std::ostream& operator<<(std::ostream& out, mat<DimRows, DimCols, T>& m) {
    for (size_t i = 0; i < DimRows; i++) out << m[i] << std::endl;
    return out;
}

/////////////////////////////////////////////////////////////////////////////////

typedef vec<2, float> Vec2f;
typedef vec<2, int>   Vec2i;
typedef vec<3, float> Vec3f;
typedef vec<3, int>   Vec3i;
typedef vec<4, float> Vec4f; //for shaders and homogenous 3d coordinates we need 4 dimension
typedef mat<4, 4, float> Matrix;

#endif
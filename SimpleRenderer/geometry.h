#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <vector>

template<size_t Dimcols, size_t DimRows, typename T> class mat;

//general framwork for vectors from all dimensions
template<size_t DIM, typename T> struct vec {
    vec()
    { 
        for (size_t i{ DIM }; i >= 0; i--) {
            data_[i] = T();
        }
    }
    T& operator[](const size_t i) { assert(i < DIM); return data_[i]; }
    const T& operator[](const size_t i) const { assert(i < DIM); return data_[i]; }
private:
    T data_[DIM];
};

//
template<typename T> struct vec<2, T> {
   T x, y; 
   vec(): x (T()), y (T()){}
   vec(T _x, T _y) : x(_x), y(_y){}
   //instantiate with different class
   template <class U> vec<2, T>(const vec<2, U>& v);
   T& operator[](const int index) { return index == 0 ? x : y; }
   const T& operator[](const int index) const { return index == 0 ? x : y; }
};













//Forward declare matrix
class Matrix;

template<class T> struct Vec2 {
    T x, y;
    Vec2<T>() : x{T{}}, y{T{}} {}
    Vec2<T>(T _x, T _y): x{_x}, y{_y} {}
    Vec2<T> operator +(const Vec2<T>& vec) const { return Vec2<T>{vec.x + x, vec.y + y}; }
    Vec2<T> operator -(const Vec2<T>& vec) const { return Vec2<T>{x - vec.x, y - vec.y}; }
    Vec2<T> operator *(float f) const { return Vec2<T>{x* f, y* f }; }
    T& operator[](const int i) { return i == 0 ? x : y; }
    template<class > friend std::ostream& operator<<(std::ostream& out, Vec2<T>& vec);
};

template<class t> struct Vec3 {
    t x, y, z;
    Vec3<t>() : x(t()), y(t()), z(t()) { }
    Vec3<t>(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
    Vec3<t>(Matrix m);
    template <class u> Vec3<t>(const Vec3<u>& v);
    Vec3<t> operator ^(const Vec3<t>& v) const { return Vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
    Vec3<t> operator +(const Vec3<t>& v) const { return Vec3<t>(x + v.x, y + v.y, z + v.z); }
    Vec3<t> operator -(const Vec3<t>& v) const { return Vec3<t>(x - v.x, y - v.y, z - v.z); }
    Vec3<t> operator *(float f)          const { return Vec3<t>(x * f, y * f, z * f); }
    t       operator *(const Vec3<t>& v) const { return x * v.x + y * v.y + z * v.z; }
    float norm() const { return std::sqrt(x * x + y * y + z * z); }
    Vec3<t>& normalize(t l = 1) { *this = (*this) * (l / norm()); return *this; }
    t& operator[](const int i) { return i <= 0 ? x : (1 == i ? y : z); }
    template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
};




typedef Vec2<int    > Vec2i;
typedef Vec2<float  > Vec2f;
typedef Vec3<float  > Vec3f;
typedef Vec3<int    > Vec3i;

template <> template <> Vec3<int>::Vec3(const Vec3<float>& v);
template <> template <> Vec3<float>::Vec3(const Vec3<int>& v);

template<class T> std::ostream& operator<<(std::ostream& out, Vec3<T>& v) {
    out << "Vector3: " << v.x << ", " << v.y << ", " << v.z << '\n';
    return out;
}
template<class T> std::ostream& operator<<(std::ostream& out, Vec2<T>& v) {
    out << "Vector2: " << v.x << ", " << v.y <<'\n';
    return out;
}

const int DEFAULT_ALLOC = 4;
class Matrix {
private:
    //rows-columns for multiplication together
    std::vector<std::vector<float>> m;
    int rows, columns;
public:
    Matrix(int r = DEFAULT_ALLOC, int c = DEFAULT_ALLOC);
    Matrix(Vec3f v);
    inline int nrows();
    inline int nrows() const;
    inline int ncols();
    inline int ncols() const;

    static Matrix identity(int dimensions);
    std::vector<float>& operator[](const int i);
    const std::vector<float>& operator[](const int i) const;
    Matrix operator*(const Matrix& a);
    Matrix transpose();
    Matrix inverse();

    friend std::ostream& operator<<(std::ostream& out, const Matrix& m);


};
#endif //__GEOMETRY_H__
#pragma once
#include <iostream>
#include <cmath>

template<class T> struct Vec2 {

	union {
		struct { T u, v; };
		struct { T x, y; };
		T raw[2];
	};

	Vec2() : u{0}, v{0}{}
	Vec2(T _u, T _v) : u{_u}, v{_v}{}

	inline Vec2<T> operator +(const Vec2<T>& V) const { return Vec2<T>{u + V.u, v + V.v}; }
	inline Vec2<T> operator -(const Vec2<T>& V) const { return Vec2<T>{u - V.u, v - V.v}; }
	inline Vec2<T> operator *(float t)					const { return Vec2<T>{u* t, v* t}; }
	template<class T> 
	friend std::ostream& operator<<(std::ostream& out, const Vec2<T>& v);
	

};
template <class T> struct Vec3 {
	union {
		struct { T x, y, z; };
		struct { T ivert, iuv, inorm; };
		T raw[3];
	};
	Vec3() : x{ 0 }, y{ 0 }, z{ 0 } {}
	Vec3(T _x, T _y, T _z) : x{ _x }, y{ _y }, z{ _z } {}
	// bu ne aq
	inline Vec3<T> operator ^(const Vec3<T>& v) const { return Vec3<T>{y* v.z - z * v.y, z* v.x - x * v.z, x* v.y - y * v.x}; }
	inline Vec3<T> operator +(const Vec3<T>& v) const { return Vec3<T>(x + v.x, y + v.y, z + v.z); }
	inline Vec3<T> operator -(const Vec3<T>& v) const { return Vec3<T>(x - v.x, y - v.y, z - v.z); }
	inline Vec3<T> operator *(float f)          const { return Vec3<T>(x * f, y * f, z * f); }
	inline T       operator *(const Vec3<T>& v) const { return x * v.x + y * v.y + z * v.z; }
	float norm() const { return std::sqrt(x * x + y * y + z * z); }
	Vec3<T>& normalize(int l = 1) { *this = *this * (l / norm()); return *this; }
	template<class T>
	friend std::ostream& operator<<(std::ostream& out, const Vec3<T>& vec);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec2<int> Vec3i;

template<class T>
std::ostream& operator<<(std::ostream& out, const Vec3<T>& vec) {
	out << "(" << vec.x << ", " << vec.y << ", "<<vec.z<< ")\n";
	return out;
}
template<class T>
std::ostream& operator<<(std::ostream& out, const Vec2<T>& vec) {
	out << "(" << vec.x << ", " << vec.y << ")\n";
	return out;
}

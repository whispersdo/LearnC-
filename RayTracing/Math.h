#pragma once
// math : vector & sphere
#include<iostream>
using namespace std;

template<class T>
T clamp(T a, T min = 0, T max = 1)
{
	return a > max ? max : a < min ? min : a;
}

template<typename T, int n>
class vec_T
{
public:
	T	v[n];

	vec_T() = default;

	vec_T(const vec_T& vec)
	{
		for (size_t i = 0; i < n; i++)
			v[i] = vec.v[i];
	}

	vec_T(T vec[])
	{
		for (size_t i = 0; i < n; i++)
			v[i] = vec[i];
	}

	vec_T(initializer_list<T> vec)
	{
		size_t i = 0;
		for (auto e : vec)
			v[i++] = e;
	}

	vec_T(T x, ...)
	{
		va_list args;
		__crt_va_start(args, n);

		for (size_t i = 0; i < n; i++)
			v[i] = __crt_va_arg(args, T);

		__crt_va_end(args);
	}

	inline vec_T operator - (const vec_T& v2) const
	{
		vec_T vr;
		for (size_t i = 0; i < n; i++)
			vr.v[i] = v[i] - v2.v[i];

		return vr;
	}
	inline vec_T operator + (const vec_T& v2) const
	{
		vec_T vr;
		for (size_t i = 0; i < n; i++)
			vr.v[i] = v[i] + v2.v[i];

		return vr;
	}
	inline vec_T operator * (const vec_T& v2) const
	{
		vec_T vr;
		for (size_t i = 0; i < n; i++)
			vr.v[i] = v[i] * v2.v[i];

		return vr;
	}
	inline vec_T operator * (const T& m) const
	{
		vec_T vr;
		for (size_t i = 0; i < n; i++)
			vr.v[i] = v[i] * m;

		return vr;
	}
	inline vec_T operator / (const T& m) const
	{
		vec_T vr;
		for (size_t i = 0; i < n; i++)
			vr.v[i] = v[i] / m;

		return vr;
	}

	inline T dot(const vec_T& v2) const
	{
		T d = 0;
		for (size_t i = 0; i < n; i++)
			d += v[i] * v2.v[i];

		return d;
	}

	T& x() { return v[0]; }
	T& y() { return v[1]; }
	T& z() { return v[2]; }
	T& w() { return v[3]; }
	const T& x() const { return v[0]; }
	const T& y() const { return v[1]; }
	const T& z() const { return v[2]; }
	const T& w() const { return v[3]; }
};

using vec2 = vec_T<float, 2>;
using vec3 = vec_T<float, 3>;
using vec4 = vec_T<float, 4>;

class color : public vec4
{
public:
	using vec4::vec4;
	float& r() { return v[0]; }
	float& g() { return v[1]; }
	float& b() { return v[2]; }
	float& a() { return v[3]; }

	const float& r() const { return v[0]; }
	const float& g() const { return v[1]; }
	const float& b() const { return v[2]; }
	const float& a() const { return v[3]; }

	operator uint32_t () const {
		return  ((uint8_t(clamp(v[3]) * 255) << 24) & 0xFF000000) |
				((uint8_t(clamp(v[0]) * 255) << 16) & 0x00FF0000) |
				((uint8_t(clamp(v[1]) * 255) << 8 ) & 0x0000FF00) |
				((uint8_t(clamp(v[2]) * 255)      ) & 0x000000FF) ;
	}

	color(uint32_t color)
	{
		v[3] = float((color & 0xFF000000) >> 24) / 255.f;
		v[0] = float((color & 0x00FF0000) >> 16) / 255.f;
		v[1] = float((color & 0x0000FF00) >> 8 ) / 255.f;
		v[2] = float((color & 0x000000FF)      ) / 255.f;
	}
};

template<class T, int n>
vec_T<T, n> lerp(const vec_T<T, n>& v1, const vec_T<T, n>& v2, T factor)
{
	return (v2 - v1) * factor + v1;
}

template<class T, int n>
T dot(const vec_T<T, n>& v1, const vec_T<T, n>& v2)
{
	return v1.dot(v2);
}

template<class T, int n>
T normalize(const vec_T<T, n>& v)
{
	return v / sqrt(v.dot(v));
}

inline vec3 cross(const vec3& v1, const vec3& v2)
{
	return vec3(v1.y() * v2.z() - v1.z() * v2.y(), v1.z() * v2.x() - v1.x() * v2.z(), v1.x() * v2.y() - v1.y() * v2.x());
}
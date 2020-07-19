#pragma once
// math : vector & sphere
#include<iostream>
#include<stdarg.h>

using namespace std;

#define PI 3.14159

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

	typedef typename conditional_t<is_integral_v<T>,
		conditional_t<sizeof(T) <= sizeof(int), int, T>,
		conditional_t<is_floating_point_v<T> && sizeof(T) <= sizeof(double), double, T>> UpType;

	// enable_if_t<is_integral_v<T>, int> x = 0;

	vec_T(T x, ...)
	{		
		va_list args;
		va_start(args, x);

		v[0] = x;
		for (size_t i = 1; i < n; i++)
			v[i] = T(va_arg(args, UpType));

		va_end(args);
	}

	vec_T operator - (const vec_T& v2) const
	{
		vec_T vr;
		for (size_t i = 0; i < n; i++)
			vr.v[i] = v[i] - v2.v[i];

		return vr;
	}
	vec_T operator + (const vec_T& v2) const
	{
		vec_T vr;
		for (size_t i = 0; i < n; i++)
			vr.v[i] = v[i] + v2.v[i];

		return vr;
	}
	vec_T operator * (const vec_T& v2) const
	{
		vec_T vr;
		for (size_t i = 0; i < n; i++)
			vr.v[i] = v[i] * v2.v[i];

		return vr;
	}
	vec_T operator * (const T& m) const
	{
		vec_T vr;
		for (size_t i = 0; i < n; i++)
			vr.v[i] = v[i] * m;

		return vr;
	}
	vec_T operator / (const T& m) const
	{
		vec_T vr;
		for (size_t i = 0; i < n; i++)
			vr.v[i] = v[i] / m;

		return vr;
	}

	T dot(const vec_T& v2) const
	{
		T d = 0;
		for (size_t i = 0; i < n; i++)
			d += v[i] * v2.v[i];

		return d;
	}

	bool operator == (const vec_T& v2) const
	{
		for (size_t i = 0; i < n; i++)
			if (v[i] != v2.v[i])
				return false;

		return true;
	}

	explicit operator uint32_t () const {
		uint32_t color = n>3 ? 0 : 0xFF000000;
		static const uint32_t bits[] = { 16,8,0,24 }; // ARGB

		for (size_t i = 0; i < n; i++)
			color |= (uint8_t(clamp(v[i]) * 255.f) & 0xFF) << bits[i];

		return color;
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
vec_T<T, n> normalize(const vec_T<T, n>& v)
{
	return v / sqrt(v.dot(v));
}

vec3 cross(const vec3& v1, const vec3& v2)
{
	return vec3(v1.y() * v2.z() - v1.z() * v2.y(), v1.z() * v2.x() - v1.x() * v2.z(), v1.x() * v2.y() - v1.y() * v2.x());
}

vec3 reflect(const vec3& I, const vec3& N)
{
	return I - N * 2 * dot(I, N);
}

vec3 refract(const vec3& I, const vec3& N, float eta)
{
	float cosi = - dot(I, N);
	float cost2 = 1 - eta * eta * (1 - cosi * cosi);
	vec3 t = I * eta + N * (eta * cosi - sqrt(cost2));
	return t * float(cost2 > 0 ? 1 : -1);
}

float rand01()
{
	return (rand() & 0xFFFF) / 0xFFFF;
}

vec3 randdir(const vec3& nor, float r)
{
	float x = rand01() * 2 - 1;
	float y = rand01() * 2 - 1;
	float z = rand01();

	return vec3(nor);
}
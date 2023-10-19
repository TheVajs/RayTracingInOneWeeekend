#ifndef VEC3_H
#define VEC3_H

#include "utility.h"

#include <cmath>
#include <iostream>

using std::sqrt;

class vec3
{
public:
	double e[3];

	vec3() : e{ 0, 0, 0 } {}
	vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

	double x() const { return e[0]; }
	double y() const { return e[1]; }
	double z() const { return e[2]; }

	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
	double operator[](int i) const { return e[i]; }
	double& operator[](int i) { return e[i]; }

	vec3& operator+=(const vec3& v)
	{
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];
		return *this;
	}

	vec3& operator*=(double t)
	{
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		return *this;
	}

	vec3& operator/=(double t)
	{
		return *this *= 1 / t;
	}

	double length() const
	{
		return sqrt(length_squared());
	}

	double length_squared() const
	{
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

	static vec3 random()
	{
		return vec3(random_double(), random_double(), random_double());
	}

	static vec3 random(double min, double max)
	{
		return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}

	static vec3 lerp(const vec3& u, const vec3& v, double t)
	{
		return vec3(
			lerp_double(u.x(), v.x(), t),
			lerp_double(u.y(), v.y(), t),
			lerp_double(u.z(), v.z(), t)
		);
	}

	bool near_zero() const
	{
		// Return true if the vector is close to zero in all dimensions.
		auto s = 1e-8;
		return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
	}
};

// Utility functions

inline std::ostream& operator<<(std::ostream& out, const vec3& v)
{
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v)
{
	return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v)
{
	return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v)
{
	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3& v)
{
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3& v, double t)
{
	return t * v;
}

inline vec3 operator/(vec3 v, double t)
{
	return (1 / t) * v;
}

inline double dot(const vec3& u, const vec3& v)
{
	return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v)
{
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
		u.e[2] * v.e[0] - u.e[0] * v.e[2],
		u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 normalize(vec3 v)
{
	return v / v.length();
}

inline vec3 random_in_unit_sphere()
{
	while (true)
	{
		vec3 r = vec3::random(-1.0, 1.0);
		if (r.length_squared() < 1.0)
			return r;
	}
}

inline vec3 random_in_unit_disk()
{
	while(true)
	{
		vec3 r = vec3(random_double(-1, 1), random_double(-1, 1), 0);
		if (r.length_squared() < 1.0)
			return r;
	}
}

inline vec3 random_unit_vector()
{
	return normalize(random_in_unit_sphere());
}

inline vec3 random_on_hemisphere(const vec3& normal)
{
	auto r = random_unit_vector();
	return dot(normal, r) < 0 ? -r : r;
}

inline vec3 reflect(const vec3& v, const vec3& n)
{
	return v - 2 * dot(v, n) * n;
}

inline vec3 refract(const vec3& v, const vec3& n, double etai_over_etat)
{
	double cos_theta = fmin(dot(-v, n), 1.0);
	auto r_perp = etai_over_etat * (v + cos_theta * n);
	auto r_parallel = -sqrt(fabs(1.0 - r_perp.length_squared())) * n;
	return r_perp + r_parallel;
}

#endif
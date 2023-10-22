#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"

#include <cmath>

// Quadrilaterals - QUAD
class quad : public hittable
{
public:
	quad(const vec3& _Q, const vec3& _u, const vec3& _v, shared_ptr<material> m)
		: Q(_Q), u(_u), v(_v), mat(m)
	{
		vec3 n = cross(u, v);
		normal = normalize(n);
		D = dot(normal, Q);

		w = n / dot(n, n);
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		auto denom = dot(normal, r.direction());

		// No hit if the ray is parallel to the plane.
		if (fabs(denom) < 1e-8)
			return false;

		// Return false if the hit point parameter t is outside the ray interval.
		auto t = (D - dot(normal, r.origin())) / denom;
		if (!ray_t.contains(t))
			return false;

		auto intersection = r.at(t);

		// Determine the hit point lies within the planar shape using its plane coordinates.
		vec3 planar_hitpt_vector = intersection - Q;
		auto alpha = dot(w, cross(planar_hitpt_vector, v));
		auto beta = dot(w, cross(u, planar_hitpt_vector));

		if (!is_interior(alpha, beta, rec))
			return false;

		// Ray hits the 2D shape; set the rest of the hit record and return true.

		rec.t = t;
		rec.pos = intersection;
		rec.mat = mat;
		rec.set_normal(r, normal);

		return true;
	}

	virtual bool is_interior(double a, double b, hit_record& rec) const
	{
		// Given the hit point in plane coordinates, return false if it is outside the
		// primitive, otherwise set the hit record UV coordinates and return true.

		if ((a < 0) || (1 < a) || (b < 0) || (1 < b))
			return false;

		// rec.u = a;
		// rec.v = b;
		return true;
	}

private:
	vec3 Q;
	vec3 u, v;
	vec3 normal;
	shared_ptr<material> mat;
	double D;
	vec3 w;
};

#endif
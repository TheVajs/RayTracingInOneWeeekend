#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include "utility.h"

class material;

struct hit_record
{
public:
	vec3 pos;
	vec3 normal;
	shared_ptr<material> mat;
	double t;
	bool front_face;

	void set_normal(const ray& r, const vec3& outward_normal)
	{
		// Two things are assumed. Outward normal is pointing out of the surface and
		// the normal vector is unit length.

		front_face = dot(r.direction(), outward_normal) < .0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class hittable
{
public:
	virtual ~hittable() = default;

	virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif
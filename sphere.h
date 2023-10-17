#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "utility.h"

class sphere : public hittable
{
public:
	sphere(vec3 _center, double _radius, shared_ptr<material> _material) : 
		center(_center), radius(_radius), mat(_material) {}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		vec3 oc = r.origin() - center;
		auto a = r.dir().length_squared();
		auto half_b = dot(oc, r.dir());
		auto c = oc.length_squared() - radius * radius;

		auto discriminant = half_b * half_b - a * c;
		if (discriminant < 0)
			return false;

		auto sqrtd = sqrt(discriminant);

		// Find the nearest root that lies in the acceptable range.
		auto root = (-half_b - sqrtd) / a;
		if (!ray_t.surrounds(root))
		{
			root = (-half_b + sqrtd) / a;
			if (!ray_t.surrounds(root))
				return false;
		}

		rec.t = root;
		rec.pos = r.at(root);
		rec.set_normal(r, (rec.pos - center) / radius);
		rec.mat = mat;

		return true;
	}

private:
	vec3 center;
	double radius;
	shared_ptr<material> mat;
};

#endif
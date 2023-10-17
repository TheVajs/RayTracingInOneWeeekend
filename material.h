#ifndef MATERIAL_H
#define MATERIAL_H

#include "utility.h"

class hit_record;

class material
{
public:
	virtual ~material() = default;

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
};

class lambertian : public material
{
public:
	lambertian(const color& a) : albedo(a) {}

	bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		auto scattered_dir = rec.normal + random_on_unit_sphere();
		if (scattered_dir.near_zero())
			scattered_dir = rec.normal;

		scattered = ray(rec.pos, scattered_dir);
		attenuation = albedo;
		return true;
	}

private:
	color albedo;
};

class metal : public material
{
public:
	metal(const color& a) : albedo(a) {}

	bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		auto reflected_dir = reflect(normalize(r_in.dir()), rec.normal);
		scattered = ray(rec.pos, reflected_dir);
		attenuation = albedo;
		return true;
	}

private:
	color albedo;
};

#endif
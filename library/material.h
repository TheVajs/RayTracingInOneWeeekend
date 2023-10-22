#ifndef MATERIAL_H
#define MATERIAL_H

#include "utility.h"

class hit_record;

class material
{
public:
	virtual ~material() = default;

	virtual color emitted(double u, double v, const vec3& p) const
	{
		return color(0, 0, 0);
	}

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
		auto scattered_dir = rec.normal + random_unit_vector();
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
	metal(const color& a, double _fuzz) : albedo(a), fuzz(_fuzz < 1 ? _fuzz : 1) {}

	bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		auto dir = normalize(r_in.direction());
		auto reflected = reflect(dir, rec.normal) + fuzz * random_unit_vector();
		scattered = ray(rec.pos, reflected);
		attenuation = albedo;

		return dot(scattered.direction(), rec.normal) > 0;
	}

private:
	color albedo;
	double fuzz;
};

class dielectric : public material
{
public:
	dielectric(double index_of_refraction) : ir(index_of_refraction) {}

	bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		auto dir = normalize(r_in.direction());
		double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

		double cos_theta = fmin(dot(-dir, rec.normal), 1.0);
		double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

		bool cannot_refrect = refraction_ratio * sin_theta > 1.0;
		if (cannot_refrect || reflectance(cos_theta, refraction_ratio) > random_double())
		{
			dir = reflect(dir, rec.normal);
		}
		else
		{
			dir = refract(dir, rec.normal, refraction_ratio);
		}

		scattered = ray(rec.pos, dir);
		attenuation = color(1.0, 1.0, 1.0);

		return true;
	}

private:
	double ir;

	// Schlick approximation! Provides variation with angle.
	static double reflectance(double cosine, double ref_idx)
	{
		double r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow(1 - cosine, 5);
	}
};

class diffuse_light : public material
{
public:
	diffuse_light(const color& a) : emit(a) {}

	// TODO add for texture

	bool scatter(const ray& r_in, const hit_record& rec, color& atenuation, ray& scatter) const override
	{
		return false;
	}

	color emitted(double u, double v, const vec3& p) const override
	{
		return emit;
	}

private:
	color emit;
};

#endif
#include <iostream>
#include <cmath>
#include <cstdlib>

using std::sqrt;

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385;

float degress_to_radians(float degress)
{
	return degress * (pi / 180);
}

inline double random_double()
{
	return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max)
{
	return min + (max - min) * random_double();
}

class vec3
{
public:
	float e[3];

	vec3() : e{ 0, 0, 0 } {}
	vec3(float _x, float _y, float _z) : e{ _x, _y, _z } {}

	float x() const { return e[0]; }
	float y() const { return e[1]; }
	float z() const { return e[2]; }

	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
	float operator[](int i) const { return e[i]; }
	float& operator[](int i) { return e[i]; }

	vec3& operator+=(const vec3& v)
	{
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];
		return *this;
	}

	vec3& operator-=(const vec3& v)
	{
		e[0] -= v.e[0];
		e[1] -= v.e[1];
		e[2] -= v.e[2];
		return *this;
	}

	vec3& operator*=(float t)
	{
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		return *this;
	}

	float length_squared() const
	{
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

	float length() const
	{
		return sqrt(length_squared());
	}

};

using color = vec3;

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

inline vec3 operator*(float t, const vec3& u)
{
	return vec3(t * u.e[0], t * u.e[1], t * u.e[2]);
}

inline vec3 operator*(const vec3& u, float t)
{
	return t * u;
}

inline vec3 operator/(const vec3& u, float t)
{
	return (1 / t) * u;
}

inline vec3 normalize(vec3 u)
{
	return u / u.length();
}

inline float dot(const vec3& u, const vec3& v)
{
	return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

class ray
{
public:
	ray() {}
	ray(const vec3& origin, const vec3& direction) : org(origin), dir(direction) {}

	vec3 origin() const { return org; }
	vec3 direction() const { return dir; }

	vec3 at(float t) const
	{
		return org + t * dir;
	}

private:
	vec3 org;
	vec3 dir;
};

struct hit_record
{
public:
	vec3 pos;
	vec3 normal;
	float t0;
	float t1;
};

class sphere
{
public:
	vec3 center;
	color scatter;
	float radius;
	float sigma_a;

	sphere(vec3 _center, color _scatter, float r, float sigma) : center(_center), scatter(_scatter), radius(r), sigma_a(sigma) {}

	bool hit(const ray& r, hit_record& rec) const
	{
		// P(t) = S + tV 
		// The line equation need to be relative to the position of the sphere. So
		// S is calculated by 'ray_origin - center_sphere'

		auto oc = r.origin() - center;
		float a = r.direction().length_squared();
		float half_b = dot(oc, r.direction());
		float c = oc.length_squared() - radius * radius;

		float d = half_b * half_b - a * c;
		if (d < 0)
			return false;

		float sqrtd = sqrt(d);
		float t0 = (-half_b - sqrtd) / a;
		float t1 = (-half_b + sqrtd) / a;

		float root = t0;
		if (root < 0.01 || root > infinity)
		{
			root = t1;
			if (root < 0.01 || root > infinity)
			{
				return false;
			}
		}

		rec.pos = r.at(root);
		rec.t0 = t0;
		rec.t1 = t1;

		return true;
	}
};

vec3 center = vec3(0, 0, 2);
vec3 loc00, pixel_delta_u, pixel_delta_v;

float aspect_ratio = 16.0 / 9.0;
int image_width = 400;
int image_height = image_width / aspect_ratio;

void initialize_camera()
{
	float focal_length = 1;
	float viewport_height = 2;
	float viewport_width = viewport_height * (static_cast<float>(image_width) / image_height);

	auto viewport_u = vec3(viewport_width, 0, 0);
	auto viewport_v = -vec3(0, viewport_height, 0);
	pixel_delta_u = viewport_u / image_width;
	pixel_delta_v = viewport_v / image_height;

	auto viewport_top_left = center - vec3(0, 0, focal_length) - .5 * (viewport_u + viewport_v);
	loc00 = viewport_top_left + .5 * (pixel_delta_u + pixel_delta_v);
}

float clamp(float t)
{
	return t > 1.0 ? 1.0 : t < 0.0 ? 0.0 : t;
}

void write_color(std::ostream& out, color pixel_color)
{
	float r = pixel_color.x(), g = pixel_color.y(), b = pixel_color.z();
	out << static_cast<int>(255.999 * clamp(r)) << ' '
		<< static_cast<int>(255.999 * clamp(g)) << ' '
		<< static_cast<int>(255.999 * clamp(b)) << '\n';
}

ray get_ray(int i, int j)
{
	auto ray_target = loc00 + (pixel_delta_u * i) + (pixel_delta_v * j);

	auto dx = (random_double() - .5);
	auto dy = (random_double() - .5);
	ray_target += pixel_delta_u * dx + pixel_delta_v * dy;

	auto pixel_sample = ray_target - center;
	return ray(center, pixel_sample);
}

color backward_ray_marching(const ray& r, const hit_record& rec,
	const sphere& sphere, const color& background_color)
{
	float step_size = 0.1;
	float sigma_a = 1.5;
	float inside_length = (rec.t1 - rec.t0);
	int ns = std::ceil(inside_length / step_size);
	step_size = inside_length / ns;

	vec3 light_dir(0, 1, 0);
	vec3 light_color(1.3, 0.3, 0.9);

	float transparency = 1;
	color result(0, 0, 0);

	hit_record rec1;

	for (int n = 0; n < ns; ++n)
	{
		float t = rec.t1 - step_size * (n + .5);
		vec3 sample_pos = r.at(t);
		sample_pos -= light_dir * .01;

		// Compute sample transparency with Beer's law. 

		float sample_transparency = exp(-step_size * sigma_a);
		transparency *= sample_transparency;

		// In-scattering. Find the distance traveled by light though
		// the volume to our sample point. Then apply Beer's law.

		if (sphere.hit(ray(sample_pos, light_dir), rec1))
		{
			float light_attenuation = exp(-rec1.t1 * sigma_a);
			result += light_color * light_attenuation * step_size;
		}

		// Finally attenuate the result by sample transparency.
		result *= sample_transparency;
	}

	return background_color * transparency + result;
}

color forward_ray_marching(const ray& r, const hit_record& rec,
	const sphere& sphere, const color& background_color)
{
	float step_size = 0.1;
	float sigma_a = 1.5;
	float inside_length = (rec.t1 - rec.t0);
	int ns = std::ceil(inside_length / step_size);
	step_size = inside_length / ns;

	vec3 light_dir(0, 1, 0);
	vec3 light_color(1.3, 0.3, 0.9);

	float transparency = 1;
	color result(0, 0, 0);

	hit_record rec1;

	for (int n = 0; n < ns; ++n)
	{
		float t = rec.t0 + step_size * (n + .5);

		vec3 sample_pos = r.at(t);
		sample_pos -= light_dir * .01;

		// Compute sample transparency with Beer's law. 

		float sample_attenuation = exp(-step_size * sigma_a);
		transparency *= sample_attenuation;

		// In-scattering. Find the distance traveled by light though
		// the volume to our sample point. Then apply Beer's law.

		if (sphere.hit(ray(sample_pos, light_dir), rec1))
		{
			float light_attenuation = exp(-rec1.t1 * sigma_a);
			result += transparency * light_color * light_attenuation * step_size;
		}
	}

	return background_color * transparency + result;
}

color pixel_color(const ray& r, const sphere& sphere)
{
	auto background_color = color(0.572, 0.772, 0.921);

	hit_record rec;
	if (!sphere.hit(r, rec))
		return background_color;

	// This is a simple demonstration of Beer's Law.

	// auto p1 = r.at(rec.t0);
	// auto p2 = r.at(rec.t1);
	// float distance = (p2 - p1).length();
	// float t = exp(-distance * sphere.sigma_a);
	// return t * background_color + (1 - t) * vec3(0,0,0); //sphere.scatter;

	// Backward ray marching with uniform steps

	// return backward_ray_marching(r, rec, sphere, background_color);

	// Forward ray marching with uniform steps

	return forward_ray_marching(r, rec, sphere, background_color);
}

int main()
{
	sphere obj(vec3(0, 0, 0), color(0.8, 0.1, 0.5), 1.3, 1);

	int rays_per_sample = 50;
	initialize_camera();

	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = 0; j < image_height; j++)
	{
		std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;

		for (int i = 0; i < image_width; i++)
		{
			color result(0, 0, 0);

			for (int n = 0; n < rays_per_sample; n++)
			{
				auto r = get_ray(i, j);
				result += pixel_color(r, obj);
			}

			write_color(std::cout, result / rays_per_sample);
		}
	}

	std::clog << "\rDone.                  \n" << std::endl;
}
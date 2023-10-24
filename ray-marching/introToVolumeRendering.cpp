#include <iostream>

#include <cmath>
#include <cstdlib>
#include <algorithm>

using std::sqrt;

const float infinity = std::numeric_limits<float>::infinity();

float degress_to_radians(float degress)
{
	return degress * (M_PI / 180);
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

inline vec3 normalize(const vec3& u)
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
	float radius;

	sphere(vec3 _center, float r) : center(_center), radius(r) {}

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
		float q = (d > 0) ? (-half_b - sqrtd) : (-half_b + sqrtd);
		float t0 = q / a;
		float t1 = c / q;
		
		float root = t0;
		if (root < 0.01 || root > infinity)
		{
			root = t1;
			if (root < 0.01 || root > infinity)
			{
				return false;
			}
		}

		rec.pos = r.at(t0);
		rec.normal = normalize(rec.pos - center);
		rec.t0 = t0;
		rec.t1 = t1;

		return true;
	}
};

void write_color(std::ostream& out, color pixel_color)
{
	float r = pixel_color.x(), g = pixel_color.y(), b = pixel_color.z();
	out << static_cast<int>(255.999 * std::clamp(r, 0.0f, 1.0f)) << ' '
		<< static_cast<int>(255.999 * std::clamp(g, 0.0f, 1.0f)) << ' '
		<< static_cast<int>(255.999 * std::clamp(b, 0.0f, 1.0f)) << '\n';
}


// Ray marching

// The Henyey-Greenstein phase function
float phase(const float& g, const float& cos_theta)
{
	float denom = 1 + g * g - 2 * g * cos_theta;
	return 1 / (4 * M_PI) * (1 - g * g) / (denom * sqrtf(denom));
}

color pixel_color(const ray& r, const sphere& sphere)
{
	auto background_color = color(0.572, 0.772, 0.921);

	hit_record rec;
	if (!sphere.hit(r, rec))
		return background_color;

	float step_size = 0.1;
	float absorption = .5; // sigma_a
	float scattering = .5;	// sigma_s
	float density = .25;
	float g = 0;
	int d = 2; // russian roulette "probability"

	int ns = std::ceil((rec.t1 - rec.t0) / step_size);
	step_size = (rec.t1 - rec.t0) / ns;

	auto light_dir = vec3(-1, 0, 0);
	auto ray_dir = r.direction();
	auto light_color = vec3(13, 13, 13);

	float transparency = 1;
	color result(0, 0, 0);

#if defined(SIMPLE_DEMOSTRATION)
	// This is a simple demonstration of Beer's Law.
	auto p1 = r.at(rec.t0);
	auto p2 = r.at(rec.t1);
	float distance = (p2 - p1).length();
	float t = exp(-distance * sphere.absorption);
	return t * background_color + (1 - t) * vec3(0, 0, 0); //sphere.scatter;
#elif defined(BACKWARD_RAYMARCHING)
	for (int n = 0; n < ns; ++n)
	{
		float t = rec.t1 - step_size * (n + .5);
		vec3 sample_pos = r.at(t);
		sample_pos -= light_dir * .01;

		// Compute sample transparency with Beer's law. 

		float sample_transparency = exp(-step_size * absorption);
		transparency *= sample_transparency;

		// In-scattering. Find the distance traveled by light though
		// the volume to our sample point. Then apply Beer's law to 
		// attenuate the light contrution due to in-scattering.

		hit_record rec1;
		if (sphere.hit(ray(sample_pos, light_dir), rec1))
		{
			float light_attenuation = exp(-rec1.t1 * absorption);
			result += light_color * light_attenuation * step_size;
		}

		// Finally attenuate the result by sample transparency.
		result *= sample_transparency;
	}
#else
	for (int n = 0; n < ns; ++n)
	{
		float t = rec.t0 + step_size * (n + random_double());
		vec3 sample_pos = r.at(t) - light_dir * 1e-2;

		// Compute sample transparency with Beer's law. 

		float sample_attenuation = exp(-density * step_size * (absorption + scattering));
		transparency *= sample_attenuation;

		// In-scattering. Find the distance traveled by light though
		// the volume to our sample point. Then apply Beer's law.

		hit_record rec1;
		if (sphere.hit(ray(sample_pos, light_dir), rec1))
		{
			float light_attenuation = exp(-density * rec1.t1 * (absorption + scattering));
			float cos_theta = dot(ray_dir, light_dir);
			result += light_attenuation * light_color * density * scattering * 
				phase(g, cos_theta) * transparency * step_size;
		}

		// Russian roulette
		if (transparency < 1e-3)
		{
			if (random_double() > 1.f / d)
				break;
			else
				transparency *= d;
		}
	}
#endif

	return background_color * transparency + result;
}

int rays_per_sample = 1;
int width = 640;
int height = 480;
float fov = 45;

int main()
{
	sphere obj(vec3(0, 0, -20), 5);

	auto center = vec3(0, 0, 0);
	float aspect_ratio = width / static_cast<float>(height);

	float focal = tan(M_PI / 180 * fov * 0.5);
	float viewport_width = 2 * focal;
	float viewport_height = 2 * focal * 1 / aspect_ratio;

	auto viewport_u = vec3(viewport_width, 0, 0);
	auto viewport_v = -vec3(0, viewport_height, 0);
	auto pixel_delta_u = viewport_u / width;
	auto pixel_delta_v = viewport_v / height;

	auto viewport_top_left = center - vec3(0, 0, 1) - .5 * (viewport_u + viewport_v);
	auto loc00 = viewport_top_left + .5 * (pixel_delta_u + pixel_delta_v);

	std::cout << "P3\n" << width << ' ' << height << "\n255\n";

	for (int j = 0; j < height; j++)
	{
		std::clog << "\rScanlines remaining: " << (height - j) << ' ' << std::flush;

		for (int i = 0; i < width; i++)
		{
			color result(0, 0, 0);

			for (int n = 0; n < rays_per_sample; n++)
			{
				// vec3 ray_dir;
				// ray_dir[0] = (2.f * (i + 0.5f) / width - 1) * focal;
				// ray_dir[1] = (1 - 2.f * (j + 0.5f) / height) * focal * 1 / aspect_ratio; // Maya style
				// ray_dir[2] = -1.f;

				auto ray_target = loc00 + (pixel_delta_u * i) + (pixel_delta_v * j);
				vec3 ray_dir = ray_target - center;

				result += pixel_color(ray(center, normalize(ray_dir)), obj);
			}

			write_color(std::cout, result / rays_per_sample);
		}
	}

	std::clog << "\rDone.                  \n" << std::endl;
}
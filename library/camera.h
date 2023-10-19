#ifndef CAMERA_H
#define CAMERA_H

#include "utility.h"

#include "color.h"
#include "hittable.h"
#include "material.h"

#include <iostream>

class camera
{
public:
	double aspect_ratio = 1;
	int image_width = 100;
	int samples_per_pixel = 10;
	double fov = 90.0;
	double gamma = 1;
	double defocus_angle = 0;
	double focus_distance = 0;
	int max_depth = 10;

	vec3 look_from = vec3(0, 0, -1.0);
	vec3 look_at = vec3(0, 0, 0);
	vec3 vup = vec3(0, 1, 0);

	void render(const hittable& world)
	{
		initialize();

		std::cout << "P3\n"
			<< image_width << ' ' << image_height << "\n255\n";

		for (int j = 0; j < image_height; j++)
		{
			std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;

			for (int i = 0; i < image_width; i++)
			{
				color pixel_color(0, 0, 0);
				for (int n = 0; n < samples_per_pixel; n++)
				{
					ray r = get_ray(i, j);
					pixel_color += ray_color(r, max_depth, world);
				}

				write_color(std::cout, linear_to_gamma(pixel_color / samples_per_pixel, gamma));
			}
		}

		std::clog << "\rDone.                 \n";
	}

private:
	int image_height;
	vec3 pixel_start_loc;
	vec3 pixel_delta_u;
	vec3 pixel_delta_v;
	vec3 center;
	vec3 w, u, v;
	vec3 defocus_disk_u, defocus_disk_v;

	void initialize()
	{
		image_height = static_cast<int>(image_width * 1 / aspect_ratio);
		image_height = (image_height < 1) ? 1 : image_height;

		bool blur = defocus_angle <= 0;

		center = look_from;
		w = (look_from - look_at);
		focus_distance = blur ? w.length() : focus_distance;

		auto theta = degrees_to_radians(fov);
		auto h = focus_distance * tan(theta * .5);
		auto viewport_height = 2.0 * h;
		auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

		w = normalize(w);
		u = normalize(cross(vup, w));
		v = cross(w, u);

		auto viewport_u = u * viewport_width;
		auto viewport_v = -v * viewport_height;

		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		auto viewport_upper_left = center - (focus_distance * w) - .5 * (viewport_u + viewport_v);
		pixel_start_loc = viewport_upper_left + .5 * (pixel_delta_u + pixel_delta_v);

		auto focus_radius = focus_distance * tan(degrees_to_radians(defocus_angle) * .5);
		defocus_disk_u = focus_radius * u;
		defocus_disk_v = focus_radius * v;

		center = blur ? center : defocus_disk_sample();
	}

	ray get_ray(int i, int j) const
	{
		auto pixel_center = pixel_start_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
		auto pixel_sample = pixel_center + pixel_sample_square();

		auto ray_direction = pixel_sample - center;
		return ray(center, ray_direction);
	}

	vec3 defocus_disk_sample() const
	{
		auto d = random_in_unit_disk();
		return center + d[0] * defocus_disk_u + d[1] * defocus_disk_v;
	}

	vec3 pixel_sample_square() const
	{
		double dx = random_double() - .5;
		double dy = random_double() - .5;
		return (dx * pixel_delta_u) + (dy * pixel_delta_v);
	}

	color ray_color(const ray& r, int depth, const hittable& world) const
	{
		hit_record rec;

		if (depth <= 0)
			return color(0, 0, 0);

		if (world.hit(r, interval(0.001, infinity), rec))
		{
			ray scattered;
			color attenuation;

			if (rec.mat->scatter(r, rec, attenuation, scattered))
				return attenuation; // * ray_color(scattered, depth - 1, world);

			return color(0, 0, 0);
		}

		auto dir = normalize(r.direction());
		auto a = .5 * (dir.y() + 1.0);
		return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(.5, .7, 1);
	}
};

#endif
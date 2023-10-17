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
	int max_depth = 10;

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

				write_color(std::cout, linear_to_gamma(pixel_color / samples_per_pixel));

				// color pixel_color(0, 0, 0);
				// for (int n = 0; n < samples_per_pixel; n++)
				// {
				// 	ray r = get_ray(i, j);
				// 	pixel_color = vec3::lerp(pixel_color, ray_color(r, max_depth, world), 1 / (n + 1.0));
				// }

				// write_color(std::cout, linear_to_gamma(pixel_color));
			}
		}

		std::clog << "\rDone.                 \n";
	}

private:
	int image_height;
	vec3 camera_center;
	vec3 pixel_start_loc;
	vec3 pixel_delta_u;
	vec3 pixel_delta_v;

	void initialize()
	{
		image_height = static_cast<int>(image_width * 1 / aspect_ratio);
		image_height = (image_height < 1) ? 1 : image_height;

		auto focal_length = 1.0;
		auto viewport_height = 2.0;
		auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);
		camera_center = vec3(0, 0, 0);

		// Horizontal and vertical screen vectors

		auto viewport_u = vec3(viewport_width, 0, 0);
		auto viewport_v = vec3(0, -viewport_height, 0);

		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		auto viewport_upper_left = camera_center -
			vec3(0, 0, focal_length) - viewport_u * .5 - viewport_v * .5;
		pixel_start_loc = viewport_upper_left + .5 * (pixel_delta_u + pixel_delta_v);
	}

	ray get_ray(int i, int j) const
	{
		auto pixel_center = pixel_start_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
		auto pixel_sample = pixel_center + pixel_sample_square();

		auto ray_origin = camera_center;
		auto ray_direction = pixel_sample - ray_origin;

		return ray(ray_origin, ray_direction);
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
                return attenuation * ray_color(scattered, depth-1, world);

            return color(0,0,0);

			// auto dir = random_on_hemisphere(rec.normal);
			// auto dir = rec.normal + random_on_unit_sphere(); // Lambertian
			// return 0.5 * ray_color(ray(rec.pos, dir), depth - 1, world);

			// To visualize normals
			// return 0.5 * (rec.normal + color(1.0, 1.0, 1.0));
		}

		auto dir = normalize(r.dir());
		auto a = .5 * (dir.y() + 1.0);
		return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(.5, .7, 1);
	}
};

#endif
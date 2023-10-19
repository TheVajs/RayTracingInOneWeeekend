#include <iostream>
#include <chrono>

#include "library/utility.h"

#include "library/camera.h"
#include "library/color.h"
#include "library/hittableList.h"
#include "library/material.h"
#include "library/sphere.h"

int main()
{
	hittable_list world;

	auto material_ground = make_shared<lambertian>(color(0.5, 0.5, 0.5));
	auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
	auto material_left = make_shared<dielectric>(3);
	auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

	world.add(make_shared<sphere>(vec3(0.0, -100.5, -1.0), 100.0, material_ground));
	world.add(make_shared<sphere>(vec3(0.0, 0.0, -1.0), 0.5, material_center));
	world.add(make_shared<sphere>(vec3(-1.0, 0.0, -1.0), 0.5, material_left));
	world.add(make_shared<sphere>(vec3(1.0, 0.0, -1.0), 0.5, material_right));

	camera cam;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 50;
	cam.max_depth = 50;
	cam.fov = 90.0;

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	cam.render(world);

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::clog << "Time difference = "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
		<< " ms" << std::endl;
}
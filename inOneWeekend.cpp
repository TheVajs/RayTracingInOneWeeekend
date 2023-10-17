#include "utility.h"

#include "camera.h"
#include "color.h"
#include "hittableList.h"
#include "material.h"
#include "sphere.h"

#include <iostream>
#include <chrono>

int main()
{
	hittable_list world;

	auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
	auto material_left = make_shared<metal>(color(0.8, 0.8, 0.8));
	auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2));

	world.add(make_shared<sphere>(vec3(0.0, -100.5, -1.0), 100.0, material_ground));
	world.add(make_shared<sphere>(vec3(0.0, 0.0, -1.0), 0.5, material_center));
	world.add(make_shared<sphere>(vec3(-1.0, 0.0, -1.0), 0.5, material_left));
	world.add(make_shared<sphere>(vec3(1.0, 0.0, -1.0), 0.5, material_right));

	camera cam;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 50;
	cam.max_depth = 50;

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	cam.render(world);

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::clog << "Time difference = "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
		<< " ms" << std::endl;
}
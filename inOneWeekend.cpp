#include <iostream>
#include <chrono>

#include "library/utility.h"

#include "library/camera.h"
#include "library/color.h"
#include "library/hittableList.h"
#include "library/material.h"
#include "library/sphere.h"

void scene1(camera& cam, hittable_list& world)
{
	auto material_ground = make_shared<lambertian>(color(0.1, 0.15, 0.2));
	auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
	auto material_left = make_shared<dielectric>(3);
	auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

	world.add(make_shared<sphere>(vec3(0.0, -100.5, -1.0), 100.0, material_ground));
	world.add(make_shared<sphere>(vec3(0.0, 0.0, -1.0), 0.5, material_center));
	world.add(make_shared<sphere>(vec3(-1.0, 0.0, -1.0), 0.5, material_left));
	world.add(make_shared<sphere>(vec3(1.0, 0.0, -1.0), 0.5, material_right));

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;
	cam.gamma = 2;

	cam.fov = 30;
	cam.look_from = vec3(-2, 2, 1);
	cam.look_at = vec3(0, 0, -1);
	// cam.defocus_angle = 10;
	// cam.focus_distance = 3.4;
}

int main()
{
	camera cam;
	hittable_list world;

	scene1(cam, world);

	// Time point.
	auto begin = std::chrono::steady_clock::now();

	cam.render(world);

	auto end = std::chrono::steady_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
	std::clog << "Duration = " << time << " ms" << std::endl;
}
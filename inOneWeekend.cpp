#include <iostream>
#include <chrono>

#include "library/utility.h"

#include "library/camera.h"
#include "library/color.h"
#include "library/hittableList.h"
#include "library/material.h"
#include "library/sphere.h"
#include "library/quad.h"

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
	cam.background = color(0.70, 0.80, 1.00);
	cam.gamma = 2;

	cam.fov = 30;
	cam.look_from = vec3(-2, 2, 1);
	cam.look_at = vec3(0, 0, -1);

	cam.defocus_angle = 0;
}

void scene_quads(camera& cam, hittable_list& world)
{
	// Materials
	auto left_red = make_shared<lambertian>(color(1.0, 0.2, 0.2));
	auto back_green = make_shared<lambertian>(color(0.2, 1.0, 0.2));
	auto right_blue = make_shared<lambertian>(color(0.2, 0.2, 1.0));
	auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
	auto lower_teal = make_shared<lambertian>(color(0.2, 0.8, 0.8));

	// Quads
	world.add(make_shared<quad>(vec3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0), left_red));
	world.add(make_shared<quad>(vec3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
	world.add(make_shared<quad>(vec3(3, -2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
	world.add(make_shared<quad>(vec3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
	world.add(make_shared<quad>(vec3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0, -4), lower_teal));

	cam.aspect_ratio = 1.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;
	cam.background = color(0.70, 0.80, 1.00);
	cam.gamma = 2.2;

	cam.fov = 80;
	cam.look_from = vec3(0, 0, 9);
	cam.look_at = vec3(0, 0, 0);
	cam.vup = vec3(0, 1, 0);

	cam.defocus_angle = 0;
}

void cornell_box(camera& cam, hittable_list& world)
{
	auto red = make_shared<lambertian>(color(.65, .05, .05));
	auto white = make_shared<lambertian>(color(.73, .73, .73));
	auto green = make_shared<lambertian>(color(.12, .45, .15));
	auto light = make_shared<diffuse_light>(color(15, 15, 15));

	world.add(make_shared<quad>(vec3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
	world.add(make_shared<quad>(vec3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
	world.add(make_shared<quad>(vec3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));
	world.add(make_shared<quad>(vec3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
	world.add(make_shared<quad>(vec3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
	world.add(make_shared<quad>(vec3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

	cam.aspect_ratio = 1.0;
	cam.image_width = 600;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;
	cam.background = color(0, 0, 0);
	cam.gamma = 2.2;

	cam.fov = 40;
	cam.look_from = vec3(278, 278, -800);
	cam.look_at = vec3(278, 278, 0);
	cam.vup = vec3(0, 1, 0);

	cam.defocus_angle = 0;
}

int main()
{
	camera cam;
	hittable_list world;

	switch (3)
	{
	case 1: scene1(cam, world); break;
	case 2: scene_quads(cam, world); break;
	case 3: cornell_box(cam, world); break;
	}


	auto begin = std::chrono::steady_clock::now(); // Time point.

	cam.render(world);

	auto end = std::chrono::steady_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
	std::clog << "Duration = " << time << " ms" << std::endl;
}
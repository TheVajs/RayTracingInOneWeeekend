#include <iostream>

#include "library/utility.h"
#include "library/hittableList.h"
#include "library/camera.h"
#include "library/color.h"
#include "library/material.h"
#include "library/sphere.h"

int main()
{
	auto material_center = make_shared<lambertian>(color(.5, .5, .5));

	hittable_list world;
	world.add(make_shared<sphere>(vec3(0.0, -100.0, -1.0), 100.0, material_center));

	camera cam;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 50;
	cam.max_depth = 50;
	cam.fov = 90;

	cam.render(world);
}
#include <iostream>

#include "library/utility.h"
#include "library/hittableList.h"
#include "library/camera.h"
#include "library/color.h"
#include "library/material.h"
#include "library/sphere.h"

int main()
{
	// auto material_center = make_shared<lambertian>(color(.5, .5, .5));
	auto material_volume = make_shared<simple_volume>(color(0.8, 0.1, 0.5), 1);

	/* auto test_r = ray(vec3(0, 1, 2), vec3(0, 0, -1));
	auto rec = hit_record();
	auto s = sphere(vec3(0, 1, 0), 1, material_volume);
	s.hit(test_r, interval(0.001, infinity), rec);
	auto dist = (test_r.at(rec.t) - test_r.at(rec.t1)).length();
	std::clog << rec.t << " / " << rec.t1 << std::endl;
	std::clog << test_r.at(rec.t) << " / " << test_r.at(rec.t1) << std::endl;
	std::clog << dist << std::endl;
	std::clog << exp(-dist * .03) << std::endl;
	std::clog << exp(-dist * .5) << std::endl; */

	hittable_list world;
	// world.add(make_shared<sphere>(vec3(0, -100.0, -1), 100, material_center));
	world.add(make_shared<sphere>(vec3(0, 1, 0), 1.5, material_volume));

	camera cam;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;

	cam.fov = 90;
	cam.look_from = vec3(0, 1, 2);
	cam.look_at = vec3(0, 1, 0);

	cam.render(world);
}
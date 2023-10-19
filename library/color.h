#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

#include <iostream>

using color = vec3;

double linear_to_gamma(double x)
{
	return pow(x, 1 / 2.2);
}

vec3 linear_to_gamma(const vec3& pixel_color)
{
	return vec3(linear_to_gamma(pixel_color.x()),
				linear_to_gamma(pixel_color.y()),
				linear_to_gamma(pixel_color.z()));
}

void write_color(std::ostream& out, color pixel_color)
{
	double r = pixel_color.x(), g = pixel_color.y(), b = pixel_color.z();

	// Maps double color to [0, 255] range and writes to output.
	static const interval intensity(0, .999);
	out << static_cast<int>(255.999 * intensity.clamp(r)) << ' '
		<< static_cast<int>(255.999 * intensity.clamp(g)) << ' '
		<< static_cast<int>(255.999 * intensity.clamp(b)) << '\n';
}

#endif
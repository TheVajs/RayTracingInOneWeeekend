#include "..\library\utility.h"

#include <algorithm>
#include <vector>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <cmath>
#include <stdlib.h>


void integrate()
{
	double a = 0, b = 2;
	double N = 1000000;
	double result = 0;

	for (int i = 0; i < N; i++)
	{
		double x = random_double(a, b);
		result += x * x; //pow(sin(x), 5);
	}

	std::cout << std::fixed << std::setprecision(12);
	std::cout << "I = " << (b - a) * (result / N) << std::endl;
}

struct sample
{
	double x, p_x;
};

bool compare_by_x(const sample& a, const sample& b)
{
	return a.x < b.x;
}

void constructing_a_nonuniform_PDF()
{
	// Probability density function
	// Accumalitive probability density function APDF

	// Goal: create a nonuniform random value generator where the input is a PDF
	// p(x) = exp(-x / (2 * pi)) * sin^2(x);

	double N = 1000000;
	double sum = 0;
	std::vector<sample> samples;

	for (int i = 0; i < N; i++)
	{
		// Get the area under the curve
		auto x = random_double(0, 2 * pi);
		auto sin_x = sin(x);
		auto p_x = exp(-x / (2 * pi)) * sin_x * sin_x;
		sum += p_x;

		// Store this samples
		sample this_sample = { x, p_x };
		samples.push_back(this_sample);
	}

	std::sort(samples.begin(), samples.end(), compare_by_x);

	double half_sum = sum / 2.0;
	double halfway_point = 0;
	double accum = 0;

	for (int i = 0; i < N; i++)
	{
		accum += samples[i].p_x;
		if (accum > half_sum)
		{
			halfway_point = samples[i].x;
			break;
		}
	}

	std::cout << std::fixed << std::setprecision(12);
	std::cout << "Avarge = " << sum / N << std::endl;
	std::cout << "Area under the curve = " << 2 * pi * sum / N << std::endl;
	std::cout << "Halway = " << halfway_point << std::endl;
}

double f(double d)
{
	return 2.0 * d;
}

double pdf(double x)
{
	return .5;
}

void aproximating_distribution()
{
	// Probability P(r), P(0) = 0, P(2) = 1, range of probability [0, 1], range of input [0, 2]
	// Uniform random distribution is just random_double();
	// Nonuniform distribution, some range of values has a higher probability than other.

	// CDF - comulative distribution function P(x) vs PDF - probability distribution function
	// from PDF to CDF you need to integrate from -infinity to x,
	// to go from CDF to PDF you need to take its derivative.

	// p(x) = dP(x)/dx;

	int N = 100000;
	auto sum = 0.0;

	for (int i = 0; i < N; i++)
	{
		auto x = f(random_double());
		sum += x * x / pdf(x);
	}

	std::cout << std::fixed << std::setprecision(12);
	std::cout << "I = " << sum / N << std::endl;
}

int main()
{
	// Integrate x^2 or similar functions
	integrate();
	// constructing_a_nonuniform_PDF();
	aproximating_distribution();
}
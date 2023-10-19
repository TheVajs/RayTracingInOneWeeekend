#ifndef INTERVAL_H
#define INTERVAL_H

class interval
{
public:
	double min;
	double max;

	interval() {}

	interval(double _min, double _max) : max(_max), min(_min) {}

	bool contains(double x) const
	{
		return min <= x && x <= max;
	}

	bool surrounds(double x) const
	{
		return min < x && x < max;
	}

	double clamp(double x) const
	{
		if (x < min)
			return min;
		if (x > max)
			return max;
		return x;
	}
};

const static interval empty(+infinity, -infinity);
const static interval universe(-infinity, +infinity);

#endif
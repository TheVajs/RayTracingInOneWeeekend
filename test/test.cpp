#include <iostream>

int main()
{
	int x = 10;
	std::cout << x << std::endl;
	std::cout << &x << std::endl;

	int* y = &x;
	std::cout << y << std::endl;
	std::cout << *y << std::endl;
}
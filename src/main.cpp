#include "../include/calcit.hpp"
#include <iostream>
#include <ostream>
#include <filesystem>

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cout << "Usage: CalcIt <expression>.....<xpression>\n";
		return 1;
	}

	for (int i = 1; i < argc; ++i)
	{
		double result = CalcIt(argv[i]);
		printf("result = %lf\n", result);
	}

	return 0;
}
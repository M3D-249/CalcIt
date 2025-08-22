#include "../include/calcit.hpp"
#include <iostream>

int main()
{
	std::cout << "Enter an arithmetic expression : ";

	std::vector<std::string> tokens;
	std::string line;
	while (std::getline(std::cin, line))
	{
		if (ValidateExpresion(line, &tokens))
		{
			std::cout << "[ ";
			for (auto token : tokens)
			{
				std::cout << "'" << token << "'  ";
			}
			std::cout << " ]\n";
		}

		tokens.clear();
		std::cout << "Enter an arithmetic expression : ";
	}

	return 0;
}
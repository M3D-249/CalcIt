#include "../include/calcit.hpp"
#include <iostream>
#include <ostream>
#include <filesystem>

int main()
{
	std::cout << "Enter an arithmetic expression : ";

	std::deque<std::string> postfix;
	std::string line;
	while (std::getline(std::cin, line))
	{
		if (InfixToPostfix(line, &postfix))
		{
			std::cout << "[ ";
			for (auto token : postfix)
			{
				std::cout << "'" << token << "' ";
			}
			std::cout << " ]\n";
			
			double result = EvaluatePostfix(&postfix);
			printf("result = %0.5lf\n", result);
		}

		postfix.clear();
		std::cout << "Enter an arithmetic expression : ";
	}

	return 0;
}
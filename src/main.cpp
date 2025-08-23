#include "../include/calcit.hpp"
#include <iostream>

int main()
{
	std::cout << "Enter an arithmetic expression : ";

	std::deque<Token> postfix;
	std::string line;
	while (std::getline(std::cin, line))
	{
		if (InfixToPostfix(line, &postfix))
		{
			std::cout << "[ ";
			for (auto token : postfix)
			{
				std::cout << "'" << token.value << "'  ";
			}
			std::cout << " ]\n";
		}

		postfix.clear();
		std::cout << "Enter an arithmetic expression : ";
	}

	return 0;
}
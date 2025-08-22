#include "../include/calcit.hpp"
#include <exception>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <string>

std::vector<std::string> _functions = { "sin", "cos", "tan", "cotan", "ctan", "sec", "cosec", "csc"};

bool ValidateExpresion(std::string expression, std::vector<std::string>* tokens)
{
	int leftBracket = 0;

	std::stringstream numBuffer;
	std::stringstream funcBuffer;

	auto clearBuffers = [&]() {
		if (numBuffer.str().size())
		{
			tokens->push_back(numBuffer.str());
			numBuffer.str(""); // empty
			numBuffer.clear(); // clear Errors
		}

		if (funcBuffer.str().size())
		{
			if (!IsValidArithmeticFunction(funcBuffer.str())) // not an operator
				throw std::exception();

			tokens->push_back(funcBuffer.str());
			funcBuffer.str(""); // empty
			funcBuffer.clear(); // clear errors
		}
	};

	for (char ch : expression)
	{
		if (std::isdigit(ch)) // number & not reading function
		{
			if (funcBuffer.str().size())
			{
				if (!IsValidArithmeticFunction(funcBuffer.str())) // not a function
					return false;

				tokens->push_back(funcBuffer.str());
				funcBuffer.str(""); // empty
				funcBuffer.clear(); // clear errors
			}

			numBuffer.put(ch);
		}
		else if (std::isalpha(ch))
		{
			if (numBuffer.str().size())
			{
				tokens->push_back(numBuffer.str());
				numBuffer.str("");
				numBuffer.clear();
			}

			funcBuffer.put(ch);
		}
		else if (ch == '.')
		{
			if (!funcBuffer.str().size())
				numBuffer.put(ch);
			else
				return false;   // invalid syntax
		}
		else if (IsArithmeticOperator(ch))
		{
            try {
                clearBuffers();
            } catch (InvalidArithmeticExpressionError& e) {
                std::cerr << e.what() << std::endl;
            }

			tokens->push_back(std::string(1, ch));
		}
		else if (ch == '(')
		{
            try {
                clearBuffers();
            } catch (InvalidArithmeticExpressionError& e) {
                std::cerr << e.what() << std::endl;
            }
			++leftBracket;
		}
		else if (ch == ')')
		{
            try {
                clearBuffers();
            } catch (InvalidArithmeticExpressionError& e) {
                std::cerr << e.what() << std::endl;
            }
			--leftBracket;
		}
		else if (std::isspace(ch))
		{
            try {
                clearBuffers();
            } catch (InvalidArithmeticExpressionError& e) {
                std::cerr << e.what() << std::endl;
            }
			continue;
		}
		else
		{
			return false;
			std::cerr << "Uknkown item {'" << ch << "'}";
		}
	}

	if (leftBracket) // unmatched brackets
		return false;

    try {
        clearBuffers();
    } catch (InvalidArithmeticExpressionError& e) {
        std::cerr << e.what() << std::endl;
    }

	return true;
}

bool IsArithmeticOperator(const char& ch)
{
	switch (ch)
	{
	case '+':
	case '-':
	case '*':
	case '/':
	case '^':
	case '%':
	case '!':
	return true;
	default: return false;
	}
}

bool IsValidArithmeticFunction(const std::string& func)
{
	return std::find(_functions.begin(), _functions.end(), func) != _functions.end();
}

InvalidArithmeticExpressionError::InvalidArithmeticExpressionError(const std::string& msg) 
{
    this->msg = msg;
}

const char* InvalidArithmeticExpressionError::what() const noexcept 
{
    return this->msg.c_str();
}
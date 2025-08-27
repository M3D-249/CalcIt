#include "../include/calcit.hpp"
#include <cstddef>
#include <format>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <string>
#include <deque>
#include <vector>

#pragma region Maps & Lists
std::map<char, ArithmeticOperator> _operators_map = 
{
	{'+', ArithmeticOperator("+", 1, true, false) },
	{'-', ArithmeticOperator("-", 1, true, false) },
	{'*', ArithmeticOperator("*", 2, true, false) },
	{'/', ArithmeticOperator("/", 2, true, false) },
	{'%', ArithmeticOperator("%", 2, true, false) },
	{'^', ArithmeticOperator("^", 3, false, false) },
	{'!', ArithmeticOperator("!", 3, false, true) }
};

std::map<std::string, ArithmeticOperator> _functions_map = 
{
	{"sin", ArithmeticOperator("sin", 3, true, true)},
	{"cos", ArithmeticOperator("cos", 3, true, true)},
	{"tan", ArithmeticOperator("tan", 3, true, true)},
	{"cot", ArithmeticOperator("cot", 3, true, true)},
	{"sec", ArithmeticOperator("sec", 3, true, true)},
	{"csc", ArithmeticOperator("csc", 3, true, true)},
	{"log", ArithmeticOperator("csc", 3, true, true)},
};

std::vector<std::string> _functions = { "sin", "cos", "tan", "cotan", "ctan", "sec", "cosec", "csc"};
#pragma endregion

#pragma region Implementations
bool CalcIt(const std::string& expression)
{
	bool success = true;

	std::deque<Token> postfix;
	if (InfixToPostfix(expression, &postfix))
	{
        /// TODO: Complete it :)
	}
	else
		success = false;

	return success;
}

bool EvaluatePostfix(std::deque<Token>* postfix)
{
	int lastIndex = postfix->size() - 1;
	while (IsArithmeticOperator(postfix->at(lastIndex).value[0]) || IsValidArithmeticFunction(postfix->at(lastIndex).value))
	{
		--lastIndex;
	}

	for (int i = lastIndex; i >= 0; --i)
	{
		if (IsArithmeticOperator(postfix->at(lastIndex).value[0]))
		{
			ArithmeticOperator op = _operators_map.at(postfix->at(lastIndex).value[0]);
			if (op.unary)
			{
				/// TODO: figure a way for distinguishing between functions and operators 
                /// may be return to class speration ?
                /// TODO: complete the code :)
			}
		}
	}
	
	return true;
}

bool InfixToPostfix(const std::string& expression, std::deque<Token>* postfix)
{
    std::stringstream numBuffer, funcBuffer;
    std::deque<ArithmeticOperator> operatorStack;
    int leftBracket = 0;
    Token prev{""};

    for (size_t i = 0; i < expression.size(); ++i)
    {
        char ch = expression[i];

        if (std::isspace(ch))
            continue;

        // --- Number Parsing ---
        if (std::isdigit(ch) || ch == '.')
        {
            numBuffer.str("");
			numBuffer.clear();

            bool floatSeen = false;

            while (i < expression.size() && (std::isdigit(expression[i]) || expression[i] == '.'))
            {
                if (expression[i] == '.')
                {
                    if (floatSeen)
                        throw InvalidArithmeticExpressionError("Multiple dots in number index: " + std::to_string(i));
               
					floatSeen = true;
                }
                numBuffer << expression[i++];
            }

            --i; // decrement the extra increment
            prev = Token{numBuffer.str()};
            postfix->push_back(prev);
            continue;
        }

        // --- Function Name Parsing ---
        if (std::isalpha(ch))
        {
            funcBuffer.str(""); 
			funcBuffer.clear();

            while (i < expression.size() && std::isalpha(expression[i]))
            {
                funcBuffer << expression[i++];
            }
            --i; // decrement extra increment

            std::string funcName = funcBuffer.str();
            if (!IsValidArithmeticFunction(funcName))
                throw InvalidArithmeticExpressionError("Invalid function: " + funcName);

            prev = _functions_map.at(funcName);
            operatorStack.push_back(_functions_map.at(funcName));
            continue;
        }

        // --- Operator ---
        if (IsArithmeticOperator(ch))
        {
			// there is an operator with high precedence which is not (
			while (OperatorWithHigherPrecedence(&operatorStack, _operators_map.at(ch))) 
			{
				ArithmeticOperator op = operatorStack.back();
				operatorStack.pop_back();
				postfix->push_back(Token(op.value.c_str()));
			}


            // ----- Hnadling negation -----
            if (ch == '-') 
            {
                if (!prev.value.empty())
                {
                    if (prev.value == "(") // negate
                    {
                        operatorStack.push_back(
                            ArithmeticOperator("-", 1, true, true));
                        continue;
                    }

                    // operator
                    if ((IsArithmeticOperator(prev.value[0]) || IsValidArithmeticFunction(prev.value)))
                    {
                        // treat the minus sign as a negation and insert it later to the number
                        postfix->push_back(Token{"-"});
                        operatorStack.push_back(
                            ArithmeticOperator("-", 1, true, true));
                        continue;
                    }
                }
                else // negate if empty
                {
                    operatorStack.push_back(
                        ArithmeticOperator("-", 1, true, true));
                    continue;
                }
            }

            prev = _operators_map.at(ch);
			operatorStack.push_back(_operators_map.at(ch));
			continue;
        }

        // --- Left Parenthesis ---
        if (ch == '(')
        {
            prev = {"("};
            operatorStack.push_back(ArithmeticOperator("(", 0, true));
            ++leftBracket;
            continue;
        }

        // --- Right Parenthesis ---
        if (ch == ')')
        {
            if (leftBracket == 0)
                throw InvalidArithmeticExpressionError(
                std::format("Unmatched closing parenthesis at {}", i));

            while (!operatorStack.empty() && operatorStack.back().value != "(")
            {
                postfix->push_back(Token{operatorStack.back().value});
                operatorStack.pop_back();
            }

            if (operatorStack.empty())
                throw InvalidArithmeticExpressionError(
                    std::format("Mismatched parentheses at {}", i));

            prev = {")"};
            operatorStack.pop_back(); // remove "("
            --leftBracket;
            continue;
        }

        // --- Unknown Character ---
        throw InvalidArithmeticExpressionError(std::format("Unknown character: {}",  ch));
    }

    if (leftBracket != 0)
        throw InvalidArithmeticExpressionError("Unmatched opening parenthesis");

    while (!operatorStack.empty())
    {
        postfix->push_back(Token{operatorStack.back().value});
        operatorStack.pop_back();
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

bool OperatorWithHigherPrecedence(std::deque<ArithmeticOperator>* ops, const ArithmeticOperator& o1)
{
	for (int i = ops->size()-1; i >= 0; i--)
	{
		auto& o2 = ops->at(i);

        if (o2.value == "(")
            return false;

		if (o2.precedence > o1.precedence /*|| (o2.precedence == o1.precedence && o1.leftAssociative)*/)
		// the commented line works but it makes the postfix output ugly, personal prefrence though :)
            return true;
	}

	return false;
}
#pragma endregion
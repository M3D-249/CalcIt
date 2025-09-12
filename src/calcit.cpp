#include "../include/calcit.hpp"
#include <cstddef>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <string>
#include <deque>
#include <vector>
#include <stack>
#include <cmath>

#pragma region Maps & Lists
std::map<char, ArithmeticOperator> _operators_map = 
{
	{'+', ArithmeticOperator("+", 1, true, false) },
	{'-', ArithmeticOperator("-", 1, true, false) },
	{'*', ArithmeticOperator("*", 2, true, false) },
	{'/', ArithmeticOperator("/", 2, true, false) },
	{'%', ArithmeticOperator("%", 2, true, false) },
	{'^', ArithmeticOperator("^", 4, false, false)},
	{'!', ArithmeticOperator("!", 4, true, true)  }
};

std::map<std::string, ArithmeticOperator> _functions_map = 
{
	{"u-",  ArithmeticOperator("u-", 3, false, true)},
	{"sin", ArithmeticOperator("sin", 4, true, true)},
	{"cos", ArithmeticOperator("cos", 4, true, true)},
	{"tan", ArithmeticOperator("tan", 4, true, true)},
	{"cot", ArithmeticOperator("cot", 4, true, true)},
	{"sec", ArithmeticOperator("sec", 4, true, true)},
	{"csc", ArithmeticOperator("csc", 4, true, true)},
	{"log", ArithmeticOperator("log", 4, true, true)},
	{"sqrt",ArithmeticOperator("sqrt", 4, true, true)},
};

std::vector<char> _operators = { '+', '-', '*', '/', '%', '^', '!' };
std::vector<std::string> _functions = { "sin", "cos", "tan", "cotan", "ctan", "sec", "cosec", "csc", "log", "sqrt"};
#pragma endregion

#pragma region Implementations
double CalcIt(const char* expression)
{
    std::deque<std::string> postfix;
    double result = 0;

    if (InfixToPostfix(expression, &postfix))
        result = EvaluatePostfix(&postfix);
    
    return result;
}

double EvaluatePostfix(std::deque<std::string>* postfix)
{
	std::stack<double> result;

    for (auto& token : *postfix)
    {
        if (isdigit(token[0]) || (token.size() > 1 && token[0] == '-'))
        {
            result.push(stoll(token));
        }
        else
        {
            if (token == "u-")
            {
                double val = result.top();
                result.pop();

                val *= -1;
                result.push(val);
                continue;
            }

            if (token == "!")
            {
                double val = result.top();
                result.pop();

                val = Factorial(val);
                result.push(val);
                continue;
            }

            if (IsArithmeticOperator(token[0]))
            {
                double val1 = result.top();
                result.pop();
                double val2 = result.top();
                result.pop();

                if (val2 == 0 && token[0] == '/')
                {
                    printf("Division by thero isn't allowed!");
                    return 0;
                }

                double value = EvaluateBinaryOperator(token[0], val1, val2);
                result.push(value);
                continue;
            }

            if (IsValidArithmeticFunction(token))
            {
                double val = result.top();
                result.pop();

                val = EvaluateFunction(token, val);
                result.push(val);
                continue;
            }
        }
    }

    if (result.size() > 1)
    {
        printf("Invalid Postfix");
        return 0;
    }
	
	return result.top();
}

bool InfixToPostfix(const std::string& expression, std::deque<std::string>* postfix)
{
    if (expression.empty())
        return false;

    std::stringstream numBuffer, funcBuffer;
    std::deque<ArithmeticOperator> operatorStack;
    int leftBracket = 0;
    std::string prev {""};

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
            prev = numBuffer.str();
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

            prev = funcName;
            operatorStack.push_back(_functions_map.at(funcName));
            continue;
        }

        // --- Operator ---
        if (IsArithmeticOperator(ch))
        {
            // ----- Hnadling negation -----
            if (ch == '-' || ch == '+') 
            {
                std::string token = "u";
                token += ch;

                if ((!prev.empty() && 
                    (prev == "(" || (IsArithmeticOperator(prev[0]) && prev[0] != '!'))) ||
                    prev.empty())
                {
                    // ignore '+' it does nothing
                    if (ch == '+')
                        continue;

                    // unary minus
        			while (OperatorWithHigherPrecedence(&operatorStack,_functions_map.at(token))) 
                    {
                        postfix->push_back(operatorStack.back().value);
                        operatorStack.pop_back();
                    }

                    prev = token;
                    operatorStack.push_back(_functions_map.at(token));
                    continue;
                }
            }

			// there is an operator with high precedence which is not (
			while (OperatorWithHigherPrecedence(&operatorStack, _operators_map.at(ch))) 
			{
				postfix->push_back(operatorStack.back().value);
				operatorStack.pop_back();
			}

            prev = std::string(1, ch);
			operatorStack.push_back(_operators_map.at(ch));
			continue;
        }

        // --- Left Parenthesis ---
        if (ch == '(')
        {
            prev = "(";
            operatorStack.push_back(ArithmeticOperator("(", 0, true));
            ++leftBracket;
            continue;
        }

        // --- Right Parenthesis ---
        if (ch == ')')
        {
            if (leftBracket == 0)
                throw InvalidArithmeticExpressionError("Unmatched closing parenthesis at "+std::to_string(i));

            while (!operatorStack.empty() && operatorStack.back().value != "(")
            {
                postfix->push_back(operatorStack.back().value);
                operatorStack.pop_back();
            }

            if (operatorStack.empty())
                throw InvalidArithmeticExpressionError(
                    "Mismatched parentheses at "+std::to_string(i));

            prev = ")";
            operatorStack.pop_back(); // remove "("
            --leftBracket;
            continue;
        }

        // --- Unknown Character ---
        throw InvalidArithmeticExpressionError("Unknown character: "+std::string(1, ch));
    }

    if (leftBracket != 0)
        throw InvalidArithmeticExpressionError("Unmatched opening parenthesis");

    while (!operatorStack.empty())
    {
        postfix->push_back(operatorStack.back().value);
        operatorStack.pop_back();
    }
    return true;
}

double EvaluateBinaryOperator(char ch, double operand1, double operand2)
{
    switch (ch)
    {
    case '+': return operand1 + operand2;
    case '-': return operand1 - operand2;
    case '*': return operand1 * operand2;
    case '/': return operand1 / operand2;
    case '%': return (int)operand1 % (int)operand2;
    case '^': return std::pow(operand1, operand2);
    default:
        return 0;
    }
}

double EvaluateFunction(const std::string& func, double operand)
{
    double operand_radians = operand * M_PI / 180;
    if (func == "sin")
        return std::sin(operand_radians);
    else if (func == "cos")
        return std::cos(operand_radians);
    else if (func ==  "tan")
        return std::tan(operand_radians);
    else if (func == "cotan" || func == "ctan")
        return 1 / std::tan(operand_radians);
    else if (func == "sec")
        return 1 / std::cos(operand_radians);
    else if (func == "cosec" || func == "csc")
        return 1 / std::sin(operand_radians);
    else if (func == "log")
        return std::log(operand);
    else if (func == "sqrt")
        return sqrt(operand);
    else
        return 0;
}

double Factorial(double value)
{
    double fac = 1;
    double i = 2;
    while (i <= value)
    {
        fac *= i;
        i += 1;
    } 
        
    return fac;
}

bool IsArithmeticOperator(const char& ch)
{
    return std::find(_operators.begin(), _operators.end(), ch) != _operators.end();
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

		if (o2.precedence > o1.precedence || (o2.precedence == o1.precedence && o1.leftAssociative))
            return true;
	}

	return false;
}
#pragma endregion
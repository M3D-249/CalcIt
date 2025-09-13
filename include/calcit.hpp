#include <deque>
#include <format>
#include <memory>
#include <string>
#include <sys/types.h>
#include <map>

struct ArithmeticOperator;

// Helpers
bool IsArithmeticOperator(const char& ch);
bool IsValidArithmeticFunction(const std::string& func);
bool OperatorWithHigherPrecedence(std::deque<ArithmeticOperator>* ops, const ArithmeticOperator& o1);
bool InfixToPostfix(const std::string& expression, std::deque<std::string>* postfix);

// Evaluations
double EvaluatePostfix(std::deque<std::string>* postfix);
double EvaluateBinaryOperator(char ch, double operand1, double operand2);
double EvaluateFunction(const std::string& func, double operand);
double Factorial(double value);

// main api
double CalcIt(const char* expression);

class InvalidArithmeticExpressionError : public std::exception
{
protected:
    std::string msg;
public:
    explicit InvalidArithmeticExpressionError(const std::string& msg);
    const char* what() const noexcept override;
};

struct ArithmeticOperator 
{
    enum class Type : char { NUmber, Function, Operator };
    Type type;
    std::string value;
    unsigned int precedence;
    bool leftAssociative;
    bool unary; // false means binary

    explicit ArithmeticOperator(const std::string& value, unsigned int precedence, bool leftAssociative, bool unary=false) 
        : value(value), precedence(precedence), leftAssociative(leftAssociative), unary(unary)
    {  }

    ArithmeticOperator() = delete;
    
    std::string ToString() const
    {
        return "Operator: " + value + ", precedence="+std::to_string(precedence)+", leftAssociative="+std::to_string(leftAssociative)+", unary="+std::to_string(unary);
    } 
};
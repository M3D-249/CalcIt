#include <deque>
#include <string>
#include <sys/types.h>
#include <map>

class ArithmeticOperator;
struct Token;

extern std::map<char, ArithmeticOperator> _operators_map;
extern std::map<std::string, ArithmeticOperator> _functions_map;

bool IsArithmeticOperator(const char& ch);
bool IsValidArithmeticFunction(const std::string& func);
bool OperatorWithHigherPrecedence(std::deque<ArithmeticOperator>* ops, const ArithmeticOperator& o1);
bool InfixToPostfix(const std::string& expression, std::deque<Token>* postfix);
bool CalcIt(const std::string& expression);

class InvalidArithmeticExpressionError : public std::exception
{
protected:
    std::string msg;
public:
    explicit InvalidArithmeticExpressionError(const std::string& msg);
    const char* what() const noexcept override;
};

struct Token
{
    std::string value;

    Token(const std::string& value) : value(value) { }
};

class ArithmeticOperator
{
public:
    std::string value;
    uint precedence;
    bool leftAssociative;
    bool unary; // false means binary

    explicit ArithmeticOperator(const std::string& value, uint precedence, bool leftAssociative, bool unary=false) 
        : value(value), precedence(precedence), leftAssociative(leftAssociative), unary(unary)
    {  }

    ArithmeticOperator() = delete; 
};
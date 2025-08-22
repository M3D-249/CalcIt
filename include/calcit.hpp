#include <string>
#include <vector>


bool IsArithmeticOperator(const char& ch);
bool IsValidArithmeticFunction(const std::string& func);
bool ValidateExpresion(std::string expression, std::vector<std::string>* tokens);


class InvalidArithmeticExpressionError : public std::exception
{
protected:
    std::string msg;
public:
    explicit InvalidArithmeticExpressionError(const std::string& msg);
    const char* what() const noexcept override;
};
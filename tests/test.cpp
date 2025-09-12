// test/test.cpp
#include <fstream>
#include <iostream>
#include <deque>
#include <string>
#include <filesystem>

#include "../include/calcit.hpp"


std::string get_test_file(const std::string& fileName)
{
    if (std::filesystem::exists(fileName))
        return fileName;
    else
        printf("FIle doesnt exist: %s\n", fileName.c_str());

    std::string paths[] = {
        "../share"      // data dir
        "../tests",  // build dir
    };

    for (auto& dir : paths)
        if (std::filesystem::exists(dir + "/" + fileName))
        {
            return dir + "/" + fileName;
        }

    return fileName;
}

int main() {
    std::ifstream sourceFile;
    sourceFile.open(get_test_file("test_expressions.txt"));
    std::ifstream postfixFile;
    postfixFile.open(get_test_file("test_expressions_postfix.txt"));

    if (!sourceFile.good() || !postfixFile.good())
    {
        std::cerr << get_test_file("test_expressions.txt") << "\n";
        perror("Cant open test files!");
        return 1;
    }

    std::string srcLine;
    std::string postfixLine;
    int test_count = 0;
    int passed = 0;

    while (std::getline(sourceFile, srcLine) && std::getline(postfixFile, postfixLine)) 
    {
        std::deque<std::string> postfix;
        std::cout << "Test " << ++test_count << ": " << srcLine << std::endl;
    
        try {
            
            if (InfixToPostfix(srcLine, &postfix))
            {   
                std::string buff = "";
                for (size_t i = 0; i < postfix.size() - 1; ++i)
                    buff += postfix.at(i) + " ";
                buff += postfix.back();
                
                if (buff == postfixLine)
                {
                    printf("Passed:\n\t%s\n\t%s\n", postfixLine.c_str(), buff.c_str());
                    ++passed;
                }
                else
                    printf("Faild:\n\t%s\n\t%s\n", postfixLine.c_str(), buff.c_str());
            }
            else
                printf("Error parsing expression: %s.\n\n", srcLine.c_str());
        } 
        catch (InvalidArithmeticExpressionError& e) 
        {
            std::cerr << e.what();
        }
    }

    std::cout << "Total tests: " << test_count << ", Passed: " << passed << "\n";
    return 0;
}

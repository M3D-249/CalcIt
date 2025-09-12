#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

// Operator set
const std::vector<std::string> ops = { "+", "-", "*", "/", "^" };
const std::vector<std::string> nums = {
    "1","2","3","4","5","6","7","8","9","10",
    "11","12","13","14","15","16","17","18","19","20"
};

// Random utility functions
std::string random_number() {
    return nums[rand() % nums.size()];
}

std::string random_operator() {
    return ops[rand() % ops.size()];
}

int random(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Recursive expression builder
std::string generate_expression(int depth) {
    if (depth <= 0) {
        return random_number();
    }

    int choice = rand() % 4;
    switch (choice) {
        case 0: // Simple binary
            return generate_expression(depth - 1) + " " + random_operator() + " " + generate_expression(depth - 1);
        case 1: // Parenthesized binary
            return "(" + generate_expression(depth - 1) + " " + random_operator() + " " + generate_expression(depth - 1) + ")";
        case 2: // Unary negation
            return "-" + generate_expression(depth - 1);
        case 3: // Nested parentheses
            return "(" + generate_expression(depth - 1) + ")";
    }
    return "0"; // Fallback
}

int main() {
    std::ofstream outfile("test_expressions.txt");
    if (!outfile) {
        std::cerr << "Failed to open file for writing.\n";
        return 1;
    }

    srand(static_cast<unsigned>(time(nullptr)));

    int num_expressions = 100;

    for (int i = 0; i < num_expressions; ++i) {
        int max_depth = random(2, 6); // Increasing depth = more complexity
        std::string expr = generate_expression(max_depth);
        outfile << expr << "\n";
    }

    std::cout << "Generated " << num_expressions << " expressions to test_expressions.txt\n";
    return 0;
}
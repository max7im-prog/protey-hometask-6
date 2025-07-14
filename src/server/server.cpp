#include "expression.h"
#include <string>
#include <iostream>

int main(int argc, char const *argv[])
{
    Expression exp(std::string("5+2/(8)"));
    std::cout <<exp.toString() << std::endl;
    std::cout <<exp.evaluate() << std::endl;
    return 0;
}

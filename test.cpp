#include "expression.h"
#include <string>
#include <iostream>

int main(int argc, char const *argv[])
{
    std::string s = "(43.167816)-(73.096771)*(14.504428)/(81.992950)*(67.135902)";
    Expression expr(s);
    std::cout <<expr.toString() << std::endl;
    std::cout <<expr.evaluate() << std::endl;
    return 0;
}

#include "expression.h"
#include <string>
#include <iostream>

int main(int argc, char const *argv[])
{
    std::string s = "(97.089104)*(75.094864)-(28.735106)-(70.746193)-(77.713623)-(46.356651)+(34.739288)/(15.354486)+(91.390427)+(27.453520)";
    Expression expr(s);
    std::cout <<expr.toString() << std::endl;
    std::cout <<expr.evaluate() << std::endl;
    return 0;
}

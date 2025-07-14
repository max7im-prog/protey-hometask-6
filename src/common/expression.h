#pragma once
#include <variant>
#include <memory>

class Expression
{
public:
    enum operations
    {
        add = '+',
        sub = '-',
        mul = '*',
        dev = '/',
    };

    float evaluate();
    std::string toString();
    Expression() = default;
    Expression(std::string s);

protected:
    std::shared_ptr<Expression> first;
    std::shared_ptr<Expression> second;
    float value;
    bool isLeaf;
    char operation;

    static std::string stripBraces(const std::string &s);
};
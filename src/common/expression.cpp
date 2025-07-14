#include "expression.h"
#include <string>
#include <stdexcept>
#include <cctype>

float Expression::evaluate()
{
    float ret = 0;
    float firstVal = 0;
    float secondVal = 0;
    if (this->isLeaf)
    {
        return this->value;
    }
    firstVal = first->evaluate();
    secondVal = second->evaluate();
    switch (this->operation)
    {
    case Expression::add:
        ret = firstVal + secondVal;
        break;
    case Expression::sub:
        ret = firstVal - secondVal;
        break;
    case Expression::mul:
        ret = firstVal * secondVal;
        break;
    case Expression::dev:
        ret = firstVal / secondVal;
        break;
    default:
        throw std::runtime_error(this->operation + " is an unsupported operation");
        break;
    }
    return ret;
}

std::string Expression::toString()
{
    std::string ret = "";
    ret += "(";
    if (this->isLeaf)
    {
        ret += std::to_string(value);
    }
    else
    {
        ret += first->toString();
        ret += operation;
        ret += second->toString();
    }
    ret += ")";
    return ret;
}

Expression::Expression(std::string s)
{
    if (s.size() == 0)
    {
        throw std::runtime_error("expression can't be empty");
    }
    size_t bracesLevel = 0;
    bool isLeaf = true;
    std::string curExpr = "";
    std::string firstExpr = "";
    std::string secondExpr = "";
    bool foundFirst = false;
    int i = 0;
    for (; i < s.size() && !foundFirst; i++)
    {
        if (s[i] == '(')
        {
            if (i != 0)
            {
                bracesLevel++;
                curExpr += s[i];
                isLeaf = false;
            }
        }
        else if (s[i] == ')')
        {
            if (i != s.size() - 1)
            {
                if (bracesLevel <= 0)
                {
                    throw std::runtime_error("closing brace without an opening brace");
                }
                bracesLevel--;
                curExpr += s[i];
            }
        }
        else if (std::isdigit(s[i]) || s[i] == '.')
        {
            curExpr += s[i];
        }
        else if (s[i] == add || s[i] == sub || s[i] == mul || s[i] == dev)
        {
            if (i == 0 && s[i] == sub)
            {
                curExpr += s[i];
            }
            else
            {
                if (curExpr.size() != 0)
                {
                    firstExpr = curExpr;
                    this->operation = s[i];
                    foundFirst = true;
                    isLeaf = false;
                }
                else
                {
                    throw std::runtime_error("operation without a first operand: \"" + s[i] + std::string("\""));
                }
            }
        }
        else
        {
            throw std::runtime_error("failed to parse \"" + s[i] + std::string("\""));
        }
    }
    if (foundFirst)
    {
        this->isLeaf = false;
        for (; i < s.size() && !foundFirst; i++)
        {
            curExpr += s[i];
        }
        secondExpr = curExpr;
        this->first = std::make_shared<Expression>(firstExpr);
        this->second = std::make_shared<Expression>(secondExpr);
    }
    else if (isLeaf)
    {
        this->isLeaf = true;
        this->value = std::stof(curExpr);
    }
}

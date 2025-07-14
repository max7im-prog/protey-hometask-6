#include "expression.h"
#include <string>
#include <stdexcept>
#include <cctype>

std::string stripBraces(const std::string &s)
{
    std::string ret = s;
    bool canStrip = true;
    while (canStrip)
    {
        if (ret.size() == 0)
        {
            canStrip = false;
            break;
        }
        if (ret[0] != '(')
        {
            canStrip = false;
            break;
        }
        if (ret[ret.size() - 1] != ')')
        {
            canStrip = false;
            break;
        }
        int bracesLevel = 0;
        for (size_t i = 0; i < ret.size(); i++)
        {
            if (ret[i] == '(')
            {
                bracesLevel++;
            }
            else if (ret[i] == ')')
            {
                bracesLevel--;
                if (bracesLevel == 0 && i != ret.size() - 1)
                {
                    canStrip = false;
                    break;
                }
            }
        }
        if (canStrip)
        {
            ret.erase(0, 1);
            ret.erase(ret.size() - 1, 1);
        }
    }
    return ret;
}

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

Expression::Expression(std::string expr)
{
    std::string s = stripBraces(expr);
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
            bracesLevel++;
            curExpr += s[i];
            if (i != 0)
            {
                isLeaf = false;
            }
        }
        else if (s[i] == ')')
        {

            if (bracesLevel <= 0)
            {
                throw std::runtime_error("closing brace without an opening brace");
            }
            bracesLevel--;
            curExpr += s[i];
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
                if (bracesLevel == 0)
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
                else
                {
                    curExpr += s[i];
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
        curExpr = "";
        for (; i < s.size(); i++)
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

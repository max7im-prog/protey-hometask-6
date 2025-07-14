#include "expression.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <cctype>

std::string stripBraces(const std::string& s) {
    std::string ret = s;

    while (!ret.empty() && ret.front() == '(' && ret.back() == ')') {
        int level = 0;
        bool valid = true;

        for (size_t i = 0; i < ret.size(); ++i) {
            if (ret[i] == '(') level++;
            else if (ret[i] == ')') level--;

            
            if (level == 0 && i != ret.size() - 1) {
                valid = false;
                break;
            }
        }

        if (!valid || level != 0) break;

        ret = ret.substr(1, ret.size() - 2);
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
        throw std::runtime_error(std::to_string(this->operation) + " is an unsupported operation");
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
    std::string curExpr = "";
    std::string firstExpr = "";
    std::string secondExpr = "";
    std::vector<std::string> expressions;
    std::vector<char> operations;
    int i = 0;
    for (; i < s.size(); i++)
    {
        if (s[i] == '(')
        {
            bracesLevel++;
            curExpr += s[i];
            if (i != 0)
            {
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
                        expressions.push_back(curExpr);
                        curExpr = "";
                        operations.push_back(s[i]);
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
    expressions.push_back(curExpr);

    std::vector<int> mulDivVector = {};
    std::vector<int> subAddVector = {};

    for (size_t i = 0; i < operations.size(); i++)
    {
        if (operations[i] == mul || operations[i] == dev)
        {
            mulDivVector.push_back(i);
        }
        else if (operations[i] == sub || operations[i] == add)
        {
            subAddVector.push_back(i);
        }
    }
    std::vector<int> operationOrder = subAddVector;
    operationOrder.insert(operationOrder.end(), mulDivVector.begin(), mulDivVector.end());

    if (expressions.size() == 1)
    {
        this->isLeaf = true;
        this->value = std::stof(stripBraces(expressions[0]));
    }
    else
    {
        size_t operNum = operationOrder[0];
        this->operation = operations[operNum];
        curExpr = "";
        for (int i = 0; i < operations.size(); i++)
        {
            curExpr += expressions[i];
            if (i == operNum)
            {
                firstExpr = curExpr;
                curExpr = "";
                this->operation = operations[operNum];
            }
            else
            {
                curExpr += operations[i];
            }
        }
        curExpr += expressions.back();
        secondExpr = curExpr;

        this->isLeaf = false;
        this->first = std::make_shared<Expression>(firstExpr);
        this->second = std::make_shared<Expression>(secondExpr);
    }
}

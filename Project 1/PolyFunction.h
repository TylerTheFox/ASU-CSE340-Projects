/*
    Author:     Brandan Tyler Lasley (blasley@asu.edu)
    ASU ID:     1214587374
    Class:      CSE340S20 MWF 12:55
    Date:       2/1/2020
    Purpose:    This is kind of like a std::function which the parser generates 
                which allows you to call a function such as F(x, y). This is returned
                from the parser after parsing is successfully complete. 
*/
#pragma once
#include "Expression.h"
#include <vector>

class PolyFunction
{
public:
    void pushArg(char arg);
    void popArg();
    bool isVarValid(char var);
    size_t getNumArgs() const;
    void setExpression(const Expression & exp);
    long long evaluate(std::vector<long long> args);
    void setLineNum(unsigned long long num);
    unsigned long long getLineNum() const;
private:
    unsigned long long line;
    std::vector<char> Args;
    Expression Expr;
};

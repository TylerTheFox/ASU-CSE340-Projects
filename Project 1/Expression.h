/*
    Author:     Brandan Tyler Lasley (blasley@asu.edu)
    ASU ID:     1214587374
    Class:      CSE340S20 MWF 12:55
    Date:       2/1/2020
    Purpose:    This is the container for an expression Ex. (2x^2 + 4)
*/
#pragma once
#include "Token.h"
#include <memory>
#include <vector>

class Expression
{
public:
    Expression();
    void setExpression(const Expression & e);
    void setToken(const std::shared_ptr<TokenBase> & token);
    const std::vector<std::shared_ptr<Expression>>& getExpression();
    const std::shared_ptr<TokenBase> getToken();
private:
    std::vector<std::shared_ptr<Expression>> expr;
    std::shared_ptr<TokenBase>  t;
};
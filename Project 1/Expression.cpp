/*
    Author:     Brandan Tyler Lasley (blasley@asu.edu)
    ASU ID:     1214587374
    Class:      CSE340S20 MWF 12:55
    Date:       2/1/2020
    Purpose:    This is the container for an expression Ex. (2x^2 + 4)
*/
#include "Expression.h"

Expression::Expression()
{
}

void Expression::setExpression(const Expression & e)
{
    this->expr.push_back(std::shared_ptr<Expression>(new Expression(e)));
}

void Expression::setToken(const std::shared_ptr<TokenBase> & token)
{
    t = token;
}

const std::vector<std::shared_ptr<Expression>>& Expression::getExpression()
{
    return expr;
}

const std::shared_ptr<TokenBase> Expression::getToken()
{
    return t;
}

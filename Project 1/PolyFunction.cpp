/*
    Author:     Brandan Tyler Lasley (blasley@asu.edu)
    ASU ID:     1214587374
    Class:      CSE340S20 MWF 12:55
    Date:       2/1/2020
    Purpose:    This is kind of like a std::function which the parser generates
                which allows you to call a function such as F(x, y). This is returned
                from the parser after parsing is successfully complete.
*/
#include "PolyFunction.h"
#include <math.h>

void PolyFunction::pushArg(char arg)
{
    Args.push_back(arg);
}

void PolyFunction::popArg()
{
    Args.pop_back();
}

bool PolyFunction::isVarValid(char var)
{
    static char ref_x = 'x';

    if (Args.empty())
    {
        if (var != ref_x)
            return false;
        return true;
    }

    for (const auto & c : Args)
        if (var == c)
            return true;

    return false;
}

size_t PolyFunction::getNumArgs() const
{
    return Args.size();
}

void PolyFunction::setExpression(const Expression & exp)
{
    Expr = exp;
}

long long PolyFunction::evaluate(std::vector<long long> _args)
{
    const auto & exprs = Expr.getExpression();

    struct EvalData
    {
        long long value;
        std::shared_ptr<TokenBase> next_token;
    };

    auto getParamValue = [&](const std::string& str)
    {
        char c = str[0];

        if (Args.size())
        {
            for (size_t i = 0; i < Args.size(); i++)
                if (tolower(Args[i]) == tolower(c))
                    return _args[i];
        }
        else return _args[0];

        throw std::runtime_error("Internel Eval Error");
        return 0LL;
    };

    std::vector<EvalData> result;

    for (size_t i = 0; i < exprs.size(); i++)
    {
        auto token = exprs[i]->getToken();
        struct EvalData e {};

        switch (token->getTokenType())
        {
        case TokenType::TERM:
        {
            auto term = dynamic_cast<TokenTerm&>(*token);
            e.value = static_cast<long long>(term.num * pow(getParamValue(term.var), term.power));
            break;
        }
        case TokenType::NUM:
        {
            auto term = dynamic_cast<TokenNum&>(*token);
            e.value = term.num;
            break;
        }
        default:
            throw std::runtime_error("Internel Eval error");
        }

        if (i + 1 < exprs.size())
        {
            token = exprs[++i]->getToken();
            if (token->getTokenType() == TokenType::TERM || token->getTokenType() == TokenType::NUM)
            {
                e.next_token = std::shared_ptr<TokenMultiply>(new TokenMultiply);
                --i;
            }
            else
                e.next_token = token;
        }
        result.push_back(e);
    }

    // Multiplication Preprocessor.
    std::vector<EvalData> mult_result;
    for (size_t i = 0; i < result.size(); i++)
    {
        if (result[i].next_token)
        {
            switch (result[i].next_token->getTokenType())
            {
            case TokenType::OPERATOR_MULTIPLY:
            {
                long long ret = 1;
                for (; i < result.size();)
                {
                    auto lhs = result[i].value;
                    auto rhs = result[++i].value;

                    if (ret == 1)
                        ret *= lhs * rhs;
                    else ret *= rhs;
                    
                    if (!result[i].next_token || result[i].next_token->getTokenType() != TokenType::OPERATOR_MULTIPLY)
                        break;
                }
                struct EvalData e {};
                e.value = ret;
                e.next_token = result[i].next_token;
                mult_result.push_back(e);
                break;
            }
            default:
                mult_result.push_back(result[i]);
            }
        } else  mult_result.push_back(result[i]);
    }

    long long final_result = mult_result[0].value;
    for (size_t i = 1; i < mult_result.size(); i++)
    {
        if (mult_result[i - 1].next_token)
        {
            switch (mult_result[i - 1].next_token->getTokenType())
            {
            case TokenType::OPERATOR_MINUS:
                final_result -= mult_result[i].value;
                break;
            case TokenType::OPERATOR_PLUS:
                final_result += mult_result[i].value;
                break;
            default:
                throw std::runtime_error("Internel Polyfunction error");
            }
        }
    }

    return final_result;
}

void PolyFunction::setLineNum(unsigned long long num)
{
    line = num;
}

unsigned long long PolyFunction::getLineNum() const
{
    return line;
}

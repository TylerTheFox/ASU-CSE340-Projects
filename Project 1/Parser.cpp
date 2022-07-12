/*
    Author:     Brandan Tyler Lasley (blasley@asu.edu)
    ASU ID:     1214587374
    Class:      CSE340S20 MWF 12:55
    Date:       2/1/2020
    Purpose:    Parses a vector of tokens into executable code.
*/
#include "Parser.h"
#include <iostream>
#include <sstream>
#include "Build.h"
#include <cassert>

Parser::Parser() : currentFunc(0)
{
}

ParserData Parser::parse(const std::string & str)
{
    lxr.process(str);

    parsePoly();
    auto token = lxr.getToken();

    if (token->getTokenType() == TokenType::START)
    {
        do
        {
            if (currentFunc)
                lxr.unGetToken();
            data.FunctionVect.resize(data.FunctionVect.size() + 1);
            parseInput();

            token = lxr.getToken();
            if (token->getTokenType() == TokenType::CHAR) // Check if functions even exist.
            {
                lxr.unGetToken();
                parseFunctions();
            }
            lxr.unGetToken();
            token = lxr.getToken();
            currentFunc++;
        } while (token->getTokenType() == TokenType::INPUT);
        if (currentFunc)
            lxr.unGetToken();
    }
    else parse_error("Missing START?", token);

    token = lxr.getToken();

    if (token->getTokenType() != TokenType::NUM)
    {
        if (token->getTokenType() != TokenType::ENDOFFILE)
            parse_error("Expected EOF", token);
    }
    else
    {
        lxr.unGetToken();
        parseInputParams();

        token = lxr.getToken();

        if (token->getTokenType() != TokenType::ENDOFFILE)
            parse_error("Expected EOF", token);
    }

    if (data.FunctionVect.size() == 1)
        if (data.FunctionVect.begin()->FunctionCall.size() == 0 && data.FunctionVect.begin()->InputMap.size() == 0)
            parse_error("No functions!", token);

    if (!SemanticErrorVect.empty())
    {
        auto err_str = build_semantic_error_str((*SemanticErrorVect.begin()).Type); // Just choose the first error, whatever.
        throw std::runtime_error(err_str.c_str());
    }

    processInputParams();

    return data;
}

void Parser::PrintOutLexer()
{
    lxr.reset();
    std::shared_ptr<TokenBase> currToken = lxr.getToken();
    do
    {
        const std::string & data = currToken->getTokenData();
        if (data.empty())
            std::cout << currToken->getTokenName() << "\n";
        else
            std::cout << currToken->getTokenName() << " (" << currToken->getTokenData() << ")" << "\n";

        currToken = lxr.getToken();
    } while (currToken->getTokenType() != TokenType::ENDOFFILE);
}

Lexer & Parser::getLexer()
{
    return lxr;
}

std::string Parser::getVarName(std::shared_ptr<TokenBase> & token)
{
    std::ostringstream ss;
    auto token_char = dynamic_cast<TokenChar&>(*token);
    auto var_name = dynamic_cast<TokenChar&>(*token).data;
    ss << var_name;

    if (!token_char.isNextCharSpace())
    {
        token = lxr.getToken();
        do
        {
            if (token->getTokenType() == TokenType::NUM)
            {
                auto t = dynamic_cast<TokenNum&>(*token);
                auto num = t.num;
                ss << num;
                if (t.isNextCharSpace())
                {
                    token = lxr.getToken();
                    break;
                }
            }
            else if (token->getTokenType() == TokenType::CHAR)
            {
                auto t = dynamic_cast<TokenChar&>(*token);
                auto data = t.data;
                ss << data;
                if (t.isNextCharSpace())
                {
                    token = lxr.getToken();
                    break;
                }
            }
            else
                lxr.unGetToken();
            token = lxr.getToken();
        } while (token->getTokenType() == TokenType::NUM || token->getTokenType() == TokenType::CHAR);
    } else token = lxr.getToken();

    lxr.unGetToken();
    return ss.str();
}

void Parser::parsePoly()
{
    auto token = lxr.getToken();
    do
    {
        if (token->getTokenType() != TokenType::POLY)
            parse_error("Missing POLY", token);

        token = lxr.getToken();

        if (token->getTokenType() != TokenType::CHAR)
            parse_error("Invalid POLY name", token);

        auto linenum = token->getLineNum();
        auto poly_name = getVarName(token);
        if (!isalpha(poly_name[0]))
            parse_error("Invalid name", token);

        token = lxr.getToken();

        PolyFunction func;
        if (token->getTokenType() == TokenType::LPAREN)
        {
            // Process args
            processArgs(func);
            token = lxr.getToken();
        }

        if (token->getTokenType() != TokenType::EQUAL)
            parse_error("Missing equal after POLY", token);

        func.setLineNum(linenum);
        parsePolyFunctionBody(func);

        if (data.PolyMap.count(poly_name))
        {
            // Hack to prevent duplicates of the last reported duplicate.
            if (data.PolyMap[poly_name].getLineNum() != static_cast<unsigned long long>(-1))
                semantic_error(SemanticErrors::DuplicatePolyName, data.PolyMap[poly_name].getLineNum());
            semantic_error(SemanticErrors::DuplicatePolyName, func.getLineNum());
            data.PolyMap[poly_name].setLineNum(-1);
        }
        else
            data.PolyMap[poly_name] = func;

        token = lxr.getToken();

        if (token->getTokenType() == TokenType::ENDOFFILE)
            parse_error("Unexpected EOF", token);
    } while (token->getTokenType() != TokenType::START);

    lxr.unGetToken(); // unget start token.
}

void Parser::processArgs(PolyFunction & func)
{
    auto token = lxr.getToken();
    do
    {
        if (token->getTokenType() != TokenType::CHAR)
            parse_error("Invalid Arg Name", token);

        auto var_name = dynamic_cast<TokenChar&>(*token).data;
        func.pushArg(var_name);

        token = lxr.getToken();
        if (token->getTokenType() != TokenType::COMMA)
        {
            if (token->getTokenType() != TokenType::RPAREN)
                parse_error("Missing or invalid closing tag", token);
        }
        else
        {
            // Let's check next token.
            // We should be waiting for the next var
            // so we shouldn't ever EVER see RPAREN.
            token = lxr.getToken();
            if (token->getTokenType() == TokenType::RPAREN)
                parse_error("Unexpected RPAREN", token);
        }

        if (token->getTokenType() == TokenType::ENDOFFILE)
            parse_error("Unexpected EOF", token);
    } while (token->getTokenType() != TokenType::RPAREN);
}

void Parser::parsePolyFunctionBody(PolyFunction & func)
{
    Expression global_expr;
    auto token = lxr.getToken();
    do
    {
        Expression inner_expr;
        if (token->getTokenType() == TokenType::CHAR)
        {
            auto t = dynamic_cast<TokenChar&>(*token);
            auto var_name = t.data;
            if (!isalpha(var_name))
                parse_error("Invalid param name", token);


            // Lets peak ahead and check something.
            {
                token = lxr.getToken();

                if (token->getTokenType() == TokenType::CHAR)
                    if (!t.isNextCharSpace())
                        semantic_error(SemanticErrors::InvalidMonomialName, token);

                lxr.unGetToken();
                lxr.unGetToken();
                token = lxr.getToken();
            }

            auto line_num = token->getLineNum();

            long long power = 1;
            token = lxr.getToken();
            if (token->getTokenType() == TokenType::POWER)
            {
                token = lxr.getToken();
                if (token->getTokenType() != TokenType::NUM)
                    parse_error("Invalid power", token);
                power = dynamic_cast<TokenNum&>(*token).num;
            }
            else lxr.unGetToken();

            // Check something more.
            {
                token = lxr.getToken();

                if (token->getTokenType() == TokenType::NUM)
                    parse_error("error", token);

                lxr.unGetToken();
                lxr.unGetToken();
                token = lxr.getToken();
            }

            if (!func.isVarValid(var_name))
                semantic_error(SemanticErrors::InvalidMonomialName, line_num);

            // Parsed Term complete
            TokenTerm term;
            term.num = 1;
            term.power = power;
            term.var = var_name;
            inner_expr.setToken(std::shared_ptr<TokenTerm>(new TokenTerm(term)));
        }
        else if (token->getTokenType() == TokenType::NUM)
        {
            long long num = dynamic_cast<TokenNum&>(*token).num;
            token = lxr.getToken();
            long long power = 1;

            if (token->getTokenType() == TokenType::CHAR)
            {
                auto t = dynamic_cast<TokenChar&>(*token);
                auto var_name = t.data;
                if (!isalpha(var_name))
                    parse_error("Invalid param name", token);

                // Lets peak ahead and check something.
                {
                    token = lxr.getToken();

                    if (token->getTokenType() == TokenType::CHAR)
                    {
                        if (!t.isNextCharSpace())
                            semantic_error(SemanticErrors::InvalidMonomialName, token);
                    }
                    else if (token->getTokenType() == TokenType::NUM)
                        parse_error("error", token);

                    lxr.unGetToken();
                    lxr.unGetToken();
                    token = lxr.getToken();
                }

                token = lxr.getToken();
                if (token->getTokenType() == TokenType::POWER)
                {
                    token = lxr.getToken();
                    if (token->getTokenType() != TokenType::NUM)
                        parse_error("Invalid power", token);
                    power = dynamic_cast<TokenNum&>(*token).num;
                }
                else
                    lxr.unGetToken();

                // Parsed Term complete
                TokenTerm term;
                term.num = num;
                term.power = power;
                term.var = var_name;

                // Check something more.
                {
                    token = lxr.getToken();

                    if (token->getTokenType() == TokenType::NUM)
                        parse_error("error", token);

                    lxr.unGetToken();
                    lxr.unGetToken();
                    token = lxr.getToken();
                }

                if (!func.isVarValid(var_name))
                    semantic_error(SemanticErrors::InvalidMonomialName, token);

                inner_expr.setToken(std::shared_ptr<TokenTerm>(new TokenTerm(term)));
            }
            else
            {
                TokenNum tokennum;
                tokennum.num = num;
                inner_expr.setToken(std::shared_ptr<TokenNum>(new TokenNum(tokennum)));
                lxr.unGetToken();
            }
        }
        else if (token->getTokenType() == TokenType::OPERATOR_PLUS)
        {
            inner_expr.setToken(std::shared_ptr<TokenPlusOperator>(new TokenPlusOperator));

            // Lets validate the next input.
            token = lxr.getToken();
            lxr.unGetToken();

            if (token->getTokenType() != TokenType::CHAR && token->getTokenType() != TokenType::NUM)
                parse_error("Invalid Operator Usage", token);
        }
        else if (token->getTokenType() == TokenType::OPERATOR_MINUS)
        {
            inner_expr.setToken(std::shared_ptr<TokenMinusOperator>(new TokenMinusOperator));

            // Lets validate the next input.
            token = lxr.getToken();
            lxr.unGetToken();

            if (token->getTokenType() != TokenType::CHAR && token->getTokenType() != TokenType::NUM)
                parse_error("Invalid Operator Usage", token);
        }
        else parse_error("Unexpected Function EOF", token);

        token = lxr.getToken();
        global_expr.setExpression(inner_expr);

        if (token->getTokenType() == TokenType::ENDOFFILE)
            parse_error("Unexpected EOF", token);
        if (token->getTokenType() == TokenType::POLY || token->getTokenType() == TokenType::START)
            parse_error("Missing semicolon after POLY", token);
    } while (token->getTokenType() != TokenType::SEMICOLON);

    func.setExpression(global_expr);
}

void Parser::parseInput()
{
    auto token = lxr.getToken();

    if (token->getTokenType() != TokenType::INPUT)
    {
        // No inputs, no problem!
        lxr.unGetToken();
        return;
    }

    do
    {
        if (token->getTokenType() != TokenType::INPUT)
            parse_error("Missing inputs", token);

        token = lxr.getToken();

        if (token->getTokenType() != TokenType::CHAR)
            parse_error("Incorrect input name", token);

        auto var_name = getVarName(token);

        if (!data.FunctionVect[currentFunc].InputMap.count(var_name))
            data.FunctionVect[currentFunc].InputMapOrder.push_back(var_name);

        // Check for double calls.
        if (data.FunctionVect[currentFunc].InputMap[var_name] == -1)
            data.FunctionVect[currentFunc].InputMap[var_name]--;
        else data.FunctionVect[currentFunc].InputMap[var_name] = -1;

        token = lxr.getToken();
        if (token->getTokenType() != TokenType::SEMICOLON)
            parse_error("Missing semicolon?", token);

        token = lxr.getToken();

        if (token->getTokenType() == TokenType::ENDOFFILE)
            parse_error("Unexpected EOF", token);
    } while (token->getTokenType() == TokenType::INPUT);
    lxr.unGetToken();
}

std::vector<std::shared_ptr<FunctionBase>> Parser::parseFunction(std::shared_ptr<TokenBase> & token, std::string & name, long long & ret_line)
{
    static int depth = 0;

    depth++;

    if (token->getTokenType() != TokenType::CHAR)
        parse_error("Invalid function name", token);

    auto func_line_num = token->getLineNum();
    auto var_name = getVarName(token);

    if (!isalpha(var_name[0]))
        parse_error("Invalid name", token);

    if (!data.PolyMap.count(var_name))
        semantic_error(SemanticErrors::EvaluationOfUnDeclaredPoly, func_line_num);

    token = lxr.getToken();

    if (token->getTokenType() != TokenType::LPAREN)
        parse_error("Missing or incorrect function LPAREN", token);

    token = lxr.getToken();

    std::vector<std::shared_ptr<FunctionBase>> fbv;
    do
    {
        if (token->getTokenType() == TokenType::CHAR)
        {
            auto var_line_num = token->getLineNum();
            auto var_name = getVarName(token);
            token = lxr.getToken();

            // Some kind of function
            if (token->getTokenType() == TokenType::LPAREN || token->getTokenType() == TokenType::CHAR || token->getTokenType() == TokenType::NUM)
            {
                token = lxr.getToken();
                if (token->getTokenType() == TokenType::CHAR || token->getTokenType() == TokenType::LPAREN)
                {
                    lxr.unGetToken();
                    lxr.unGetToken();
                    lxr.unGetToken();
                    for (size_t i = 1; i < var_name.size(); i++)
                        lxr.unGetToken();

                    token = lxr.getToken();

                    long long new_line_num = -1;
                    auto params = parseFunction(token, name, new_line_num);
                    std::shared_ptr<FuncitonParamFunction> pfunc(new FuncitonParamFunction);
                    pfunc->setFunctionName(name);
                    pfunc->setFunctionParams(params);

                    if (new_line_num != -1)
                        pfunc->setLineNum(new_line_num);
                    else
                        pfunc->setLineNum(func_line_num);

                    fbv.push_back(pfunc);
                }
                else if (token->getTokenType() == TokenType::NUM)
                {
                    std::shared_ptr<FuncitonParamFunction> pfunc(new FuncitonParamFunction);
                    pfunc->setFunctionName(var_name);
                    pfunc->setLineNum(func_line_num);
                    if (!data.PolyMap.count(var_name))
                        semantic_error(SemanticErrors::EvaluationOfUnDeclaredPoly, var_line_num);
                    do
                    {
                        auto num = dynamic_cast<TokenNum&>(*token).num;
                        std::shared_ptr<FunctionBase> p(new FunctionParamNum(num));
                        pfunc->setFunctionParams({ p });
                        pfunc->setLineNum(func_line_num);
                        token = lxr.getToken();
                        if (token->getTokenType() == TokenType::COMMA)
                            token = lxr.getToken();
                    } while (token->getTokenType() == TokenType::NUM);
                    fbv.push_back(pfunc);
                }
                else parse_error("Invalid function", token);
            }
            else if (token->getTokenType() != TokenType::COMMA && token->getTokenType() != TokenType::RPAREN)
                parse_error("Invalid function", token);
            else
            {
                lxr.unGetToken();
                lxr.unGetToken();
                token = lxr.getToken();
                auto param_name = var_name;

                // Search for Input
                bool found = false;
                for (auto & fv : data.FunctionVect)
                    if (fv.InputMap.count(param_name))
                        found = true;

                if (!found)
                    semantic_error(SemanticErrors::UninitalizedArgument, token);

                fbv.push_back(std::shared_ptr<FunctionBase>(new FunctionParamVar(param_name)));
            }
        }
        else if (token->getTokenType() == TokenType::NUM)
        {
            auto num = dynamic_cast<TokenNum&>(*token).num;
            fbv.push_back(std::shared_ptr<FunctionBase>(new FunctionParamNum(num)));
        }
        else
            parse_error("Invalid function param", token);

        token = lxr.getToken();
        if (token->getTokenType() != TokenType::COMMA)
        {
            if (token->getTokenType() != TokenType::RPAREN)
                parse_error("Missing or invalid closing tag", token);
        }
        else 
        {
            if (token->getTokenType() == TokenType::COMMA)
            {
                token = lxr.getToken();
                if (token->getTokenType() == TokenType::RPAREN)
                    parse_error("error", token);
            } else token = lxr.getToken();
        }

        if (token->getTokenType() == TokenType::ENDOFFILE)
            parse_error("Unexpected EOF", token);
    } while (token->getTokenType() != TokenType::RPAREN);

    ret_line = func_line_num;
    name = var_name;
    depth--;

    return fbv;
}

void Parser::checkFunctionArgSemantics(const std::shared_ptr<FunctionBase> p)
{
    switch (p->getType())
    {
    case ParamType::NUM:
        break;
    case ParamType::CHAR:
        break;
    case ParamType::FUNC:
    {
        // Lets check num of vars.
        auto fpp = dynamic_cast<FuncitonParamFunction*>(p.get());
        const auto & func_name = fpp->getFuncName();
        const auto line_num = fpp->getLineNum();
        auto & vect = fpp->getFuncParams();
        const auto num_of_args = vect.size();

        if (data.PolyMap.count(func_name))
        {
            const auto real_func_num_of_args = data.PolyMap[func_name].getNumArgs();

            if (!real_func_num_of_args && num_of_args > 1)
                semantic_error(SemanticErrors::IncorrectArguments, line_num);
            else if (real_func_num_of_args && num_of_args != real_func_num_of_args)
                semantic_error(SemanticErrors::IncorrectArguments, line_num);
        }
        //else // This would be a serious error.
        for (auto & v : vect)
            checkFunctionArgSemantics(v);
    }
    break;
    default:
        ; // nothing.
    }
}

void Parser::checkFunctionParamSemantics(const struct FunctionParm & fp, long long line_num)
{
    // Since we destroyed the token line numbers we kind of have to guess where it is.
    lxr.unGetToken();
    auto token = lxr.getToken();

    if (data.PolyMap.count(fp.name))
    {
        if (!data.PolyMap[fp.name].getNumArgs() && fp.fbvect.size() > 1)
            semantic_error(SemanticErrors::IncorrectArguments, line_num);
        else if (data.PolyMap[fp.name].getNumArgs() && fp.fbvect.size() != data.PolyMap[fp.name].getNumArgs())
            semantic_error(SemanticErrors::IncorrectArguments, line_num);

        // Check Funtion Params
        for (const auto & p : fp.fbvect)
            checkFunctionArgSemantics(p);
    }
    //else // This would be a serious error.
}

void Parser::parseFunctions()
{
    auto token = lxr.getToken();
    do
    {
        auto line_num = token->getLineNum();
        long long dummy = -1;
        std::string func_name;
        struct FunctionParm fp;
        fp.fbvect = parseFunction(token, func_name, dummy);
        fp.name = func_name;
        checkFunctionParamSemantics(fp, line_num);
        data.FunctionVect[currentFunc].FunctionCall.push_back(fp);
        token = lxr.getToken();
        if (token->getTokenType() != TokenType::SEMICOLON)
            parse_error("Error missing semicolon?", token);
        token = lxr.getToken();
    } while (token->getTokenType() == TokenType::CHAR);
}

void Parser::parseInputParams()
{
    auto token = lxr.getToken();
    do
    {
        if (token->getTokenType() == TokenType::NUM)
        {
            auto num = dynamic_cast<TokenNum&>(*token).num;
            data.InputParams.push_back(num);
        }
        else parse_error("Expected number.", token);
        token = lxr.getToken();
    } while (token->getTokenType() != TokenType::ENDOFFILE);
}

void Parser::processInputParams()
{
    size_t currentInputNum = 0;

    for (auto & f : data.FunctionVect)
        for (auto & z : f.InputMapOrder)
        {
            if (currentInputNum >= data.InputParams.size())
                parse_error("Not enough input parameters", 0);

            if (f.InputMap[z] < -1)
                currentInputNum += static_cast<size_t>(-1 - f.InputMap[z]);

            f.InputMap[z] = data.InputParams[currentInputNum++];
        }
}

void Parser::parse_error(const char * err_str, const std::shared_ptr<TokenBase> t)
{
#if USE_USEFUL_ERRORS
    std::ostringstream stringStream;
    stringStream << "Parser Exception on token : " << lxr.getPosition() + 1 << " Line: " << t->getLineNum() << " Message: " << err_str;
    throw std::runtime_error(stringStream.str());
#else
    throw std::runtime_error("SYNTAX ERROR !!!");
#endif
}

bool Parser::should_I_report_this_error(enum SemanticErrors error, const unsigned long long line)
{
    // Prevent the parser from adding duplicate duplicare errors.
    /*(switch (error)
    {
    case SemanticErrors::DuplicatePolyName:
        for (const auto & e : SemanticErrorVect)
            if (e.line == line)
                return false;
        break;
    default:
        ; // nothing
    }*/
    return true;
}

void Parser::semantic_error(enum SemanticErrors error, const unsigned long long line)
{
    if (!should_I_report_this_error(error, line))
        return;

    SemanticErrorData err;
    err.line = line;
    err.Type = error;
    SemanticErrorVect.insert(err);
}

void Parser::semantic_error(enum SemanticErrors error, const std::shared_ptr<TokenBase> t)
{
    if (!should_I_report_this_error(error, t->getLineNum()))
        return;

    SemanticErrorData err;
    err.line = t->getLineNum();
    err.Type = error;
    SemanticErrorVect.insert(err);
}

std::string Parser::build_semantic_error_str(SemanticErrors error)
{
    size_t e = static_cast<size_t>(error);
    std::stringstream ss;
    ss << SemanticErrorStrs[e - 1] << " ";
    for (auto idx : this->SemanticErrorVect)
        if (error == idx.Type)
            ss << idx.line + 1 << " ";
    return ss.str();
}

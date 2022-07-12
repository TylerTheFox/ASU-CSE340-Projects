#include "Parser.h"
#include "Build.h"
#include <sstream>
#include <iostream>

Parser::Parser()
{

}

ParserData Parser::parse(const std::string & str)
{
    lxr.process(str);

    data.count = 0;

    TokenT token;

    token = lxr.getToken();
    if (token->getTokenType() == TokenType::DOUBLEHASH || token->getTokenType() == TokenType::ENDOFFILE)
        return data;

    lxr.unGetToken();

    do
    {
        processDef();
#if PRINT_SCRIPT
        std::cout << "\n";
#endif

        // Peak
        token = lxr.getToken();
        lxr.unGetToken();
    } while (token->getTokenType() != TokenType::DOUBLEHASH && token->getTokenType() != TokenType::ENDOFFILE);

    token = lxr.getToken();
    if (token->getTokenType() != TokenType::DOUBLEHASH)
        throw std::runtime_error("Missing ##");

    //token = lxr.getToken();
    //if (token->getTokenType() != TokenType::ENDOFFILE)
    //    throw std::runtime_error("File not empty.");

#if PRINT_SCRIPT
    std::cout << "##\n";
#endif

    return data;
}

void Parser::processDef()
{
    auto def_name = getString();
    auto token = lxr.getToken();

#if PRINT_SCRIPT
    if (!def_name.data.empty())
        std::cout << def_name.id << " (" << def_name.data << ") " << " -> ";
    else
        std::cout << def_name.id << " -> ";
#endif

    if (token->getTokenType() != TokenType::OPERATOR_DEF)
        throw std::runtime_error("Missing ->");

    if (!data.defmap.count(def_name))
        data.order.push_back(def_name);

    IDD_Container container;
    container.name = def_name;

    auto & m = data.rulemap[data.count++];
    m.name = def_name;
    m.idlist = processDefList();
    data.defmap[def_name].push_back(&m.idlist);
}

IDLIST Parser::processDefList()
{
    IDLIST ret;
    TokenT token;

    do
    {
        token = lxr.getToken();
        if (token->getTokenType() == TokenType::HASH)
        {
#if PRINT_SCRIPT
            std::cout << "#";
#endif
            return ret;
        }
        else if (token->getTokenType() == TokenType::CHAR || token->getTokenType() == TokenType::NUM)
        {
            lxr.unGetToken();
            auto name = getString();
            ret.push_back(name);
#if PRINT_SCRIPT
            if (!name.data.empty())
                std::cout << name.id << " (" << name.data << ") ";
            else
                std::cout << name.id << " ";
#endif
        }
        else
            throw std::runtime_error("");

        // Peak
        token = lxr.getToken();
        lxr.unGetToken();
    } while (token->getTokenType() != TokenType::HASH && token->getTokenType() != TokenType::ENDOFFILE);

    if (token->getTokenType() != TokenType::HASH)
        throw std::runtime_error("");

    token = lxr.getToken(); // drop off hash.
#if PRINT_SCRIPT
    std::cout << "#";
#endif

    return ret;
}

// This supports ID (PARAM), however, this is restricted in the Lexer 
// such that ID (PARAM) is not allowed.
std::string Parser::getString()
{
    std::ostringstream ss;

    auto token = lxr.getToken();
    do
    {
        if (token->getTokenType() == TokenType::CHAR)
        {
            auto chr = dynamic_cast<TokenChar&>(*token);
            ss << chr.data;
            if (chr.newline)
            {
                token = lxr.getToken();
                lxr.unGetToken();

                if (token->getTokenType() == TokenType::ENDOFFILE || token->getTokenType() == TokenType::DOUBLEHASH)
                    throw std::runtime_error("");

                if (isdigit(ss.str()[0]))
                    throw std::runtime_error("");

                return ss.str();
            }
        }
        else if (token->getTokenType() == TokenType::NUM)
        {
            auto num = dynamic_cast<TokenNum&>(*token);
            ss << num.num;
            if (num.newline)
            {
                token = lxr.getToken();
                lxr.unGetToken();

                if (token->getTokenType() == TokenType::ENDOFFILE || token->getTokenType() == TokenType::DOUBLEHASH)
                    throw std::runtime_error("");

                //if (isdigit(ss.str()[0]))
               //     throw std::runtime_error("");
                return ss.str();
            }
        }
        else
        {
            throw std::runtime_error("Invalid Token in identifer");
        }
        token = lxr.getToken();
    } while (token->getTokenType() == TokenType::CHAR || token->getTokenType() == TokenType::NUM);

    lxr.unGetToken(); // unget -> or #
    //if (isdigit(ss.str()[0]))
    //    throw std::runtime_error("");

    return ss.str();
}

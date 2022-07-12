/*
    Author:     Brandan Tyler Lasley (blasley@asu.edu)
    ASU ID:     1214587374
    Class:      CSE340S20 MWF 12:55
    Date:       2/1/2020
    Purpose:    Parses string of text into a vector of tokens then allows get/unget those tokens.
*/
#include "Build.h"
#include "Lexer.h"
#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <cctype>
#include <iostream>

Lexer::Lexer() : position(0)
{
}

Lexer::Lexer(const std::string & str) : position(0)
{
    process(str);
}

void Lexer::process(const std::string & str)
{
    tokens.clear();
    std::map<size_t, unsigned long long> lineMapPreProcess;
    const auto strvect = TokenizeString(str, lineMapPreProcess);
    processTokenInput(strvect, lineMapPreProcess);
}

const std::shared_ptr<TokenBase> Lexer::getToken()
{
    if (position < tokens.size())
        return tokens[position++];
    return std::shared_ptr<TokenEOF>(new TokenEOF);
}

void Lexer::unGetToken()
{
    position--;
}

void Lexer::reset()
{
    position = 0;
}

size_t Lexer::getPosition()
{
    return position;
}

std::vector<std::string> Lexer::TokenizeString(const std::string & str, std::map<size_t, unsigned long long> & lineMapPreProcess)
{
    std::vector<std::string> tokens_tmp;
    std::vector<std::string> tokens;
    std::string to;

    if (!str.empty())
    {
        std::stringstream ss(str);
        while (std::getline(ss, to, '\n'))
        {
            tokens_tmp.push_back(to);
        }
    }

    // We need a method to resolve token idx's to 
    // line numbers and this is the best way to do it.
    unsigned long long line = 0;
    size_t tokennum = 0;
    for (const auto & str : tokens_tmp)
    {
        std::stringstream ss(str);
        while (std::getline(ss, to, ' '))
        {
            tokens.push_back(to);
            lineMapPreProcess[tokennum++] = line;
        }
        line++;
    }

#if 0
    /// DEBUG DEBUG DEBUG ///
    int i = 0;
    for (auto &str : tokens)
    {
        std::cout << lineMapPreProcess[i++] << ": " << str << "\n";
    }
    /// DEBUG DEBUG DEBUG
#endif

    // This is a super simple way of doing what we want to accomplish.
    // Howevever this completely destroys line numbers. So I have to 
    // make a worse way to do it to meet project requirments.
    //std::istringstream iss(str);
    //std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));
    return tokens;
}

void Lexer::processTokenInput(const std::vector<std::string>& stringVect, const std::map<size_t, unsigned long long> & lineMapPreProcess)
{
    std::map<size_t, unsigned long long> lineMapPostProcess;
    size_t currToken = 0;
    size_t tokennum = 0;

    auto sortOutLineNumbers = [&]()
    {
        lineMapPostProcess[currToken++] = lineMapPreProcess.at(tokennum - 1);
        return lineMapPostProcess[currToken - 1];
    };
    
    for (const auto & word : stringVect)
    {
        tokennum++;
        if (word.empty()) continue;
        if (word == "POLY")
        {
            std::shared_ptr<TokenPoly> data(new TokenPoly);
            data->setLineNum(sortOutLineNumbers());
            tokens.push_back(data);
            continue;
        }
        else if (word == "START")
        {
            std::shared_ptr<TokenStart> data(new TokenStart);
            data->setLineNum(sortOutLineNumbers());
            tokens.push_back(data);
            continue;
        }
        else if (word == "INPUT")
        {
            std::shared_ptr<TokenInput> data(new TokenInput);
            data->setLineNum(sortOutLineNumbers());
            tokens.push_back(data);
            continue;
        }
        else if (word == "=")
        {
            std::shared_ptr<TokenEqual> data(new TokenEqual);
            data->setLineNum(sortOutLineNumbers());
            tokens.push_back(data);
            continue;
        }
        else if (word == "+")
        {
            std::shared_ptr<TokenPlusOperator> data(new TokenPlusOperator);
            data->setLineNum(sortOutLineNumbers());
            tokens.push_back(data);
            continue;
        }
        else if (word == "-")
        {
            std::shared_ptr<TokenMinusOperator> data(new TokenMinusOperator);
            data->setLineNum(sortOutLineNumbers());
            tokens.push_back(data);
            continue;
        }
        else if (word == ",")
        {
            std::shared_ptr<TokenComma> data(new TokenComma);
            data->setLineNum(sortOutLineNumbers());
            tokens.push_back(data);
            continue;
        }

        // Check if number.
        auto it = word.begin();
        while (it != word.end() && std::isdigit(*it)) ++it;

        // This is a int
        if (it == word.end())
        { 
            std::shared_ptr<TokenNum> data(new TokenNum);
            data->num = atoi(&word[0]);
            data->setLineNum(sortOutLineNumbers());
            tokens.push_back(data);
            continue;
        }

        std::vector<std::shared_ptr<TokenBase>> token_tmp;
        for (auto it = word.begin(); it != word.end(); it++)
        {
            const auto & letter = *it;
            if (letter == '^')
            {
                std::shared_ptr<TokenPower> data(new TokenPower);
                data->setLineNum(sortOutLineNumbers());
                token_tmp.push_back(data);
            }
            else if (letter == ';')
            {
                std::shared_ptr<TokenSemiColon> data(new TokenSemiColon);
                data->setLineNum(sortOutLineNumbers());
                token_tmp.push_back(data);
            }
            else if (letter == '=')
            {
                std::shared_ptr<TokenEqual> data(new TokenEqual);
                data->setLineNum(sortOutLineNumbers());
                token_tmp.push_back(data);
            }
            else if (letter == '+')
            {
                std::shared_ptr<TokenPlusOperator> data(new TokenPlusOperator);
                data->setLineNum(sortOutLineNumbers());
                token_tmp.push_back(data);
            }
            else if (letter == '-')
            {
                std::shared_ptr<TokenMinusOperator> data(new TokenMinusOperator);
                data->setLineNum(sortOutLineNumbers());
                token_tmp.push_back(data);
            }
            else if (letter == '(')
            {
                std::shared_ptr<TokenELParen> data(new TokenELParen);
                data->setLineNum(sortOutLineNumbers());
                token_tmp.push_back(data);
            }
            else if (letter == ')')
            {
                std::shared_ptr<TokenERParen> data(new TokenERParen);
                data->setLineNum(sortOutLineNumbers());
                token_tmp.push_back(data);
            }
            else if (letter == ',')
            {
                std::shared_ptr<TokenComma> data(new TokenComma);
                data->setLineNum(sortOutLineNumbers());
                token_tmp.push_back(data);
            }
            else if (isdigit(letter))
            {
                char letter2;
                std::string number;

                do
                {
                    letter2 = *it;
                    number += letter2;
                    it++;
                }   while (it != word.end() && isdigit(*it));

                if (number[0] == '0' && number.size() > 1)
                {
                    // Let's assume this is a char in instead.
                    for (auto c : number)
                    {
                        std::shared_ptr<TokenChar> data(new TokenChar);
                        data->data = c;
                        data->setLineNum(sortOutLineNumbers());
                        token_tmp.push_back(data);
                    }
                }
                else
                {
                    std::shared_ptr<TokenNum> data(new TokenNum);
                    data->num = atoi(&number[0]);
                    data->setLineNum(sortOutLineNumbers());
                    token_tmp.push_back(data);
                }

                it--;
            }
            else
            {
                // Some kind of char?
                if (isalpha(letter))
                {
                    std::shared_ptr<TokenChar> data(new TokenChar);
                    data->data = letter;
                    data->setLineNum(sortOutLineNumbers());
                    token_tmp.push_back(data);
                }
                else if (letter == '\t')
                    ; // Do nothing
#if USE_USEFUL_ERRORS
                else throw std::runtime_error("Illegal character.");
#else
                else throw std::runtime_error("SYNTAX ERROR !!!");
#endif
            }
        }

        for (auto c : token_tmp)
        {
            if (c->getTokenType() == TokenType::CHAR)
            {
                if (token_tmp.back() == c)
                {
                    auto & nc = dynamic_cast<TokenChar&>(*token_tmp.back());
                    nc.setSpace();
                }
            }
            tokens.push_back(c);
        }
    }

#if 0
    /// DEBUG DEBUG DEBUG ///
    for (auto &str : tokens)
    {
        std::cout << str->getLineNum()+1 << ": " << str->getTokenName() << " ( " << str->getTokenData() << " )\n";
    }
    /// DEBUG DEBUG DEBUG
#endif
}

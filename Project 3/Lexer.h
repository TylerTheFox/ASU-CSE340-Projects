/*
    Author:     Brandan Tyler Lasley (blasley@asu.edu)
    ASU ID:     1214587374
    Class:      CSE340S20 MWF 12:55
    Date:       3/29/2020
    Purpose:    Parses string of text into a vector of tokens then allows get/unget those tokens.
*/
#pragma once
#include "Token.h"
#include <string>
#include <memory>
#include <vector>
#include <map>

typedef std::shared_ptr<TokenBase> TokenT;

class Lexer
{
public:
    Lexer();
    Lexer(const std::string& str);
    void process(const std::string& str);

    const std::shared_ptr<TokenBase> getToken();
    void unGetToken();
    void reset();

    void   setPosition(size_t pos);
    size_t getPosition();
private:
    size_t position;
    std::vector<std::shared_ptr<TokenBase>> tokens;

    std::vector<std::string>    TokenizeString(const std::string & str, std::map<size_t, unsigned long long> & lineMapPreProcess);
    void                        processTokenInput(const std::vector<std::string>& stringVect, const std::map<size_t, unsigned long long> & lineMapPreProcess);
};
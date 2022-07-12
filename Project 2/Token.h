/*
    Author:     Brandan Tyler Lasley (blasley@asu.edu)
    ASU ID:     1214587374
    Class:      CSE340S20 MWF 12:55
    Date:       2/1/2020
    Purpose:    This is a polymorphic class which handles all the different types of tokens.
                Some data structures in here are unused. Although the project spec does not list
                a multiply operator OPERATOR_MULTIPLY exists only internally for cases such as XY or X * Y.
                XY will be translated internally to X * Y with the OPERATOR_MULTIPLY.
*/
#pragma once
#include <string>
#include <memory>

enum class TokenType
{
    NONE,
    ID,
    NUM,
    TERM,
    CHAR,
    LETTER,
    PDIGIT,
    DIGIT,
    SEMICOLON,
    COMMA,
    OPERATOR_PLUS,
    OPERATOR_MINUS,
    POWER,
    EQUAL,
    LPAREN,
    RPAREN,
    POLY,
    START,
    INPUT,
    OPERATOR_MULTIPLY,
    OPERATOR_DEF,
    HASH,
    DOUBLEHASH,
    ENDOFFILE
};

const std::string TokenNames[] =
{
    "NONE",
    "ID",
    "NUM",
    "TERM",
    "CHAR",
    "LETTER",
    "PDIGIT",
    "DIGIT",
    "SEMICOLON",
    "COMMA",
    "OPERATOR_PLUS",
    "OPERATOR_MINUS",
    "POWER",
    "EQUAL",
    "LPAREN",
    "RPAREN",
    "POLY",
    "START",
    "INPUT",
    "OPERATOR_MULTIPLY",
    "OPERATOR_DEF",
    "HAHS",
    "DOUBLEHASH",
    "ENDOFFILE"
};

class TokenBase
{
public:
    TokenBase() : line_num(0) {}
    virtual ~TokenBase() {}
    virtual TokenType getTokenType() const = 0;
    virtual std::string getTokenData() const = 0;
    const std::string & getTokenName()
    {
        const auto type = this->getTokenType();
        return TokenNames[static_cast<size_t>(type)];
    }

    void setLineNum(const unsigned long long num)
    {
        line_num = num;
    }

    unsigned long long getLineNum() const
    {
        return line_num;
    }
private:
    unsigned long long line_num;
};

// -
struct TokenMinusOperator : public TokenBase
{
    ~TokenMinusOperator() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::OPERATOR_MINUS;
    }
};

// +
struct TokenPlusOperator : public TokenBase
{
    ~TokenPlusOperator() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::OPERATOR_PLUS;
    }
};

// 0 | pdigit . digit*
struct TokenNum : public TokenBase
{
    TokenNum() : newline(false), num(0) {}
    ~TokenNum() {}

    std::string getTokenData() const
    {
        return std::to_string(num);
    }

    TokenType getTokenType() const
    {
        return TokenType::NUM;
    }

    void setSpace()
    {
        newline = true;
    }

    bool isNextCharSpace()
    {
        return newline;
    }

    bool newline;
    long long num;
};

// 2x^2
struct TokenTerm : public TokenBase
{
    TokenTerm() : num(1), power(1) {}
    ~TokenTerm() {}

    std::string getTokenData() const
    {
        return std::to_string(num) + " " + var + " " + std::to_string(power);
    }

    TokenType getTokenType() const
    {
        return TokenType::TERM;
    }

    long long num;
    std::string var;
    long long power;
};

// a | b | ... | z | A | B | ... | Z  | 0 |  1 | ... | 9
struct TokenChar : public TokenBase
{
    TokenChar() : newline(false), data(0) {}
    ~TokenChar() {}

    std::string getTokenData() const
    {
        return std::string(&data, 1);
    }

    TokenType getTokenType() const
    {
        return TokenType::CHAR;
    }

    void setSpace()
    {
        newline = true;
    }

    bool isNextCharSpace()
    {
        return newline;
    }

    bool newline;
    char data;
};

// a | b | ... | z | A | B | ... | Z
struct TokenLetter : public TokenBase
{
    TokenLetter() : data(0) {}
    ~TokenLetter() {}

    std::string getTokenData() const
    {
        return std::string(&data, 1);
    }

    TokenType getTokenType() const
    {
        return TokenType::LETTER;
    }

    char data;
};

// 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
struct TokenPDigit : public TokenBase
{
    TokenPDigit() : data(0) {}
    ~TokenPDigit() {}

    std::string getTokenData() const
    {
        return std::to_string(data);
    }

    TokenType getTokenType() const
    {
        return TokenType::PDIGIT;
    }

    unsigned int data;
};

// 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
struct TokenDigit : public TokenBase
{
    TokenDigit() : data(0) {}
    ~TokenDigit() {}

    std::string getTokenData() const
    {
        return std::to_string(data);
    }

    TokenType getTokenType() const
    {
        return TokenType::PDIGIT;
    }

    unsigned int data;
};

// ;
struct TokenSemiColon : public TokenBase
{
    TokenSemiColon() {}
    ~TokenSemiColon() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::SEMICOLON;
    }
};

// ,
struct TokenComma : public TokenBase
{
    TokenComma() {}
    ~TokenComma() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::COMMA;
    }
};

// ^
struct TokenPower : public TokenBase
{
    TokenPower() {}
    ~TokenPower() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::POWER;
    }
};

// =
struct TokenEqual : public TokenBase
{
    TokenEqual() {}
    ~TokenEqual() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::EQUAL;
    }
};

// (
struct TokenELParen : public TokenBase
{
    TokenELParen() {}
    ~TokenELParen() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::LPAREN;
    }
};

// )
struct TokenERParen : public TokenBase
{
    TokenERParen() {}
    ~TokenERParen() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::RPAREN;
    }
};

// POLY
struct TokenPoly : public TokenBase
{
    TokenPoly() {}
    ~TokenPoly() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::POLY;
    }
};

// START
struct TokenStart : public TokenBase
{
    TokenStart() {}
    ~TokenStart() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::START;
    }
};

// INPUT
struct TokenInput : public TokenBase
{
    TokenInput() {}
    ~TokenInput() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::INPUT;
    }
};

// EOF
struct TokenEOF : public TokenBase
{
    TokenEOF() {}
    ~TokenEOF() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::ENDOFFILE;
    }
};

// EOF
struct TokenDoubleHash : public TokenBase
{
    TokenDoubleHash() {}
    ~TokenDoubleHash() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::DOUBLEHASH;
    }
};

// Multiply
struct TokenMultiply : public TokenBase
{
    TokenMultiply() {}
    ~TokenMultiply() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::OPERATOR_MULTIPLY;
    }
};

struct TokenOperatorDef : public TokenBase
{
    TokenOperatorDef() {}
    ~TokenOperatorDef() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::OPERATOR_DEF;
    }
};

struct TokenHash : public TokenBase
{
    TokenHash() {}
    ~TokenHash() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::HASH;
    }
};

struct TokenID : public TokenBase
{
    TokenID() {}
    ~TokenID() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::ID;
    }
};
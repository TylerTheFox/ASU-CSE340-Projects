/*
    Author:     Brandan Tyler Lasley (blasley@asu.edu)
    ASU ID:     1214587374
    Class:      CSE340S20 MWF 12:55
    Date:       3/29/2020
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
    NUM,
    CHAR,
    SEMICOLON,
    COMMA,
    COLON,
    OPERATOR_PLUS,
    OPERATOR_MINUS,
    OPERATOR_MULTIPLY,
    OPERATOR_DIV,
    OPERATOR_GREATER_THAN,
    OPERATOR_LESS_THAN,
    EQUAL,
    NOTEQUAL,
    LPAREN,
    RPAREN,
    LBRACKET,
    RBRACKET,
    KEYWORD_VAR,
    KEYWORD_FOR,
    KEYWORD_IF,
    KEYWORD_WHILE,
    KEYWORD_SWITCH,
    KEYWORD_CASE,
    KEYWORD_DEFAULT,
    KEYWORKD_INPUT,
    KEYWORD_OUTPUT,
    KEYWORD_ARRAY,
    ENDOFFILE
};

const std::string TokenNames[] =
{
    "NONE",
    "NUM",
    "CHAR",
    "COLON",
    "SEMICOLON",
    "COMMA",
    "OPERATOR_PLUS",
    "OPERATOR_MINUS",
    "OPERATOR_MULTIPLY",
    "OPERATOR_DIV",
    "OPERATOR_GREATER_THAN",
    "OPERATOR_LESS_THAN",
    "EQUAL",
    "NOTEQUAL",
    "LPAREN",
    "RPAREN",
    "LBRACKET",
    "RBRACKET",
    "KEYWORD_VAR",
    "KEYWORD_FOR",
    "KEYWORD_IF",
    "KEYWORD_WHILE",
    "KEYWORD_SWITCH",
    "KEYWORD_CASE",
    "KEYWORD_DEFAULT",
    "KEYWORKD_INPUT",
    "KEYWORD_OUTPUT",
    "KEYWORD_ARRAY",
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

// Dividion
struct TokenDivision : public TokenBase
{
    TokenDivision() {}
    ~TokenDivision() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::OPERATOR_DIV;
    }
};

// Less Than
struct TokenLessThan : public TokenBase
{
    TokenLessThan() {}
    ~TokenLessThan() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::OPERATOR_LESS_THAN;
    }
};

// Greater Than
struct TokenGreaterThan : public TokenBase
{
    TokenGreaterThan() {}
    ~TokenGreaterThan() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::OPERATOR_GREATER_THAN;
    }
};

// Not Equal
struct TokenNotEqual : public TokenBase
{
    TokenNotEqual() {}
    ~TokenNotEqual() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::NOTEQUAL;
    }
};

// Left Bracket
struct TokenLeftBracket : public TokenBase
{
    TokenLeftBracket() {}
    ~TokenLeftBracket() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::LBRACKET;
    }
};

// Right Bracket
struct TokenRightBracket : public TokenBase
{
    TokenRightBracket() {}
    ~TokenRightBracket() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::RBRACKET;
    }
};

// Var
struct TokenVar : public TokenBase
{
    TokenVar() {}
    ~TokenVar() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::KEYWORD_VAR;
    }
};

// For
struct TokenFor : public TokenBase
{
    TokenFor() {}
    ~TokenFor() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::KEYWORD_FOR;
    }
};

// IF
struct TokenIF : public TokenBase
{
    TokenIF() {}
    ~TokenIF() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::KEYWORD_IF;
    }
};

// WHILE
struct TokenWHILE : public TokenBase
{
    TokenWHILE() {}
    ~TokenWHILE() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::KEYWORD_WHILE;
    }
};

// SWITCH
struct TokenSWITCH : public TokenBase
{
    TokenSWITCH() {}
    ~TokenSWITCH() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::KEYWORD_SWITCH;
    }
};

// CASE
struct TokenCASE : public TokenBase
{
    TokenCASE() {}
    ~TokenCASE() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::KEYWORD_CASE;
    }
};

// DEFAULT
struct TokenDEFAULT : public TokenBase
{
    TokenDEFAULT() {}
    ~TokenDEFAULT() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::KEYWORD_DEFAULT;
    }
};

// INPUT
struct TokenINPUT : public TokenBase
{
    TokenINPUT() {}
    ~TokenINPUT() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::KEYWORKD_INPUT;
    }
};

// OUTPUT
struct TokenOUTPUT : public TokenBase
{
    TokenOUTPUT() {}
    ~TokenOUTPUT() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::KEYWORD_OUTPUT;
    }
};

// OUTPUT
struct TokenARRAY : public TokenBase
{
    TokenARRAY() {}
    ~TokenARRAY() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::KEYWORD_ARRAY;
    }
};

// COLON
struct TokenCOLON : public TokenBase
{
    TokenCOLON() {}
    ~TokenCOLON() {}

    std::string getTokenData() const
    {
        return {};
    }

    TokenType getTokenType() const
    {
        return TokenType::COLON;
    }
};

#pragma once
#include "Lexer.h"

class ScopedLexer
{
public:
    ScopedLexer(Lexer & l, bool resetOnExit = false);
    ~ScopedLexer();
    void reset();
private:
    bool    r;
    size_t  pos;
    Lexer * lex;
};
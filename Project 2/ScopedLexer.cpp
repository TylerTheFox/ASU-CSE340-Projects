#include "ScopedLexer.h"
#include <cassert>

ScopedLexer::ScopedLexer(Lexer & l, bool resetOnExit) : r(resetOnExit), lex(&l)
{
    pos = l.getPosition();
}

ScopedLexer::~ScopedLexer()
{
    assert(lex);
    if (r)
        lex->setPosition(pos);
    lex = nullptr;
}

void ScopedLexer::reset()
{
    r = true;
}

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cctype>
#include <cstring>
#include <string>
#include <iostream>
#include "compiler.h"
#include "Parser.h"

struct InstructionNode * parse_generate_intermediate_representation()
{
    std::string data, line;
    for (std::string line; std::getline(std::cin, line);)
    {
        data += line + '\n';
    }

    Parser psr;
    auto prog = psr.parse(data);
    return prog;
}

#pragma once
#include "Parser.h"
#include "Grammar.h"
#include <set>
#include <map>
#include <unordered_set>

class Task4
{
public:
    Task4() = delete;

    static void run(ParserData & d);

    static bool calculateFollow(ParserData & d, Grammar & g, std::map<std::string, std::unordered_set<std::string>>& first, std::map<int, Definition>& follow);
};
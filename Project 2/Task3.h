#pragma once
#include "Parser.h"
#include <unordered_set>

class Task3
{
public:
    Task3() = delete;

    static void run(ParserData & d);
    static bool calculateFirstSet(ParserData & d, const std::string& def, std::map<std::string, std::unordered_set<std::string>> & first, std::map<std::string, bool> & func_call, const std::string* rdef = nullptr, bool resursive = false);
    static std::map<std::string, std::unordered_set<std::string>> getFirst(ParserData & d);
};
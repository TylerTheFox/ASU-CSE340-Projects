#pragma once
#include "Parser.h"

class Task2
{
public:
    Task2() = delete;

    static void run(ParserData & d);

    static bool isTerminal(ParserData & d, const std::string & term);
    static void remove_all_elements_not_in_list(ParserData & d);
private:
    static void print_grammar(ParserData & d);

    static std::set<std::string> findGeneratingSymbols(ParserData & d);
    static std::set<std::string> findReachableSymbols(ParserData & d, const std::set<std::string> & generating);
    static void cleanUpUselessGrammar(ParserData & d, const std::set<std::string> & generating, const std::set<std::string> & reachable);
};
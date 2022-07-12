#include "Task3.h"
#include <map>
#include <unordered_map>
#include <iostream>
#include <algorithm>

#include "Task2.h"
#include "Task1.h"

#include "Grammar.h"

void Task3::run(ParserData & d)
{
    auto first = getFirst(d);
    Grammar g(d);

    std::unordered_map<int, std::set<int>> first_sym;

    auto& defs = g.getDef();

    // Init non-terminals.
    const auto & t = g.getTerminalMap();
    for (const auto & term : t)
    {
        if (!term.second)
            first_sym[term.first] = {};
    }

    for (const auto& def : first)
    {
        for (const auto& rule : def.second)
        {
            first_sym[g.getSymbolNumber(def.first)].insert(g.getSymbolNumber(rule));
        }
    }

    struct sort_symbol
    {
        int symbol;
        std::set<int> symbol_list;

        bool operator <(const sort_symbol& pt) const
        {
            return symbol < pt.symbol;
        }
    };

    std::set<sort_symbol> sorted;

    for (const auto & s : first_sym)
    {
        struct sort_symbol ss;
        ss.symbol = s.first;
        ss.symbol_list = s.second;
        sorted.insert(ss);
    }

    for (auto& def_sym : sorted)
    {
        bool first_it = true;
        std::cout << "FIRST(" << g.getSymbolName(def_sym.symbol) << ") = { ";
        for (const auto& rule : def_sym.symbol_list)
        {
            if (!first_it)
                std::cout << ", ";

            std::cout << g.getSymbolName(rule);

            first_it = false;
        }
        std::cout << " }\n";
    }
}

/*
https://kartikkukreja.wordpress.com/2013/04/24/first-set-of-each-variable-in-a-cfg-c-implementation/

Calculating First sets
    1. If X is a terminal then First(X) is just X!
    2. If there is a Production X → ε then add ε to first(X)
    3. If there is a Production X → Y1Y2..Yk then add first(Y1Y2..Yk) to first(X)
    4. First(Y1Y2..Yk) is either
        1. First(Y1) (if First(Y1) doesn't contain ε)
        2. OR (if First(Y1) does contain ε) then First (Y1Y2..Yk) is everything in First(Y1) <except for ε > as well as everything in First(Y2..Yk)
        3. If First(Y1) First(Y2)..First(Yk) all contain ε then add ε to First(Y1Y2..Yk) as well.
*/
bool hasEplison(ParserData & d, std::map<std::string, std::unordered_set<std::string>> & first, const std::string& rule)
{
    // Easily detectable eplison.
    for (const auto & def_rulelist : d.defmap[rule])
        if (!def_rulelist->size())
            return true;

    //if (first[rule].find("#") != first[rule].end())
    //    return true;

    return false;
}

bool Task3::calculateFirstSet(ParserData & d, const std::string& def, std::map<std::string, std::unordered_set<std::string>> & first, std::map<std::string, bool> & func_call, const std::string* rdef, bool resursive)
{
    bool ep = false;
    func_call[def] = true;

    // Generate Rule 1 and Rule 2
    //if (!resursive)
    {
        for (const auto & def_rulelist : d.defmap[def])
        {
            if (def_rulelist->size())
            {
                if (Task2::isTerminal(d, (*def_rulelist)[0]))
                {
                    first[def].insert((*def_rulelist)[0]);
                }
            }
            else first[def].insert("#");
        }
    }

    // Rule 5
    //if (!resursive)
    {
        for (const auto & defs : d.defmap[def])
        {
            if (defs->size())
            {
                for (const auto & rule : *defs)
                {
                    if (!Task2::isTerminal(d, rule) && hasEplison(d, first, rule))
                    {
                        first[def].insert("#");
                    } else if (Task2::isTerminal(d, rule))
                        break;
                }
            }
        }
    }

    // Rule 3 and Rule 4
    for (const auto & def_rulelist : d.defmap[def])
    {
        if (def_rulelist->size())
        {
            for (const auto & rule : *def_rulelist)
            {
                if (Task2::isTerminal(d, rule))
                {
                    if (rdef)
                        first[*rdef].insert(rule);
                    first[def].insert(rule);
                }
                else
                {
                    // Rule 3 
                    if (!func_call[rule]) // Prevents infinte loops.
                    {
                        if (resursive)
                        {
                            if (calculateFirstSet(d, rule, first, func_call, rdef, true))
                            {
                                ep = true;
                                continue;
                            }
                        }
                        else
                        {
                            if (calculateFirstSet(d, rule, first, func_call, &def, true))
                            {
                                ep = true;
                                continue;
                            }
                        }
                    }
                    else if (hasEplison(d, first, rule) || (first[rule].find("#") != first[rule].end()))
                    {
                        // Rule 3.1 (Has eplison)
                        continue;
                    }

                }
                break;
            }
        }
        else
        {
            first[def].insert("#");
            ep = true;
        }
    }
    return ep;
}

std::map<std::string, std::unordered_set<std::string>> Task3::getFirst(ParserData & d)
{
    std::map<std::string, bool> function_call;
    std::map<std::string, std::unordered_set<std::string>> first;

    //  Calulcate first set
    for (const auto & def : d.order)
    {
        calculateFirstSet(d, def, first, function_call);
        function_call.clear();
    }

    return first;
}

#include "Task2.h"
#include <map>
#include <iostream>
#include <set>
#include <map>
#include <stack>

void Task2::run(ParserData & d)
{
    auto generating = Task2::findGeneratingSymbols(d);
    auto reachable = Task2::findReachableSymbols(d, generating);
    Task2::cleanUpUselessGrammar(d, generating, reachable);
    Task2::remove_all_elements_not_in_list(d);
    Task2::print_grammar(d);
}

/*
http://kilby.stanford.edu/~rvg/154/handouts/useless.html
Call a variable generating if it derives a string of terminals. Note that the language accepted by a context-free grammar is non-empty if and only if the start symbol is generating. Here is an algorithm to find the generating variables in a CFG:
    1. Mark a variable X as "generating" if it has a production X -> w where w is a string of only terminals and/or variables previously marked "generating".
    2. Repeat the step above until no further variables get marked "generating".
All variables not marked "generating" are non-generating (by a simple induction on the length of derivations).
*/
std::set<std::string> Task2::findGeneratingSymbols(ParserData & d)
{
    std::set<std::string> generating;

    // Mark a variable X as "generating" if it has a production X -> w where w is a string of only terminals
    for (const auto& r : d.rulemap)
    {
        bool only_terminals = true;
        for (const auto& rule : r.second.idlist)
            if (!Task2::isTerminal(d, rule))
                only_terminals = false;

        // This should also pass eplsion.
        if (only_terminals)
            generating.insert(r.second.name);
    }

    // Now that all non-terminals are added with terminals only. 
    // Mark a variable X as "generating" is the ruleset contains previously marked 
    // generating symbols.
    bool am_i_completely_generating;

    bool changed = true;
    while (changed)
    {
        changed = false;
        for (int i = 0; i < d.count; i++)
        {
            const auto & rulet = d.rulemap[i];
            const auto & def_name = rulet.name;

            if (generating.find(def_name) == generating.end())
            {
                am_i_completely_generating = true;

                for (auto const & rule : rulet.idlist)
                    if (!Task2::isTerminal(d, rule))
                        if (generating.find(rule) == generating.end())
                            am_i_completely_generating = false;

                if (am_i_completely_generating)
                {
                    generating.insert(def_name);
                    changed = true;
                }
            }
        }
    }

    // Check if the start symbol is generating
    const auto & start = d.order[0];
    if (generating.find(start) == generating.end())
        generating.clear(); // Grammar is not generating.

    return generating; 
}

/*
http://kilby.stanford.edu/~rvg/154/handouts/useless.html
Call a variable reachable if the staart symbol derives a string containing that variable. Here is an algorithm to find the reachable variables in a CFG:
    1. Mark the start variable as "reachable".
    2. Mark a variable Y as "reachable" if there is a production X -> w where X is a variable previously marked as "reachable" and w is a string containing Y.
    3. Repeat the step above until no further variables get marked "reachable".
All variables not marked "reachable" are non-reachable (by a simple induction on the length of derivations).
*/
std::set<std::string> Task2::findReachableSymbols(ParserData & d, const std::set<std::string> & generating)
{
    std::set<std::string> reachable;
    std::stack<std::string> s;

    if (generating.size())
    {
        const auto & start = d.order[0];
        s.push(start);

        while (!s.empty())
        {
            auto front = s.top();
            s.pop();

            // Make sure not reachable already
            if (reachable.find(front) == reachable.end())
            {
                reachable.insert(front);

                for (const auto & rulelist : d.defmap[front])
                {
                    for (const auto & rule : *rulelist)
                    {
                        if (!Task2::isTerminal(d, rule))
                        {
                            s.push(rule);
                            if (generating.find(rule) == generating.end()) break;
                        }
                    }
                }
            }
        }
    }
    return reachable;
}

/*
http://kilby.stanford.edu/~rvg/154/handouts/useless.html
Observe that a CFG has no useless variables if and only if all its variables are reachable and generating.
Therefore it is possible to eliminate useless variables from a grammar as follows:
    1. Find the non-generating variables and delete them, along with all productions involving non-generating variables.
    2. Find the non-reachable variables in the resulting grammar and delete them, along with all productions involving non-reachable variables.
Note that step 1 does not make other variables non-generating, and step 2 does not make other variables non-reachable or non-generating. Therefore the end result is a grammar in which all variables are reachable and generating, and hence useful.
*/
void Task2::cleanUpUselessGrammar(ParserData & d, const std::set<std::string>& generating, const std::set<std::string>& reachable)
{
    for (auto rulet_it = d.rulemap.begin(); rulet_it != d.rulemap.end();)
    {
        auto & rulet = rulet_it->second;
        const auto & def_name = rulet.name;
        bool deleted_elm = false;
        for (auto rule_it = rulet.idlist.begin(); rule_it != rulet.idlist.end();)
        {
            if (!Task2::isTerminal(d, *rule_it))
            {
                // If a rule is non generating OR not reachable.
                if (generating.find(*rule_it) == generating.end() || reachable.find(*rule_it) == reachable.end())
                {
                    rule_it = rulet.idlist.erase(rule_it);
                    deleted_elm = true;
                    continue;
                }
            }
            rule_it++;
        }
        
        if (deleted_elm || reachable.find(def_name) == reachable.end())
        {
            rulet_it = d.rulemap.erase(rulet_it);
            continue;
        }
        rulet_it++;
    }
}

bool Task2::isTerminal(ParserData & d, const std::string & term)
{
    return d.defmap.count(term) == 0;
}

void Task2::print_grammar(ParserData & d)
{
    // Print grammar
    for (int i = 0; i < d.count; i++)
    {
        if (d.rulemap.count(i))
        {
            auto & rulelist = d.rulemap[i];
            std::cout << rulelist.name << " -> ";
            if (!rulelist.idlist.size())
            {
                std::cout << "#\n";
                continue;
            }
            else
            {
                for (const auto & rule : rulelist.idlist)
                {
                    std::cout << rule << " ";
                }
            }
            std::cout << '\n';
        }
    }
}

void Task2::remove_all_elements_not_in_list(ParserData & d)
{
    // Remove all items not in rulelist
    for (auto rulellist = d.rulemap.begin(); rulellist != d.rulemap.end();)
    {
        // Check if rulelist is not in the defmap
        bool found = false;
        for (const auto & def : d.defmap)
        {
            for (const auto & v : def.second)
            {
                if (rulellist->second.idlist == *v && def.first == rulellist->second.name)
                {
                    found = true;
                }
            }
        }

        if (!found) rulellist = d.rulemap.erase(rulellist);
        else        rulellist++;
    }
}

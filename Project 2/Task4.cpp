#include "Task4.h"
#include "Grammar.h"
#include <unordered_map>
#include <set>
#include <iostream>
#include "Task3.h"
#include "Task2.h"

void Task4::run(ParserData & d)
{
    Grammar g(d);
    auto first = Task3::getFirst(d);
    std::map<int, Definition> follow;
    const auto& start = g.getSymbolNumber(d.rulemap[0].name);
    Definition df(start, &g);
    df.addRule(FOLLOWEOF);
    follow[start] = df;
    calculateFollow(d, g, first, follow);
    for (auto& def : follow)
    {
        bool first_run = true;
        std::cout << "FOLLOW(" << g.getSymbolName(def.first) << ") = { ";
        const auto & rules = def.second.getRules();
        for (const auto& rule : rules)
        {
            if (!first_run)
                std::cout << ", ";
            std::cout << g.getSymbolName(rule);
            first_run = false;
        }
        std::cout << " }\n";
    }
}

bool Task4::calculateFollow(ParserData & d, Grammar & g, std::map<std::string, std::unordered_set<std::string>>& first, std::map<int, Definition> & follow)
{
    std::vector<const IDLIST*> found_rules;
    std::map<std::string, std::vector<std::string>> next_var;
    std::map<std::string, std::vector<std::string>> follow_var;

    for (const auto & deflist : d.defmap)
    {
        for (const auto& outter_rule_data : d.rulemap)
        {
            for (const auto& rule : outter_rule_data.second.idlist)
            {
                if (rule == deflist.first)
                {
                    found_rules.push_back(&outter_rule_data.second.idlist);
                    break;
                }
            }
        }

        bool add_next_var = false;
        bool all_eplison = false;
        // We found all the rules, let's get the next var.
        for (const auto & rules : found_rules)
        {
            for (const auto & rule : *rules)
            {
                if (add_next_var)
                {
                    next_var[deflist.first].push_back(rule);
                    if (first[rule].find("#") == first[rule].end())
                    {
                        add_next_var = false;
                        all_eplison = false;
                    }
                }

                if (rule == deflist.first)
                {
                    add_next_var = true;
                    all_eplison = true;
                }
            }

            if (all_eplison)
            {
                // Check if start symbol
                if (&d.rulemap[0].idlist == rules)
                {
                    follow_var[deflist.first].push_back(d.rulemap[0].name);
                    next_var[deflist.first].push_back("$");
                }
                else
                {
                    // Find out who this is.
                    std::string follow_to_add;
                    for (const auto & outter_rule : d.rulemap)
                    {
                        if (&outter_rule.second.idlist == rules)
                        {
                            follow_to_add = outter_rule.second.name;
                            break;
                        }
                    }
                    follow_var[deflist.first].push_back(follow_to_add);
                }

                all_eplison = false;
            }

            add_next_var = false;
        }
        // Clean up.
        found_rules.clear();
    }

    // Add firsts & terminals
    for (const auto & var : next_var)
    {
        int my_number = g.getSymbolNumber(var.first);

        // Resolve next vars
        for (const auto& v : var.second)
        {
            if (Task2::isTerminal(d, v))
            {
                if (!follow.count(g.getSymbolNumber(var.first)))
                {
                    Definition df(g.getSymbolNumber(var.first), &g);
                    df.addRule(v);
                    follow[my_number] = df;
                }
                else follow[my_number].addRule(v);
            }
            else
            {
                for (const auto& r : first[v])
                {
                    if (r != "#")
                    {
                        if (!follow.count(my_number))
                        {
                            Definition df(my_number, &g);
                            df.addRule(r);
                            follow[my_number] = df;
                        }
                        else follow[my_number].addRule(r);
                    }
                }
            }
        }
    }

    // Add follows
    bool changed = true;
    while (changed) // Requires 2 passes.
    {
        changed = false;
        for (auto f = follow_var.rbegin(); f != follow_var.rend(); f++)
        {
            const auto& follow_to_add_to = f->first;
            int follow_to_add = g.getSymbolNumber(follow_to_add_to);
            if (!follow.count(follow_to_add))
            {
                Definition df(follow_to_add, &g);
                follow[follow_to_add] = df;
            }

            auto& term = follow[follow_to_add];

            for (const auto& r : f->second)
            {
                auto& term = follow[g.getSymbolNumber(r)];
                const auto count = term.size();
                for (int i = 0; i < count; i++)
                {
                    const auto& rule = term.getNextRule();
                    if (rule.getSymbol() != EPSLION)
                    {
                        if (!follow.count(g.getSymbolNumber(f->first)))
                        {
                            Definition df(g.getSymbolNumber(f->first), &g);
                            df.addRule(rule.getSymbol());
                            follow[g.getSymbolNumber(f->first)] = df;
                            changed = true;
                        }
                        else
                        {
                            if (!follow[g.getSymbolNumber(f->first)].doesRuleExist(rule.getSymbol()))
                            {
                                follow[g.getSymbolNumber(f->first)].addRule(rule.getSymbol());
                                changed = true;
                            }
                        }
                    }
                }
                term.resetPosition();
            }
        }
    }
    return false;
}

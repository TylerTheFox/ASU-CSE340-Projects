#include "Task1.h"
#include <iostream>

void Task1::run(ParserData & d)
{
    std::map<std::string, bool> alreadyPrinted;

    // Terminals
    for (int i = 0; i < d.count; i++)
    {
        const auto & rulelist = d.rulemap.at(i);

        for (const auto & rule : rulelist.idlist)
        {
            // If not a non terminal
            if (!d.defmap.count(rule) && !alreadyPrinted[rule])
            {
                std::cout << rule << " ";
                alreadyPrinted[rule] = true;
            }
        }
    }

    // Non Terminals
    for (int i = 0; i < d.count; i++)
    {
        const auto & rulelist = d.rulemap[i];

        if (!alreadyPrinted[rulelist.name])
        {
            std::cout << rulelist.name << " ";
            alreadyPrinted[rulelist.name] = true;
        }

        for (const auto & rule : rulelist.idlist)
        {
            // If not a non terminal
            if (d.defmap.count(rule) && !alreadyPrinted[rule])
            {
                std::cout << rule << " ";
                alreadyPrinted[rule] = true;
            }
        }
    }
}
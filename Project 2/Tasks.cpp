#include "Tasks.h"
#include "Task1.h"
#include "Task2.h"
#include "Task3.h"
#include "Task4.h"
#include "Task5.h"

#include "Build.h"
#include "Parser.h"

#include <iostream>

void Tasks::do_task(const std::string & input, Task job)
{
    try
    {
        Parser psr;
        auto dat = psr.parse(input);
        switch (job)
        {
        case Task::PrintTerminals:
            Task1::run(dat);
            break;
        case Task::RemoveUselessSymbols:
            Task2::run(dat);
            break;
        case Task::PrintFirstSet:
            Task3::run(dat);
            break;
        case Task::PrintFollowSet:
            Task4::run(dat);
            break;
        case Task::PrintGrammarPredictiveParser:
            Task5::run(dat);
            break;
        default:
            throw std::runtime_error("Invalid Task");
        }
    }
    catch (const std::exception & e)
    {
#if USE_USEFUL_ERRORS
        std::cout << "Error: " << e.what();
#else
        e; // make warning go away.
        std::cout << "SYNTAX ERROR !!!";
#endif
    }
}

#pragma once
#include <string>

enum class Task
{
    PrintTerminals = 1,
    RemoveUselessSymbols,
    PrintFirstSet,
    PrintFollowSet,
    PrintGrammarPredictiveParser
};

class Tasks
{
public:
    Tasks() = delete;

    static void do_task(const std::string & input, Task job);
};
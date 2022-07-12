#include "Tasks.h"
#include <string>
#include <iostream>

//#undef BRANDAN

int main(int argc, char* argv[])
{
#if BRANDAN
        const std::string test =
R"(
S -> A B e F D #
S -> B C d E F #
A -> A a B #
A -> A A b B #
A -> #
B -> C A #
B -> c a a a a C b b b b A #
C -> #
C -> C c C #
C -> E B #
D -> E F #
E -> F E #
F -> D z D #
F -> C C #
##

)";
#else 
        std::string test;
#endif
    int task;
    if (argc < 2)
    {
        std::cout << "Error: missing argument\n";
        return 1;
    }
    task = atoi(argv[1]);

    // Read input from standard in
    std::string data;
    if (test.empty())
    {
        for (std::string line; std::getline(std::cin, line);)
        {
            data += line + '\n';
        }
    } else data = test;

    Tasks::do_task(data, static_cast<Task>(task));

#if BRANDAN
    std::cin.get();
#endif
    return 0;
}
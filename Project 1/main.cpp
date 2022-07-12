/*
    Author:     Brandan Tyler Lasley (blasley@asu.edu)
    ASU ID:     1214587374
    Class:      CSE340S20 MWF 12:55
    Date:       2/1/2020
    Purpose:    This is the main entry point of the program.
*/
#include "Build.h"
#include "Tests.h"
#include <iostream>
#include <string>

int main()
{
#if SELF_CHECK
    // We check the parser with known inputs and outputs 
    // every run to ensure the parser is working correctly
    // before trying to parse. This helps to idenify any code
    // changes that might cause the parser to fail or change 
    // behavior with some inputs but not others.
    run_self_check_no_error();
    self_check_syntax_error();
    self_check_error_codes(SemanticErrors::DuplicatePolyName);
    self_check_error_codes(SemanticErrors::InvalidMonomialName);
    self_check_error_codes(SemanticErrors::EvaluationOfUnDeclaredPoly);
    self_check_error_codes(SemanticErrors::IncorrectArguments);
    self_check_error_codes(SemanticErrors::UninitalizedArgument);
#endif

    // Read input from standard in
    std::string data;
    for (std::string line; std::getline(std::cin, line);)
    {
        data += line + '\n';
    }

    // Data read in, lets parse the data.
    try
    {
        Parser psr;
        auto parsed = psr.parse(data);
        auto results = parsed.evaluateInput();
        for (const auto r : results)
            std::cout << r << '\n';
    }
    catch (const std::exception & e)
    {
        std::cout << e.what();
        return -1;
    }
    return 0;
}
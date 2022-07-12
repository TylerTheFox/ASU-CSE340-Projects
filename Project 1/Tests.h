/*
    Author:     Brandan Tyler Lasley (blasley@asu.edu)
    ASU ID:     1214587374
    Class:      CSE340S20 MWF 12:55
    Date:       2/1/2020
    Purpose:    This is an automatic unit test code based off the examples in project 1 spec.
*/
#pragma once
#include "Parser.h"
#include "Build.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <streambuf>

std::string load_files(const std::string & filename)
{
    std::ifstream t(filename);
    if (t.is_open())
    {
        std::string data((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        t.close();
        return data;
    }
    return {};
}

void run_self_check_no_error()
{
    bool error = false;
    std::string expected_val;

    for (int i = 1; i <= 75; i++)
    {
        //std::cout << "Test " << i << "... ";
        std::vector<std::string> expected_values_strs;
        std::vector<long long> expected_values;

        std::stringstream ss;
        ss << "No_Error\\t" << i << ".txt";
        auto script = load_files(ss.str());
        if (!script.empty())
        {
            ss << ".expected";
            auto expected_val = load_files(ss.str());

            std::istringstream iss(expected_val);
            std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(expected_values_strs));

            for (const auto & str_num : expected_values_strs)
                expected_values.push_back(atoi(&str_num[0]));
            
            try
            {
                Parser psr;
                auto data = psr.parse(script);
                auto result = data.evaluateInput();

                if (result != expected_values)
                    throw std::runtime_error("Value mismatch!\n");

                //std::cout << "Passed!\n";
            }
            catch (const std::exception & e)
            {
                std::cout << "(No Errors) -- " << i << " --- " << e.what() << "\n";
                error = true;
            }
        }
    }
    if (!error)
    {
        std::cout << "Self Check (No Errors) Passed!\n";
    } else std::cout << "Self Check (No Errors) Failed! **********************************************\n";
}

void self_check_syntax_error()
{
    bool error = false;
    for (int i = 1; i <= 75; i++)
    {
        std::stringstream ss;
        ss << "Syntax_Error\\t" << i << ".txt";

        auto script = load_files(ss.str());
        if (!script.empty())
        {
            try
            {
                Parser psr;
                auto data = psr.parse(script);
                auto result = data.evaluateInput();
                std::cout << "(Syntax Error) -- " << i << " --- " << " Unexpected Pass\n";
                error = true;
            }
            catch (const std::exception & e)
            {
                if (std::string(e.what()) != "SYNTAX ERROR !!!")
                {
                    std::cout << "(Syntax Error) -- " << i << " --- " << " Unexpected Pass (With Semantic Errors)\n";
                    error = true;
                }
            }
        }
    }

    if (!error)
    {
        std::cout << "Self Check (Syntax Error) Passed!\n";
    }
    else std::cout << "Self Check (Syntax Error) Failed! **********************************************\n";
}

void self_check_error_codes(SemanticErrors code)
{
    bool error = false;
    for (int i = 1; i <= 10; i++)
    {
        std::stringstream ss;
        ss << "Error_Code_" << (int)code << "\\t" << i << ".txt";

        auto script = load_files(ss.str());
        if (!script.empty())
        {
            ss << ".expected";
            auto expected_val = load_files(ss.str());

            try
            {
                Parser psr;
                auto data = psr.parse(script);
                auto result = data.evaluateInput();
                std::cout << "(Error Code " << (int)code << ") -- " << i << " --- " << " Unexpected Pass\n";
                error = true;
            }
            catch (const std::exception & e)
            {
                std::string err(e.what());
                if (err == "SYNTAX ERROR !!!")
                {
                    std::cout << "(Error Code " << (int)code << ") -- " << i << " --- " << " Syntax Error\n";
                    error = true;
                } else if (err != expected_val)
                {
                    std::cout << "(Error Code " << (int)code << ") -- " << i << " --- " << " Wrong code or line number " << "\n(Error Code " << (int)code << ") -- " << "Expected: " << expected_val << "\n(Error Code " << (int)code << ") -- " << "Recieved: " << err << "\n";
                    error = true;
                }
            }
        }
    }

    if (!error)
    {
        std::cout << "Self Check (Error Code " << (int)code << ") Passed!\n";
    }
    else std::cout << "Self Check (Error Code " << (int)code << ") Failed! **********************************************\n";
}
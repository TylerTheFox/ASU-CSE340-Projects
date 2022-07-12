#pragma once
#include "Lexer.h"
#include <string>
#include <map>
#include <vector>
#include <set>

typedef std::vector<std::string> IDLIST;
struct IDD_Container
{
    IDLIST              idlist;
    std::string         name;
};

typedef std::map<int, IDD_Container> RULELIST;
typedef std::map<std::string, std::vector<IDLIST*>> RULEMAP;

struct ParserData
{
    IDLIST  order;
    RULELIST rulemap;
    RULEMAP defmap;
    int     count;
};

class Parser
{
public:
    Parser();
    ParserData parse(const std::string& str);
private:
    void processDef();
    IDLIST processDefList();

    std::string getString();

    ParserData          data;
    Lexer               lxr;
};
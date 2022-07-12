#pragma once
#include "Parser.h"
#include <map>

enum  SpeicalRules
{
    FOLLOWEOF = -2,
    EPSLION = -1
};

class Grammar;

class Rule
{
public:
    Rule(int sym, Grammar * g);
    bool isTerminal() const;
    bool isEpslion() const;
    bool isFollowEOF() const;
    bool isNonTerminal() const;

    int getSymbol() const;
    const std::string getSymbolName() const;

    void setSymbol(int s);
    void setGrammar(Grammar* _g);
private:
    int symbol;
    Grammar * g;
};

class Definition
{
public:
    Definition();
    Definition(int symbol, Grammar * grammer);
    Definition(const std::string& symbol, Grammar * grammer);
    int getSymbol() const;
    void resetPosition();
    Rule getNextRule();
    const std::string getSymbolName() const;
    size_t size() const;
    void addRule(const int sym);
    void addRule(const std::string & term);
    bool hasEplsion() const;
    std::set<int> getRules();
    bool doesRuleExist(const std::string & r);
    bool doesRuleExist(int r);
private:
    int curr;
    int symbol;
    bool hasEp;
    std::vector<Rule> Rules;
    Grammar * g;

    void setEplsion();
};

class Grammar
{
public:
    Grammar(ParserData & pd);

    int getSymbolNumber(const std::string & sym) const;
    const std::string& getSymbolName(int sym) const;
    bool isTerminal(int sym) const;

    std::vector<class Definition>& getDef();

    const std::map<int, bool>& getTerminalMap();
private: 
    ParserData * pd;

    // Grammar Data
    std::vector<class Definition> Defs;

    // Resolution Tables
    std::map<std::string, int>  SymbolTable;
    std::map<int, std::string>  SymbolResolutionTable;
    std::map<int, bool>         Terminals;

    void buildSymbolTable();
};
#include "Grammar.h"

Grammar::Grammar(ParserData & _pd)
{
    pd = &_pd;
    this->buildSymbolTable();

    // Setup Grammar
    for (int i = 0; i < _pd.count; i++)
    {
        const auto& def_name = _pd.rulemap[i].name;
        Definition d(SymbolTable[def_name], this);
        if (_pd.rulemap[i].idlist.size())
        {
            for (const auto& rule : _pd.rulemap[i].idlist)
            {
                d.addRule(SymbolTable[rule]);
            }
        }
        else d.addRule(EPSLION);
        Defs.push_back(d);
    }
}

int Grammar::getSymbolNumber(const std::string & sym) const
{
    return SymbolTable.at(sym);
}

const std::string & Grammar::getSymbolName(int sym) const
{
    return SymbolResolutionTable.at(sym);
}

bool Grammar::isTerminal(int sym)  const
{
    return Terminals.count(sym) && Terminals.at(sym);
}

std::vector<class Definition>& Grammar::getDef() 
{
    return Defs;
}

const std::map<int, bool>& Grammar::getTerminalMap()
{
    return Terminals;
}

void Grammar::buildSymbolTable()
{
    std::map<std::string, bool> alreadyPrinted;

    int global_int = 0;

    // Terminals
    for (int i = 0; i < pd->count; i++)
    {
        const auto & rulelist = pd->rulemap.at(i);

        for (const auto & rule : rulelist.idlist)
        {
            // If not a non terminal
            if (!pd->defmap.count(rule) && !alreadyPrinted[rule])
            {
                if (SymbolTable.count(rule) == 0)
                {
                    SymbolTable[rule] = global_int++;
                    SymbolResolutionTable[global_int - 1] = rule;
                    Terminals[global_int - 1] = true;
                }
                alreadyPrinted[rule] = true;
            }
        }
    }

    // Non Terminals
    for (int i = 0; i < pd->count; i++)
    {
        const auto & rulelist = pd->rulemap[i];

        if (!alreadyPrinted[rulelist.name])
        {
            if (SymbolTable.count(rulelist.name) == 0)
            {
                SymbolTable[rulelist.name] = global_int++;
                SymbolResolutionTable[global_int - 1] = rulelist.name;
                Terminals[global_int - 1] = false;
            }
            alreadyPrinted[rulelist.name] = true;
        }

        for (const auto & rule : rulelist.idlist)
        {
            // If not a non terminal
            if (pd->defmap.count(rule) && !alreadyPrinted[rule])
            {
                if (SymbolTable.count(rule) == 0)
                {
                    SymbolTable[rule] = global_int++;
                    SymbolResolutionTable[global_int - 1] = rule;
                    Terminals[global_int - 1] = false;
                }
                alreadyPrinted[rule] = true;
            }
        }
    }

    SymbolTable["$"] = SpeicalRules::FOLLOWEOF;
    SymbolTable["#"] = SpeicalRules::EPSLION;
    SymbolResolutionTable[SpeicalRules::FOLLOWEOF] = "$";
    SymbolResolutionTable[SpeicalRules::EPSLION] = "#";
}

Definition::Definition() : curr(0), symbol(0), g(0)
{
}

Definition::Definition(int sym, Grammar * grammer) : curr(0)
{
    symbol = sym;
    g = grammer;
}

Definition::Definition(const std::string & s, Grammar * grammer)
{
    symbol = grammer->getSymbolNumber(s);
    g = grammer;
}

int Definition::getSymbol()  const
{
    return symbol;
}

void Definition::resetPosition()
{
    curr = 0;
}

Rule Definition::getNextRule() 
{
    return Rules.at(curr++);
}

const std::string Definition::getSymbolName()  const
{
    return g->getSymbolName(symbol);
}

size_t Definition::size()  const
{
    return Rules.size();
}

void Definition::addRule(const int sym)
{
    if (sym == EPSLION)
        setEplsion();
    Rules.push_back(Rule(sym, g));
}

void Definition::addRule(const std::string & term)
{
    addRule(g->getSymbolNumber(term));
}

bool Definition::hasEplsion() const
{
    return hasEp;
}

std::set<int> Definition::getRules()
{
    std::set<int> rules;

    for (const auto & r : Rules)
        rules.insert(r.getSymbol());

    return rules;
}

bool Definition::doesRuleExist(const std::string & r)
{
    return doesRuleExist(g->getSymbolNumber(r));
}

bool Definition::doesRuleExist(int r)
{
    for (const auto& rul : Rules)
        if (rul.getSymbol() == r)
            return true;
    return false;
}

void Definition::setEplsion()
{
    hasEp = true;
}

Rule::Rule(int sym, Grammar * _g)
{
    symbol = sym;
    g = _g;
}

bool Rule::isTerminal()  const
{
    return g->isTerminal(symbol);
}

bool Rule::isEpslion()  const
{
    return symbol == EPSLION;
}

bool Rule::isFollowEOF()  const
{
    return symbol == FOLLOWEOF;
}

bool Rule::isNonTerminal()  const
{
    return !g->isTerminal(symbol);
}

int Rule::getSymbol()  const
{
    return symbol;
}

const std::string Rule::getSymbolName()  const
{
    return g->getSymbolName(symbol);
}

void Rule::setSymbol(int s)
{
    symbol = s;
}

void Rule::setGrammar(Grammar * _g)
{
    g = _g;
}

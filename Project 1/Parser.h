/*
    Author:     Brandan Tyler Lasley (blasley@asu.edu)
    ASU ID:     1214587374
    Class:      CSE340S20 MWF 12:55
    Date:       2/1/2020
    Purpose:    Parses a vector of tokens into executable code.
*/
#pragma once
#include "Lexer.h"
#include "PolyFunction.h"
#include <map>
#include <iterator>
#include <iostream>
#include <set>

enum class ParamType
{
    NUM,
    CHAR,
    FUNC
};

enum class SemanticErrors
{
    DuplicatePolyName = 1,
    InvalidMonomialName = 2,
    EvaluationOfUnDeclaredPoly = 3,
    IncorrectArguments = 4,
    UninitalizedArgument = 5
};

const std::string SemanticErrorStrs[]
{
    "Error Code 1:",
    "Error Code 2:",
    "Error Code 3:",
    "Error Code 4:",
    "Error Code 5:",
};

struct SemanticErrorData
{
    unsigned long long line;
    enum SemanticErrors Type;
};

inline bool operator<(const SemanticErrorData& c, const SemanticErrorData& d) 
{
    return c.line < d.line;
}

union paramU
{
    long long   num;
    char        c;
};

struct param
{
    paramU          Data;
    ParamType       Type;
};

class FunctionBase
{
public:
    FunctionBase()
    {

    }

    virtual ~FunctionBase()
    {

    }

    virtual void setFunctionName(const std::string & name)
    {
        // nothing.
    }

    virtual ParamType getType() = 0;
private:
};

struct FunctionParamNum : public FunctionBase
{
public:
    FunctionParamNum(long long _num)
    {
        num = _num;
    }

    ~FunctionParamNum()
    {

    }

    ParamType getType()
    {
        return ParamType::NUM;
    }

    long long getNum()
    {
        return num;
    }
private:
    long long num;
};

struct FunctionParamVar : public FunctionBase
{
public:
    FunctionParamVar(const std::string & _c)
    {
        c = _c;
    }

    ~FunctionParamVar()
    {

    }

    ParamType getType()
    {
        return ParamType::CHAR;
    }

    std::string getChar()
    {
        return c;
    }
private:
    std::string c;
};

struct FuncitonParamFunction : public FunctionBase
{
public:
    FuncitonParamFunction()
    {

    }

    ~FuncitonParamFunction()
    {

    }

    void setFunctionName(const std::string & name)
    {
        FuncName = name;
    }

    void setFunctionParams(const std::vector<std::shared_ptr<FunctionBase>> & p)
    {
        Params.insert(Params.end(), p.begin(), p.end());
    }

    ParamType getType()
    {
        return ParamType::FUNC;
    }

    const std::string & getFuncName() const
    {
        return FuncName;
    }

    std::vector<std::shared_ptr<FunctionBase>> & getFuncParams()
    {
        return Params;
    }

    void setLineNum(long long num)
    {
        line_num = num;
    } 

    long long getLineNum()
    {
        return line_num;
    }
private:
    std::vector<std::shared_ptr<FunctionBase>>  Params;
    std::string                                 FuncName;
    long long                                   line_num;
};

struct FunctionParm
{
    std::vector<std::shared_ptr<FunctionBase>>  fbvect;
    std::string                                 name;
};

struct InputFunction
{
    std::vector<std::string>                            InputMapOrder;
    std::map<std::string, long long>                    InputMap;
    std::vector<FunctionParm>                           FunctionCall;
};

struct ParserData
{
    std::map<std::string, PolyFunction>                    PolyMap;
    std::vector<InputFunction>                             FunctionVect;
    std::vector<long long>                                 InputParams;

    std::vector<long long> evaluateInput()
    {
        std::vector<long long> ret;
        std::vector<long long> inputs;

        // This preprocesses the function calls
        // to evaluate all X,Y's to values.

        // This doesn't really need to happen but it makes
        // the code less awful to work with.

        // Don't ask me how this works after about a week.
        for (auto & f : FunctionVect)
            for (auto & i : f.FunctionCall)
                for (auto & v : i.fbvect)
                    func_preprocessor(FunctionVect, f, v);

        // Evaluate
        for (auto & f : FunctionVect)
            for (auto & i : f.FunctionCall)
            {
                inputs.clear();
                for (auto & v : i.fbvect)
                {
                    switch (v->getType())
                    {
                    case ParamType::NUM:
                    {
                        auto fi = dynamic_cast<FunctionParamNum*>(v.get());
                        inputs.push_back(fi->getNum());
                    }
                        break;
                    case ParamType::FUNC: 
                        inputs.push_back(eval_func(f, v));
                        break;
                    default:
                        throw std::runtime_error("Internel Eval Error");
                    }
                }
                ret.push_back(PolyMap[i.name].evaluate(inputs));
            }

#if 0
        // DEBUG DEBUG DEBUG DEBUG
        size_t z = 0;
        for (auto & f : FunctionVect)
            for (auto & i : f.FunctionCall)
            {
                std::cout << i.name << "(";
                for (auto & v : i.fbvect)
                {
                    if (v->getType() == ParamType::FUNC)
                    {
                        auto fi = dynamic_cast<FuncitonParamFunction*>(v.get());
                        std::cout << fi->getFuncName() << "(";
                    }

                    debug_print_func(v);

                    if (v->getType() == ParamType::FUNC)
                        std::cout << "), ";
                }
                std::cout  << ")" << " = " << ret[z] <<"\n";
                z++;
            }
        // DEBUG DEBUG DEBUG DEBUG
#endif

        return ret;
    }

private:
    long long eval_func(const InputFunction & ifn, std::shared_ptr<FunctionBase> & f)
    {
        switch (f->getType())
        {
        case ParamType::NUM:
        {
            auto fi = dynamic_cast<FunctionParamNum*>(f.get());
            return fi->getNum();
            break;
        }
        case ParamType::FUNC:
        {
            std::vector<long long> inputs;
            auto fpp = dynamic_cast<FuncitonParamFunction*>(f.get());
            auto & vect = fpp->getFuncParams();

            for (auto & v : vect)
            {
                switch (v->getType())
                {
                case ParamType::NUM:
                {
                    auto fi = dynamic_cast<FunctionParamNum*>(v.get());
                    inputs.push_back(fi->getNum());
                    break;
                }
                case ParamType::FUNC:
                {
                    inputs.push_back(eval_func(ifn, v));
                    break;
                }
                default:
                    throw std::runtime_error("Internel Eval Error");
                }
            }
            return PolyMap[fpp->getFuncName()].evaluate(inputs);
            break;
        }
        default:
            throw std::runtime_error("Internel Eval Error");
        }
    }

    std::shared_ptr<FunctionBase> parse_normal_data(const std::vector<InputFunction> & ifv, const InputFunction & ifn, std::shared_ptr<FunctionBase> & f)
    {
        switch (f->getType())
        {
        case ParamType::CHAR:
        {            
            auto cfb = dynamic_cast<FunctionParamVar*>(f.get());

            if (ifn.InputMap.count(cfb->getChar()))
                return std::shared_ptr<FunctionBase>(new FunctionParamNum(ifn.InputMap.at(cfb->getChar())));
            else // No recent inputs, search for one in the backlog.
            {
                bool found_me = false;
                for (auto rit = ifv.rbegin(); rit != ifv.rend(); rit++)
                {
                    if (found_me && (*rit).InputMap.count(cfb->getChar()))
                        return std::shared_ptr<FunctionBase>(new FunctionParamNum((*rit).InputMap.at(cfb->getChar())));

                    // We want to start looking for the Input above mine,
                    // not below mine.
                    if (!found_me)
                        found_me = &(*rit) == &ifn;
                }
                if (!found_me)
                    throw std::runtime_error("Internel Eval Error");
            }
        }
        break;
        case ParamType::NUM:
        {
            auto cfb = dynamic_cast<FunctionParamNum*>(f.get());
            return std::shared_ptr<FunctionBase>(new FunctionParamNum(cfb->getNum()));
        }
        break;
        default:
            throw std::runtime_error("Internel Eval Error");
        }
        return {}; // Nullptr
    }

    void func_preprocessor(const std::vector<InputFunction> & ifv, const InputFunction & ifn, std::shared_ptr<FunctionBase> & f)
    {
        std::vector<std::shared_ptr<FunctionBase>> eval_tmp;

        switch (f->getType())
        {
        case ParamType::CHAR:
        case ParamType::NUM:
        {
            auto reti = parse_normal_data(ifv, ifn, f);
            f = reti;
            break;
        }
        case ParamType::FUNC:
        {
            auto fpp = dynamic_cast<FuncitonParamFunction*>(f.get());
            auto & vect = fpp->getFuncParams();

            for (auto & v : vect)
            {
                switch (v->getType())
                {
                case ParamType::NUM:
                    break;
                case ParamType::CHAR:
                {
                    auto reti = parse_normal_data(ifv, ifn, v);
                    v = reti;
                    break;
                }
                case ParamType::FUNC:
                {
                    func_preprocessor(ifv, ifn, v);
                    break;
                }        
                default:
                    throw std::runtime_error("Internel Eval Error");
                }
            }
            break;
        }
        default:
            throw std::runtime_error("Internel Eval Error");
        }
    }

    void debug_print_func(std::shared_ptr<FunctionBase> & f)
    {
        switch (f->getType())
        {
        case ParamType::NUM:
        {
            auto fi = dynamic_cast<FunctionParamNum*>(f.get());
            std::cout << fi->getNum() << ", ";
            break;
        }
        case ParamType::FUNC:
        {
            auto fpp = dynamic_cast<FuncitonParamFunction*>(f.get());
            auto & vect = fpp->getFuncParams();

            for (auto & v : vect)
            {
                switch (v->getType())
                {
                case ParamType::NUM:
                {
                    auto fi = dynamic_cast<FunctionParamNum*>(v.get());
                    std::cout << fi->getNum() << ", ";
                    break;
                }
                case ParamType::FUNC:
                {
                    auto fi = dynamic_cast<FuncitonParamFunction*>(v.get());
                    std::cout << fi->getFuncName() << "(";
                    debug_print_func(v);
                    std::cout << "), ";
                    break;
                }
                default:
                    throw std::runtime_error("Internel Eval Error");
                }
            }
            break;
        }
        default:
            throw std::runtime_error("Internel Eval Error");
        }
    }
};

class Parser
{
public:
    Parser();

    ParserData parse(const std::string& str);

    // Debug Debug Debug //
    void PrintOutLexer();
    Lexer & getLexer();
    // Debug Debug Debug //
private:
    unsigned int        currentFunc;
    struct ParserData   data;
    Lexer               lxr;

    std::multiset<struct SemanticErrorData> SemanticErrorVect;

    std::string getVarName(std::shared_ptr<TokenBase> & token);

    void parsePoly();
    void processArgs(PolyFunction & func);
    void parsePolyFunctionBody(PolyFunction & func);
    void parseInput();
    std::vector<std::shared_ptr<FunctionBase>> parseFunction(std::shared_ptr<TokenBase> & token, std::string & name, long long & ret_line);
    void parseFunctions();
    void parseInputParams();
    void checkFunctionParamSemantics(const struct FunctionParm & fp, long long line_num);
    void checkFunctionArgSemantics(const std::shared_ptr<FunctionBase> p);

    void processInputParams();
    void parse_error(const char* err_str, const std::shared_ptr<TokenBase> t);
    void semantic_error(enum SemanticErrors error, const std::shared_ptr<TokenBase> t);
    void semantic_error(enum SemanticErrors error, const unsigned long long line);
    std::string build_semantic_error_str(enum SemanticErrors error);
    bool should_I_report_this_error(enum SemanticErrors error, const unsigned long long line);
};
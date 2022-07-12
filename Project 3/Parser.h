#pragma once
#include "Lexer.h"
#include <map>
#include <vector>

struct InstructionNode;
extern int mem[1000];

struct InstructionData
{
    std::shared_ptr<TokenBase>  token;
    std::string                 str_data;
    int                         int_data;
};

struct InstructionIntermediateData
{
    InstructionData token_assignment_data;
    InstructionData token_lhs_data;
    InstructionData token_rhs_data;
};

class Parser
{
public:
    Parser();
    InstructionNode* parse(const std::string& str);
private:
    InstructionNode*            master;
    Lexer                       lxr;
    std::map<std::string, int>  VarAddressTable;
    int                         CurrentMemoryAddress;
    int                         InstructionCount;
    
    std::vector<std::string>                    parseVarHeader();
    std::vector<int>                            parseNumFooter();
    struct InstructionNode *                    parseInstructions();
    struct InstructionNode *                    parseInput();
    struct InstructionNode *                    parseOutput();
    struct InstructionNode *                    parseIF();
    struct InstructionNode *                    parseWHILE();
    struct InstructionNode *                    parseFOR();
    struct InstructionNode *                    parseSWITCH();
    std::vector<struct SwitchIntermediateData>  parseSwitchCases();
    struct InstructionNode *                    parseInstruction();

    std::string                 getString();
    int                         getNextMemoryAddress();
    int                         getMemoryAddress(const InstructionData & ID);
    InstructionData&            compute_IID(InstructionData & ID);
    void                        addInstructionToEndOfInstructionList(InstructionNode * I, InstructionNode *& list);
    void                        sortOutCJMPOperator(InstructionNode * I, TokenType TT);

    void                        AssignStaticMemory(const std::vector<std::string> & vars);
};
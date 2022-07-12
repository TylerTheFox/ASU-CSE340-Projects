#include "Parser.h"
#include "Lexer.h"
#include "compiler.h"
#include <sstream>
#include <cassert>
#include <cstring>

Parser::Parser() : master(nullptr), CurrentMemoryAddress(0), InstructionCount(0)
{
}

InstructionNode * Parser::parse(const std::string & str)
{
    lxr.process(str);

    memset(&mem[0], 0, 1000 * sizeof(int));

    auto vars = parseVarHeader();
    AssignStaticMemory(vars);
    auto token = lxr.getToken();

    master = parseInstructions();

    inputs = parseNumFooter();

    return master;
}

std::vector<std::string> Parser::parseVarHeader()
{
    std::shared_ptr<TokenBase> token;
    std::vector<std::string> vars;
    do
    {
        // Get list of vars.
        vars.push_back(getString());

        token = lxr.getToken();
        lxr.unGetToken();
    } while (token->getTokenType() != TokenType::SEMICOLON);

    return vars;
}

std::vector<int> Parser::parseNumFooter()
{
    auto token = lxr.getToken();
    std::vector<int> ints;
    do
    {
        auto num = dynamic_cast<TokenNum&>(*token);
        ints.push_back(num.num);
        token = lxr.getToken();
    } while (token->getTokenType() != TokenType::ENDOFFILE);

    return ints;
}

struct InstructionNode * Parser::parseInstructions()
{
    struct InstructionNode * in_begin = nullptr;
    struct InstructionNode * in_prev = nullptr;
    struct InstructionNode * in = nullptr;

    auto add_instruction = [&](struct InstructionNode * i)
    {
        if (in)
        {
            in_prev = in;
            in = i;
            in_prev->next = in;
        }
        else
        {
            in = i;
            in_begin = in;
        }

        if (i->next)
        {
            while (i->next)
            {
                if (i->type == CJMP)
                {
                    i = i->cjmp_inst.target;
                }
                else i = i->next;
            }
            in = i;
        }
    };

    auto debug_skip_statement = [&]()
    {
        std::shared_ptr<TokenBase> token;
        do
        {
            token = lxr.getToken();
        } while (token->getTokenType() != TokenType::SEMICOLON);
    };

    auto token = lxr.getToken();
    if (token->getTokenType() == TokenType::LBRACKET)
    {
        token = lxr.getToken();
        lxr.unGetToken();
        do
        {
            if (token->getTokenType() == TokenType::KEYWORD_IF)
            {
                add_instruction(parseIF());
            }
            else if (token->getTokenType() == TokenType::KEYWORD_WHILE)
            {
                add_instruction(parseWHILE());
            }
            else if (token->getTokenType() == TokenType::KEYWORKD_INPUT)
            {
                add_instruction(parseInput());
            }
            else if (token->getTokenType() == TokenType::KEYWORD_OUTPUT)
            {
                add_instruction(parseOutput());
            }
            else if (token->getTokenType() == TokenType::KEYWORD_SWITCH)
            {
                add_instruction(parseSWITCH());
            }
            else if (token->getTokenType() == TokenType::KEYWORD_FOR)
            {
                add_instruction(parseFOR());
            }
            else
            {
                add_instruction(parseInstruction());
            }

            token = lxr.getToken();
            lxr.unGetToken();
        } while (token->getTokenType() != TokenType::RBRACKET);
        token = lxr.getToken();
    }

    return in_begin;
}

InstructionNode * Parser::parseInput()
{
    struct InstructionNode * ISN = new InstructionNode{};

    lxr.getToken();
    auto var_name = getString();
    lxr.getToken(); // Ignore semicolon

    ISN->type = IN;
    ISN->input_inst.var_index = VarAddressTable[var_name];
    ISN->next = nullptr;

    return ISN;
}

InstructionNode * Parser::parseOutput()
{
    struct InstructionNode * ISN = new InstructionNode{};

    lxr.getToken();
    auto var_name = getString();
    lxr.getToken(); // Ignore semicolon

    ISN->type = OUT;
    ISN->input_inst.var_index = VarAddressTable[var_name];
    ISN->next = nullptr;

    return ISN;
}

InstructionData& Parser::compute_IID(InstructionData & ID)
{
    auto token = lxr.getToken();
    ID.token = token;
    if (token->getTokenType() == TokenType::NUM)
    {
        auto num = dynamic_cast<TokenNum&>(*token);
        ID.int_data = num.num;
    }
    else
    {
        lxr.unGetToken();
        auto var_name = getString();
        ID.str_data = var_name;
    }
    return ID;
}

void Parser::addInstructionToEndOfInstructionList(InstructionNode * I, InstructionNode *& list)
{
    auto if_body_end = list;
    while (if_body_end->next)
    {
        if (if_body_end->type == CJMP)
        {
            if_body_end = if_body_end->cjmp_inst.target;
        }
        else if_body_end = if_body_end->next;
    }
    if_body_end->next = I;
}

void Parser::sortOutCJMPOperator(InstructionNode * I, TokenType TT)
{
    switch (TT)
    {
    case TokenType::NOTEQUAL:
        I->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
        break;
    case TokenType::OPERATOR_GREATER_THAN:
        I->cjmp_inst.condition_op = CONDITION_GREATER;
        break;
    case TokenType::OPERATOR_LESS_THAN:
        I->cjmp_inst.condition_op = CONDITION_LESS;
        break;
    };
}

InstructionNode * Parser::parseIF()
{
    struct InstructionNode * IN_IF = new InstructionNode{};

    auto token = lxr.getToken();

    if (token->getTokenType() != TokenType::KEYWORD_IF)
        assert(false);

    InstructionIntermediateData IID;

    auto lhs = compute_IID(IID.token_lhs_data);
    auto op = lxr.getToken();
    auto rhs = compute_IID(IID.token_rhs_data);
    int rhs_mem = getMemoryAddress(lhs);
    int lhs_mem = getMemoryAddress(rhs);

    IN_IF->type = CJMP;
    sortOutCJMPOperator(IN_IF, op->getTokenType());

    IN_IF->cjmp_inst.operand1_index = rhs_mem;
    IN_IF->cjmp_inst.operand2_index = lhs_mem;
    auto if_body = parseInstructions();

    struct InstructionNode * IN_NOOP = new InstructionNode{};
    IN_NOOP->type = NOOP;
    IN_NOOP->next = nullptr;

    IN_IF->cjmp_inst.target = IN_NOOP;                      
    IN_IF->next = if_body;

    // Find end of IF_BODY
    addInstructionToEndOfInstructionList(IN_NOOP, if_body);

    return IN_IF;
}

InstructionNode * Parser::parseWHILE()
{
    struct InstructionNode * IN_WHILE = new InstructionNode{};

    auto token = lxr.getToken();

    if (token->getTokenType() != TokenType::KEYWORD_WHILE)
        assert(false);

    InstructionIntermediateData IID;

    auto lhs = compute_IID(IID.token_lhs_data);
    auto op = lxr.getToken();
    auto rhs = compute_IID(IID.token_rhs_data);
    int rhs_mem = getMemoryAddress(lhs);
    int lhs_mem = getMemoryAddress(rhs);

    IN_WHILE->type = CJMP;
    sortOutCJMPOperator(IN_WHILE, op->getTokenType());

    IN_WHILE->cjmp_inst.operand1_index = rhs_mem;
    IN_WHILE->cjmp_inst.operand2_index = lhs_mem;

    auto while_body = parseInstructions();

    struct InstructionNode * IN_NOOP = new InstructionNode{};
    IN_NOOP->type = NOOP;
    IN_NOOP->next = nullptr;
    IN_WHILE->cjmp_inst.target = IN_NOOP;
    IN_WHILE->next = while_body;

    // Find end of IF_WHILE
    addInstructionToEndOfInstructionList(IN_WHILE, while_body);

    return IN_WHILE;
}

InstructionNode * Parser::parseFOR()
{
    struct InstructionNode * IN_FOR = new InstructionNode{};

    auto token = lxr.getToken();

    if (token->getTokenType() != TokenType::KEYWORD_FOR)
        assert(false);

    InstructionIntermediateData IID;
    token = lxr.getToken();

    if (token->getTokenType() != TokenType::LPAREN)
        assert(false);

    auto assignment1 = parseInstruction();
    auto cmp_lhs = compute_IID(IID.token_lhs_data);
    auto cmp_op = lxr.getToken();
    auto cmp_rhs = compute_IID(IID.token_rhs_data);
    int cmp_rhs_mem = getMemoryAddress(cmp_lhs);
    int cmp_lhs_mem = getMemoryAddress(cmp_rhs);
    token = lxr.getToken(); // ignore ;
    auto assignment2 = parseInstruction();
    token = lxr.getToken();

    if (token->getTokenType() != TokenType::RPAREN)
        assert(false);

    struct InstructionNode * IN_NOOP = new InstructionNode{};
    IN_NOOP->type = NOOP;
    IN_NOOP->next = nullptr;

    auto for_instructions = parseInstructions();

    auto cjmp_i = IN_FOR;
    IN_FOR = assignment1;
    IN_FOR->next = cjmp_i;

    cjmp_i->type = CJMP;
    sortOutCJMPOperator(cjmp_i, cmp_op->getTokenType());
    cjmp_i->cjmp_inst.operand1_index = cmp_rhs_mem;
    cjmp_i->cjmp_inst.operand2_index = cmp_lhs_mem;

    addInstructionToEndOfInstructionList(assignment2, for_instructions);
    addInstructionToEndOfInstructionList(cjmp_i, for_instructions);

    cjmp_i->next = for_instructions;
    cjmp_i->cjmp_inst.target = IN_NOOP;

    return IN_FOR;
}

enum class SwitchCaseType
{
    CASE,
    DEFAULT
};

struct SwitchIntermediateData
{
    SwitchCaseType              Type;
    InstructionData             CaseNum;
    struct InstructionNode *    Body;
};

InstructionNode * Parser::parseSWITCH()
{
    InstructionIntermediateData IID;

    auto token = lxr.getToken();
    if (token->getTokenType() != TokenType::KEYWORD_SWITCH)
        assert(false);

    auto switch_var = compute_IID(IID.token_assignment_data);
    int switch_var_mem = getMemoryAddress(switch_var);

    token = lxr.getToken();
    if (token->getTokenType() != TokenType::LBRACKET)
        assert(false);

    auto cases = parseSwitchCases();

    token = lxr.getToken();
    if (token->getTokenType() != TokenType::RBRACKET)
        assert(false);

    // This is the escape noop
    struct InstructionNode * IN_END_NOOP = new InstructionNode{};
    IN_END_NOOP->type = NOOP;
    IN_END_NOOP->next = nullptr;
    
    bool default_found = false;
    struct InstructionNode * BEGIN_IN = nullptr;
    struct InstructionNode * PREV_IN = nullptr;
    for (auto & v : cases)
    {
        switch (v.Type)
        {
        case SwitchCaseType::CASE:
        {
            struct InstructionNode * IN_IF = new InstructionNode{};
            int switch_case_mem = getMemoryAddress(v.CaseNum);
            addInstructionToEndOfInstructionList(IN_END_NOOP, v.Body);
            IN_IF->type = CJMP;
            IN_IF->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
            IN_IF->cjmp_inst.target = v.Body;
            IN_IF->cjmp_inst.operand1_index = switch_var_mem;
            IN_IF->cjmp_inst.operand2_index = switch_case_mem;
            if (PREV_IN)
            {
                PREV_IN->next = IN_IF;
            }
            else BEGIN_IN = IN_IF;
            PREV_IN = IN_IF;
            break;
        }
        case SwitchCaseType::DEFAULT:
            PREV_IN->next = v.Body;
            addInstructionToEndOfInstructionList(IN_END_NOOP, v.Body);
            default_found = true;
            break;
        }  
    }

    if (!default_found)
    {
        addInstructionToEndOfInstructionList(IN_END_NOOP, PREV_IN);
    }

    return BEGIN_IN;
}

std::vector<struct SwitchIntermediateData> Parser::parseSwitchCases()
{
    InstructionIntermediateData IID;
    std::vector<struct SwitchIntermediateData> Cases;
    std::shared_ptr<TokenBase> token;

    do
    {
        token = lxr.getToken();
        switch (token->getTokenType())
        {
        case TokenType::KEYWORD_CASE:
        {
            auto switch_case_num = compute_IID(IID.token_assignment_data);
            lxr.getToken(); // ignore :
            auto case_body = parseInstructions();
            Cases.push_back({ SwitchCaseType::CASE, switch_case_num , case_body });
            break;
        }
        case TokenType::KEYWORD_DEFAULT:
        {
            lxr.getToken(); // ignore :
            auto default_body = parseInstructions(); // ignore body.
            Cases.push_back({ SwitchCaseType::DEFAULT, {}, default_body });
            break;
        }
        default:
            assert(false);
        }

        token = lxr.getToken();
        lxr.unGetToken();
    } while (token->getTokenType() == TokenType::KEYWORD_CASE || token->getTokenType() == TokenType::KEYWORD_DEFAULT);

    return Cases;
}

struct InstructionNode * Parser::parseInstruction()
{
    struct InstructionNode * IN = new InstructionNode{};
    InstructionIntermediateData IID;

    auto token_assignment = compute_IID(IID.token_assignment_data);
    auto token_equals = lxr.getToken();
    auto token_lhs = compute_IID(IID.token_lhs_data);
    auto token_operator = lxr.getToken();

    if (token_operator->getTokenType() == TokenType::OPERATOR_PLUS ||
        token_operator->getTokenType() == TokenType::OPERATOR_MINUS ||
        token_operator->getTokenType() == TokenType::OPERATOR_DIV ||
        token_operator->getTokenType() == TokenType::OPERATOR_MULTIPLY)
    {
        // Assignment with math a = b + c, a = b - c, a = b * c, a = b / c
        auto token_rhs = compute_IID(IID.token_rhs_data);

        // Compute memory
        int assignment_mem = getMemoryAddress(token_assignment);
        int rhs_mem = getMemoryAddress(token_lhs);
        int lhs_mem = getMemoryAddress(token_rhs);

        IN->type = ASSIGN;
        IN->assign_inst.left_hand_side_index = assignment_mem;
        IN->assign_inst.operand1_index = rhs_mem;
        IN->assign_inst.operand2_index = lhs_mem;
        switch (token_operator->getTokenType())
        {
        case TokenType::OPERATOR_PLUS:
            IN->assign_inst.op = OPERATOR_PLUS;
            break;
        case TokenType::OPERATOR_MINUS:
            IN->assign_inst.op = OPERATOR_MINUS;
            break;
        case TokenType::OPERATOR_DIV:
            IN->assign_inst.op = OPERATOR_DIV;
            break;
        case TokenType::OPERATOR_MULTIPLY:
            IN->assign_inst.op = OPERATOR_MULT;
            break;
        default:
            assert(false);
        }
        IN->next = nullptr;
        lxr.getToken(); // Ignore ;
    }
    else
    {
        // Assignment only, no math. a = c;
        int lhs_mem = getMemoryAddress(token_assignment);
        int rhs_mem = getMemoryAddress(token_lhs);

        IN->type = ASSIGN;
        IN->assign_inst.left_hand_side_index = lhs_mem;
        IN->assign_inst.op = OPERATOR_NONE;
        IN->assign_inst.operand1_index = rhs_mem;
        IN->next = nullptr;
    }
    InstructionCount++;
    return IN;
}

std::string Parser::getString()
{
    std::ostringstream ss;

    auto token = lxr.getToken();
    do
    {
        if (token->getTokenType() == TokenType::CHAR)
        {
            auto chr = dynamic_cast<TokenChar&>(*token);
            ss << chr.data;
            if (chr.newline)
            {
                token = lxr.getToken();
                lxr.unGetToken();
                return ss.str();
            }
        }
        else if (token->getTokenType() == TokenType::NUM)
        {
            auto num = dynamic_cast<TokenNum&>(*token);
            ss << num.num;
            if (num.newline)
            {
                token = lxr.getToken();
                lxr.unGetToken();
                return ss.str();
            }
        }
        token = lxr.getToken();
    } while (token->getTokenType() == TokenType::CHAR || token->getTokenType() == TokenType::NUM);

    lxr.unGetToken();

    return ss.str();
}

int Parser::getNextMemoryAddress()
{
    return CurrentMemoryAddress++;
}

int Parser::getMemoryAddress(const InstructionData & ID)
{
    int address = -1;

    if (ID.token->getTokenType() == TokenType::NUM)
    {
        address = getNextMemoryAddress();
        mem[address] = ID.int_data;
    }
    else
    {
        address = VarAddressTable[ID.str_data];
    }
    return address;
}

void Parser::AssignStaticMemory(const std::vector<std::string>& vars)
{
    for (const auto & var : vars)
        VarAddressTable[var] = getNextMemoryAddress();
}

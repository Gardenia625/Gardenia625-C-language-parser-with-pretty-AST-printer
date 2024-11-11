#include "error.h"
#include "parser.h"

struct TAC {

};

struct TAC_Program : public TAC {
    vector<unique_ptr<TAC_TopLevel>> program;
};

struct TAC_TopLevel : public TAC {

};

struct TAC_Function : public TAC_TopLevel {
    string identifier;
    bool global;
    vector<unique_ptr<TAC_Id>> params;
    vector<unique_ptr<TAC_Instruction>> body;
};

struct TAC_V : public TAC {
    string id;
    int val;
};

struct TAC_Instruction : public TAC {

};

struct TAC_Return : public TAC_Instruction {
    // value;
};



struct TAC_Unary : public TAC_Instruction {
    string op;
    TAC_V arg;
    TAC_V result;
};

struct TAC_Binary : public TAC_Instruction {
    string op;
    TAC_V arg1;
    TAC_V arg2;
    TAC_V dst;
};
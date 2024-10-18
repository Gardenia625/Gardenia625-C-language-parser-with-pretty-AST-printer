#include <memory>
#include <string>
#include <vector>

#include "lexer.h"

using std::unique_ptr;
using std::make_unique;


enum class CType {
    VOID,
    INT,
};

class AST {
public:
    virtual ~AST() = default;
    virtual void print(int tabs) = 0;
};

class Program: public AST {
public:
    void print(int tabs);
    Program& operator+=(unique_ptr<AST> other) {
        decls.push_back(std::move(other));
        return *this;
    }
private:
    vector<unique_ptr<AST>> decls;
};

class Literal: public AST {
public:
    Literal(): type(CType::INT), value(0) {}
    void print(int tabs) {}; // 待完成
private:
    CType type;
    TokenValue value;
};

class Variable: public AST {
public:
    Variable(CType t, string s): type(t), name(s) {}
    void print(int tabs);
protected:
    CType type;
    string name;
};



class Expression: public AST {
public:
    Expression(int val): val(val) {}
    void print(int tabs);
private:
    int val;
};


// statement
class Statement: public AST {


};

class ReturnStatement: public Statement {
public:
    ReturnStatement(unique_ptr<Expression> p) : exp(std::move(p)) {}
    void print(int tabs);
private:
    unique_ptr<Expression> exp;
};


class Block: public AST {
public:
    Block() = default;
    Block& operator+=(unique_ptr<Statement> other) {
        items.push_back(std::move(other));
        return *this;
    }
    void print(int tabs);
private:
    vector<unique_ptr<Statement>> items;
};

// function
class Parameter: public Variable {
public:
    Parameter(): Variable(CType::VOID, ""), default_value() {}
    Parameter(CType t, string s) : Variable(t, s), default_value() {}
    void print(int tabs);
private:
    bool default_flag = false;
    Literal default_value;
};

class Function: public AST {
public:
    Function(CType t, string s): ret_type(t), name(s) {}
    void add_parameter(unique_ptr<Parameter> p) { parameters.push_back(std::move(p)); }
    void set_body(unique_ptr<Block> p) { body = std::move(p); }
    void print(int tabs);
private:
    CType ret_type;
    string name;
    vector<unique_ptr<Parameter>> parameters;
    unique_ptr<Block> body;
};

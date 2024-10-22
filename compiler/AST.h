#include <memory>
#include <string>
#include <vector>

#include "lexer.h"

using std::unique_ptr;
using std::make_unique;

// specifier
enum class CS{
    NONE,
    // storage class
    STATIC,
    EXTERN,
    // type
    VOID,
    CHAR,    // 1 Byte
    INT,     // 2 Byte
    LONG,    // 4 Byte
    DOUBLE,  // 8 Byte
    // modifier
    UNSIGNED,
    // else
    STRUCT
};

// specifier combination
struct CType {
    CType() = default;
    CType(CS t): type(t) {}
    CType(CS m, CS t) : modifier(m), type(t) {}
    CType(CS t, string s) : type(t), name(s) {}
    void print(int tabs=0);
    CS storage = CS::NONE;
    CS modifier = CS::NONE;
    CS type = CS::VOID;
    string name = "";  // identifier of struct
};

// declarator
class Parameter;
class Expression;
struct CDecl {
    CDecl() = default;
    CDecl(string s) : name(s) {}
    void print(int tabs=0);
    string name;
    int depth = 0;  // pointer depth
    vector<Parameter> parameters;
    vector<unique_ptr<Expression>> indexes;
};

struct Parameter {
    Parameter() = default;
    Parameter(CType t, CDecl d) : type(t), decl(std::move(d)) {}
    void print(int tabs);
    CType type;
    CDecl decl;
};

// abstract syntax tree
class AST {
public:
    virtual ~AST() = default;
    virtual void print(int tabs) = 0;
};

class Program : public AST {
public:
    void print(int tabs);
    Program& operator+=(unique_ptr<AST> other) {
        decls.push_back(std::move(other));
        return *this;
    }
private:
    vector<unique_ptr<AST>> decls;
};


// expression
class Expression : public AST {
public:
    Expression(int v) : val(v) {}
    void print(int tabs);
private:
    int val;
};

// statement type
enum class ST {
    NONE,
    RETURN,
    IF,
    WHILE,
    DO,
    FOR,
    CONTINUE,
    BREAK,
    BLOCK,
    EXP
};

// statement
class Statement : public AST {
public:
    Statement() = default;
    Statement(ST st) : statement_type(st) {}
    ST type() { return statement_type; }
    void print(int tabs);
protected:
    ST statement_type = ST::NONE;
};

class ReturnStatement : public Statement {
public:
    ReturnStatement(unique_ptr<Expression> e) : exp(std::move(e)) {}
    void print(int tabs);
private:
    unique_ptr<Expression> exp;
};

class IfStatement : public Statement {
public:
    IfStatement(unique_ptr<Expression> c, unique_ptr<Statement> t) : cond(std::move(c)), then(std::move(t)) {}
    void add_else(unique_ptr<Statement> p) { _else = std::move(p); }
    void print(int tabs);
private:
    unique_ptr<Expression> cond;
    unique_ptr<Statement> then;
    unique_ptr<Statement> _else;
};

class WhileStatement : public Statement {
public:
    WhileStatement(unique_ptr<Expression> c, unique_ptr<Statement> s) : cond(std::move(c)), stmt(std::move(s)) {}
    void print(int tabs);
private:
    unique_ptr<Expression> cond;
    unique_ptr<Statement> stmt;
};

class DoStatement : public Statement {
public:
    DoStatement(unique_ptr<Statement> s, unique_ptr<Expression> c) : stmt(std::move(s)), cond(std::move(c)) {}
    void print(int tabs);
private:
    unique_ptr<Statement> stmt;
    unique_ptr<Expression> cond;
};

class ForStatement : public Statement {
public:
    ForStatement() = default;
    void add_init(unique_ptr<AST> f) { for_init = std::move(f); }
    void add_cond(unique_ptr<Expression> c) { cond = std::move(c); }
    void add_inc(unique_ptr<Expression> i) { inc = std::move(i); }
    void add_stmt(unique_ptr<Statement> s) { stmt = std::move(s); }
    void print(int tabs);
private:
    unique_ptr<AST> for_init;
    unique_ptr<Expression> cond;
    unique_ptr<Expression> inc;
    unique_ptr<Statement> stmt;
};

class Block : public Statement {
public:
    Block() = default;
    Block& operator+=(unique_ptr<AST> other) {
        items.push_back(std::move(other));
        return *this;
    }
    void print(int tabs);
private:
    vector<unique_ptr<AST>> items;
};

class ExpStatement : public Statement {
public:
    ExpStatement(unique_ptr<Expression> e) : exp(std::move(e)) {}
    void print(int tabs);
private:
    unique_ptr<Expression> exp;
};


// declaration
class Initializer : public AST {
public:
    Initializer() = default;
    Initializer(unique_ptr<Expression> p) { exp = std::move(p); }
    Initializer& operator+=(unique_ptr<Initializer> other) {
        init_list.push_back(std::move(other));
        return *this;
    }
    void print(int tabs);
private:
    unique_ptr<Expression> exp;
    vector<unique_ptr<Initializer>> init_list;
};

class Variable : public AST {
public:
    Variable(CType t, CDecl d) : type(t), decl(std::move(d)) {}
    void init(unique_ptr<Initializer> p) { initializer = std::move(p); }
    void print(int tabs);
private:
    CType type;
    CDecl decl;
    unique_ptr<Initializer> initializer;
};

class Function : public AST {
public:
    Function(CType t, CDecl d) : type(t) {
        decl = std::move(d);
    }
    void set_body(unique_ptr<Block> p) { body = std::move(p); }
    void print(int tabs);
private:
    CType type;
    CDecl decl;
    unique_ptr<Block> body;
};

class Struct : public AST {
public:
    Struct(string s) : name(s) {}
    void print(int tabs);
private:
    string name;
};
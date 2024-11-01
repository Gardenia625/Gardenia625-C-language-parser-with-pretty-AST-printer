#include <memory>
#include <unordered_map>
#include <unordered_set>

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
    CHAR,    // 1 byte
    INT,     // 2 byte
    LONG,    // 4 byte
    DOUBLE,  // 8 byte
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
    void print();
    CS storage = CS::NONE;
    CS modifier = CS::NONE;
    CS type = CS::VOID;
    string name = "";  // identifier of struct
};

// abstract syntax tree
class AST {
public:
    virtual ~AST() = default;
    virtual void print(bool ending) = 0;
protected:
    // The following members are all used for printing the AST.
    static vector<int> indent;
    static int cur;
    void print_indent(bool ending);
    void indent_push();
    void print_component(string name, bool ending);
};

class Program : public AST {
public:
    void print(bool ending);
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
    Expression() = default;
    Expression(string s) : name(s) {}
    Expression(unique_ptr<Expression> l, string s) : name(s), left(std::move(l)) {}
    void set_left(unique_ptr<Expression> l) { left = std::move(l); }
    void set_mid(unique_ptr<Expression> m) { mid = std::move(m); }
    void set_right(unique_ptr<Expression> r) { right = std::move(r); }
    void make_call(vector<unique_ptr<Expression>>&& c) { call = std::move(c); }
    void print(bool ending);
private:
    string name;
    unique_ptr<Expression> left;
    unique_ptr<Expression> mid;
    unique_ptr<Expression> right;
    vector<unique_ptr<Expression>> call;
};

class Constant : public Expression {
public:
    Constant(int v) : val(v) {}
    void print(bool ending);
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
    void print(bool ending);
protected:
    ST statement_type = ST::NONE;
};

class ReturnStatement : public Statement {
public:
    ReturnStatement(unique_ptr<Expression> e) : exp(std::move(e)) {}
    void print(bool ending);
private:
    unique_ptr<Expression> exp;
};

class IfStatement : public Statement {
public:
    IfStatement(unique_ptr<Expression> c, unique_ptr<Statement> t) : cond(std::move(c)), then(std::move(t)) {}
    void add_else(unique_ptr<Statement> p) { _else = std::move(p); }
    void print(bool ending);
private:
    unique_ptr<Expression> cond;
    unique_ptr<Statement> then;
    unique_ptr<Statement> _else;
};

class WhileStatement : public Statement {
public:
    WhileStatement(unique_ptr<Expression> c, unique_ptr<Statement> s) : cond(std::move(c)), body(std::move(s)) {}
    void print(bool ending);
private:
    unique_ptr<Expression> cond;
    unique_ptr<Statement> body;
};

class DoStatement : public Statement {
public:
    DoStatement(unique_ptr<Statement> s, unique_ptr<Expression> c) : body(std::move(s)), cond(std::move(c)) {}
    void print(bool ending);
private:
    unique_ptr<Statement> body;
    unique_ptr<Expression> cond;
};

class ForStatement : public Statement {
public:
    ForStatement() = default;
    void add_init(unique_ptr<AST> f) { for_init = std::move(f); }
    void add_cond(unique_ptr<Expression> c) { cond = std::move(c); }
    void add_inc(unique_ptr<Expression> i) { inc = std::move(i); }
    void add_body(unique_ptr<Statement> s) { body = std::move(s); }
    void print(bool ending);
private:
    unique_ptr<AST> for_init;
    unique_ptr<Expression> cond;
    unique_ptr<Expression> inc;
    unique_ptr<Statement> body;
};

class Block : public Statement {
public:
    Block() = default;
    Block& operator+=(unique_ptr<AST> other) {
        items.push_back(std::move(other));
        return *this;
    }
    void print(bool ending);
private:
    vector<unique_ptr<AST>> items;
};

class ExpStatement : public Statement {
public:
    ExpStatement(unique_ptr<Expression> e) : exp(std::move(e)) {}
    void print(bool ending);
private:
    unique_ptr<Expression> exp;
};


// declaration
class Parameter;
struct Declarator : AST {
    Declarator() = default;
    Declarator(string s) : name(s) {}
    void print(bool ending);
    string name;
    int depth = 0;  // pointer depth
    vector<Parameter> parameters;
    vector<unique_ptr<Expression>> indexes;
};

struct Parameter : AST {
    Parameter() = default;
    Parameter(CType t, Declarator d) : type(t), decl(std::move(d)) {}
    void print(bool ending);
    CType type;
    Declarator decl;
};

class Initializer : public AST {
public:
    Initializer() = default;
    Initializer(unique_ptr<Expression> p) { exp = std::move(p); }
    Initializer& operator+=(unique_ptr<Initializer> other) {
        init_list.push_back(std::move(other));
        return *this;
    }
    void print(bool ending);
private:
    unique_ptr<Expression> exp;
    vector<unique_ptr<Initializer>> init_list;
};

class Variable : public AST {
public:
    Variable(CType t, Declarator d) : type(t), decl(std::move(d)) {}
    void init(unique_ptr<Initializer> p) { initializer = std::move(p); }
    void print(bool ending);
private:
    CType type;
    Declarator decl;
    unique_ptr<Initializer> initializer;
};

class Function : public AST {
public:
    Function(CType t, Declarator d) : type(t) {
        decl = std::move(d);
    }
    void set_body(unique_ptr<Block> p) { body = std::move(p); }
    void print(bool ending);
private:
    CType type;
    Declarator decl;
    unique_ptr<Block> body;
};

class Struct : public AST {
public:
    Struct(string s) : name(s) {}
    void print(bool ending);
private:
    string name;
};
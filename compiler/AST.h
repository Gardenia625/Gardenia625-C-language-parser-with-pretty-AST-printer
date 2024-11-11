#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "lexer.h"

using std::unique_ptr;
using std::make_unique;

// Note:
// 1. Except for CType, Declarator, and Parameter, instances of all other types are stored as pointers.

// specifier
enum struct CS{
    NONE,
    // storage struct
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
struct AST {
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

struct Program : public AST {
    void print(bool ending);
    Program& operator+=(unique_ptr<AST> other) {
        decls.push_back(std::move(other));
        return *this;
    }
    vector<unique_ptr<AST>> decls;
};


// expression
struct Expression : public AST {
    Expression() = default;
    Expression(string s) : name(s) {}
    Expression(unique_ptr<Expression> l, string s) : name(s), left(std::move(l)) {}
    void print(bool ending);
    string name;
    unique_ptr<Expression> left;
    unique_ptr<Expression> mid;
    unique_ptr<Expression> right;
    vector<unique_ptr<Expression>> call;
};

struct Constant : public Expression {
    Constant(int v) : val(v) {}
    void print(bool ending);
    int val;
};

// statement
struct Statement : public AST {
    // empty statement
    void print(bool ending);
};

struct ContinueStatement : public Statement {
    void print(bool ending);
};

struct BreakStatement : public Statement {
    void print(bool ending);
};

struct ReturnStatement : public Statement {
    ReturnStatement(unique_ptr<Expression> e) : exp(std::move(e)) {}
    void print(bool ending);
    unique_ptr<Expression> exp;
};

struct IfStatement : public Statement {
    IfStatement(unique_ptr<Expression> c, unique_ptr<Statement> t) : cond(std::move(c)), then(std::move(t)) {}
    void print(bool ending);
    unique_ptr<Expression> cond;
    unique_ptr<Statement> then;
    unique_ptr<Statement> _else;
};

struct WhileStatement : public Statement {
    WhileStatement(unique_ptr<Expression> c, unique_ptr<Statement> s) : cond(std::move(c)), body(std::move(s)) {}
    void print(bool ending);
    unique_ptr<Expression> cond;
    unique_ptr<Statement> body;
};

struct DoStatement : public Statement {
    DoStatement(unique_ptr<Statement> s, unique_ptr<Expression> c) : body(std::move(s)), cond(std::move(c)) {}
    void print(bool ending);
    unique_ptr<Statement> body;
    unique_ptr<Expression> cond;
};

struct ForStatement : public Statement {
    ForStatement() = default;
    void print(bool ending);
    unique_ptr<AST> init;
    unique_ptr<Expression> cond;
    unique_ptr<Expression> inc;
    unique_ptr<Statement> body;
};

struct Block : public Statement {
    Block() = default;
    Block& operator+=(unique_ptr<AST> other) {
        items.push_back(std::move(other));
        return *this;
    }
    void print(bool ending);
    vector<unique_ptr<AST>> items;
};

struct ExpStatement : public Statement {
    ExpStatement(unique_ptr<Expression> e) : exp(std::move(e)) {}
    void print(bool ending);
    unique_ptr<Expression> exp;
};


// declaration
struct Parameter;
struct Declarator : public AST {
    Declarator() = default;
    Declarator(string s) : name(s) {}
    void print(bool ending);
    string name;
    int depth = 0;  // pointer depth
    vector<Parameter> parameters;
    vector<unique_ptr<Expression>> indexes;
};

struct Parameter : public AST {
    Parameter() = default;
    Parameter(CType t, Declarator d) : type(t), decl(std::move(d)) {}
    void print(bool ending);
    CType type;
    Declarator decl;
};

struct Initializer : public AST {
    Initializer() = default;
    Initializer(unique_ptr<Expression> p) { exp = std::move(p); }
    Initializer& operator+=(unique_ptr<Initializer> other) {
        init_list.push_back(std::move(other));
        return *this;
    }
    void print(bool ending);
    unique_ptr<Expression> exp;
    vector<unique_ptr<Initializer>> init_list;
};

struct Variable : public AST {
    Variable(CType t, Declarator d) : type(t), decl(std::move(d)) {}
    void init(unique_ptr<Initializer> p) { initializer = std::move(p); }
    void print(bool ending);
    CType type;
    Declarator decl;
    unique_ptr<Initializer> initializer;
};

struct Function : public AST {
    Function(CType t, Declarator d) : type(t), decl(std::move(d)) {}
    void print(bool ending);
    CType type;
    Declarator decl;
    unique_ptr<Block> body;
};

// struct Struct : public AST {
//     Struct(string s) : name(s) {}
//     void print(bool ending);
//     string name;
// };
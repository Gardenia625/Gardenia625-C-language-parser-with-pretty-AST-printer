
#include "error.h"
#include "parser.h"

// Ensure the current token is of the specified type and consume it
void Parser::match(TT t) {
    if (token.type == t) {
        consume();
    } else {
        switch (t) {
            case TT::L_PARENTHESIS:
                parser_error("expected '('", token.row);
            case TT::R_PARENTHESIS:
                parser_error("expected ')'", token.row);
            case TT::L_BRACKET:
                parser_error("expected '['", token.row);
            case TT::R_BRACKET:
                parser_error("expected ']'", token.row);
            case TT::L_BRACE:
                parser_error("expected '{'", token.row);
            case TT::R_BRACE:
                parser_error("expected '}'", token.row);
            case TT::COMMA:
                parser_error("expected ','", token.row);
            case TT::SEMICOLON:
                parser_error("expected ';'", token.row);
            case TT::COLON:
                parser_error("expected ':'", token.row);
        }
    }
}

// program ::= {<global-declaration>}
unique_ptr<Program> Parser::program() {
    unique_ptr<Program> ret = make_unique<Program>();
    while (token.type != TT::END) {
        *ret += declaration(true);
    }
    if (par_flag) {
        cout << COLOR_TITLE << "AST" << COLOR_RESET << endl;
        ret->print(true);
    }
    return ret;
}

// <global-declaration> ::= <variable-declaration> | <function-declaration>
// <variable-declaration> ::= <specifier> <declarator> [ "=" <initializer> ] ";"
// <function-declaration> ::= <specifier> <declarator> ( <block> | ";" )
unique_ptr<AST> Parser::declaration(bool global) {
    if (token.type == TT::STRUCT) {
        // return struct_declaration();
    }
    CType type = specifier(global);
    Declarator decl = declarator();
    if (decl.parameters.empty()) {
        // variable declaration
        unique_ptr<Variable> ret = make_unique<Variable>(type, std::move(decl));
        if (token.is_operator("=")) {
            consume();  // "="
            ret->init(initializer());
        }
        match(TT::SEMICOLON);
        return ret;
    } else if (global) {
        // function declaration
        unique_ptr<Function> ret = make_unique<Function>(type, std::move(decl));
        switch(consume().type) {
            case TT::L_BRACE:
                ret->body = block();
            case TT::SEMICOLON:
                break;
            default:
                parser_error("expected '{' or ';'", token.row);
        }
        return ret;
    } else {
        parser_error("invalid declaration", token.row);
    }
    return nullptr;
}

// <specifier> ::= <type-specifier> | "static" | "extern"
CType Parser::specifier(bool global) {
    CS storage_class = CS::NONE;
    if (token.type == TT::STATIC) {
        storage_class = CS::STATIC;
        consume();
    } else if (token.type == TT::EXTERN && global) {
        storage_class = CS::EXTERN;
        consume();
    }
    CType ret = type_specifier();
    ret.storage = storage_class;
    return ret;
}

// <type-specifier> ::= "void" | "double" |
//                    | ( [ "unsigned" ] "char" | "int" | "long" )
//                    | "struct" <identifier>
CType Parser::type_specifier() {
    int line = token.row;
    CS modifier = CS::NONE;
    if (token.type == TT::UNSIGNED) {
        modifier = CS::UNSIGNED;
        consume();
        line = token.row;
    }
    switch (consume().type) {
        case TT::VOID:
            return CType();
        case TT::DOUBLE:
            return CType(CS::DOUBLE);
        case TT::STRUCT:
            return CType(CS::STRUCT, identifier());
        case TT::CHAR:
            return CType(modifier, CS::CHAR);
        case TT::INT:
            return CType(modifier, CS::INT);
        case TT::LONG:
            return CType(modifier, CS::LONG);
        default:
            parser_error("invalid type specifier", line);
    }
    return CType();
}

// <declarator> ::= "*" <declarator> | <direct-declarator>
// <direct-declarator> ::= <simple-declarator> [ <declarator-suffix> ]
// <simple-declarator> ::= <identifier> | "(" <declarator> ")"
// <declarator-suffix> ::= <parameter-list> | { "[" <const> "]" }+
Declarator Parser::declarator() {
    Declarator ret;
    if (is_operator("*")) {
        consume();
        ret = declarator();
        ++ret.depth;
        return ret;
    }
    // simple declarator
    if (token.type == TT::L_PARENTHESIS) {
        ret = declarator();
        match(TT::R_PARENTHESIS);
    } else {
        ret = Declarator(identifier());
    }
    // declarator suffix
    if (token.type == TT::L_PARENTHESIS) {
        // parameter list
        consume();  // "("
        ret.parameters = parameter_list();
    } else {
        while (token.type == TT::L_BRACKET) {
            consume();  // "["
            ret.indexes.push_back(expression());
            match(TT::R_BRACKET);
        }
    }
    return ret;
}

// <parameter-list> ::= "(" "void" ")" | "(" <parameter> { "," <parameter> } ")"
vector<Parameter> Parser::parameter_list() {
    vector<Parameter> ret;
    if (token.type == TT::VOID) {
        ret.push_back(Parameter());
        consume();  // "void"
    } else {
        while (true) {
            ret.push_back(parameter());
            if (token.type == TT::COMMA) {
                consume();  // ","
            } else {
                break;
            }
        }
    }
    match(TT::R_PARENTHESIS);
    return ret;
}

// <parameter> ::= <type-specifier> <declarator>
Parameter Parser::parameter() {
    CType t = type_specifier();
    Declarator d = Declarator(declarator());
    return Parameter(t, std::move(d));
}

// <initializer> ::= <exp> | "{" [ <initializer-list> ] "}"
// <initializer-list> ::= <initializer> { "," <initializer> } [ "," ]
unique_ptr<Initializer> Parser::initializer() {
    if (token.type == TT::L_BRACE) {
        unique_ptr<Initializer> ret = make_unique<Initializer>();
        do {
            consume();  // "," or begining "{"
            if (token.type == TT::R_BRACE) {
                break;
            }
            *ret += initializer();
        } while (token.type == TT::COMMA);
        match(TT::R_BRACE);
        return ret;
    }
    return make_unique<Initializer>(expression(2));
}

// <statement> ::= ";"
//               | "return" <exp> ";"
//               | "if" "(" <exp> ")" <statement> [ "else" <statement> ]
//               | "while" "(" <exp> ")" <statement>
//               | "do" <statement> "while" "(" <exp> ")" ";"
//               | "for" "(" <for-init> [ <exp> ] ";" [ <exp> ] ")" <statement>
//               | "continue" ";"
//               | "break" ";"
//               | <block>
//               | <exp> ";"
unique_ptr<Statement> Parser::statement() {
    unique_ptr<Statement> ret;
    if (token.type == TT::SEMICOLON) {
        consume();  // ";"
        return make_unique<Statement>();
    } else if (token.type == TT::RETURN) {
        consume();  // "return"
        unique_ptr<Expression> ans = expression();
        unique_ptr<Statement> ret = make_unique<ReturnStatement>(std::move(ans));
        match(TT::SEMICOLON);
        return ret;
    } else if (token.type == TT::IF) {
        consume();  // "if"
        match(TT::L_PARENTHESIS);
        unique_ptr<Expression> cond = expression();
        match(TT::R_PARENTHESIS);
        unique_ptr<IfStatement> ret = make_unique<IfStatement>(std::move(cond), statement());
        if (token.type == TT::ELSE) {
            consume();  // "else"
            ret->_else = statement();
        }
        return ret;
    } else if (token.type == TT::WHILE) {
        consume();  // "while"
        match(TT::L_PARENTHESIS);
        unique_ptr<Expression> cond = expression();
        match(TT::R_PARENTHESIS);
        return make_unique<WhileStatement>(std::move(cond), statement());
    } else if (token.type == TT::DO) {
        consume();  // "do"
        unique_ptr<Statement> stmt = statement();
        match(TT::WHILE);
        match(TT::L_PARENTHESIS);
        unique_ptr<Expression> cond = expression();
        match(TT::R_PARENTHESIS);
        match(TT::SEMICOLON);
        return make_unique<DoStatement>(std::move(stmt), std::move(cond));
    } else if (token.type == TT::FOR) {
        consume();  // "for"
        match(TT::L_PARENTHESIS);
        unique_ptr<ForStatement> ret = make_unique<ForStatement>();
        if (is_specifier()) {
            ret->init = declaration(false);
        } else {
            ret->init = expression();
            match(TT::SEMICOLON);
        }
        if (token.type != TT::SEMICOLON) {
            ret->cond = expression();
        }
        match(TT::SEMICOLON);
        if (token.type != TT::R_PARENTHESIS) {
            ret->inc = expression();
        }
        match(TT::R_PARENTHESIS);
        ret->body = statement();
        return ret;
    } else if (token.type == TT::CONTINUE) {
        consume();  // "continue"
        match(TT::SEMICOLON);
        return make_unique<ContinueStatement>();
    } else if (token.type == TT::BREAK) {
        consume();  // "break"
        match(TT::SEMICOLON);
        return make_unique<BreakStatement>();
    } else if (token.type == TT::L_BRACE) {
        consume();  // "{"
        return block();
    } else {
        unique_ptr<ExpStatement> ret = make_unique<ExpStatement>(expression());
        match(TT::SEMICOLON);
        return ret;
    }
    return nullptr;
}

// <block> ::= "{" { <block-item> } "}"
// <block-item> ::= <statement> | <declaration>
unique_ptr<Block> Parser::block() {
    unique_ptr<Block> ret = make_unique<Block>();
    while (token.type != TT::R_BRACE) {
        int line = token.row;
        if (is_specifier()) {
            *ret += declaration(false);
        } else {
            *ret += statement();
        }
    }
    match(TT::R_BRACE);
    return ret;
}

// expression
std::unordered_set<string> unop = { 
    "++", "--", "+", "-", "!", "~", "*", "&", "sizeof"
};

std::unordered_set<string> binop = { 
    "+", "-", "*", "/", "%",
    "&", "^", "|", "&&", "||", "<<", ">>",
    "==", "!=", "<", "<=", ">", ">=", 
    "=", "+=", "-=", "*=", "/=", "*=", "<<=", ">>=", "&=", "^=", "|=",
    ",", "?"
};

bool Parser::is_unary() {
    return token.is_operator() && (unop.find(token.value) != unop.end());
}

bool Parser::is_binary() {
    return token.is_operator() && (binop.find(token.value) != binop.end());
}

std::unordered_map<string, std::pair<int, int>> map_prec = {
    // 13 multiplicative
    {"*", {13, 1}},
    {"/", {13, 1}},
    {"%", {13, 1}},
    // 12 additive
    {"+", {12, 1}},
    {"-", {12, 1}},
    // 11 shift
    {"<<", {11, 1}},
    {">>", {11, 1}},
    // 10 relational
    {"<", {10, 1}},
    {"<=", {10, 1}},
    {">", {10, 1}},
    {">=", {10, 1}},
    // 9 equality
    {"==", {9, 1}},
    {"!=", {9, 1}},
    // 8 bitwise AND
    {"&", {8, 1}},
    // 7 bitwise XOR
    {"^", {7, 1}},
    // 6 bitwise OR
    {"|", {6, 1}},
    // 5 logical AND
    {"&&", {5, 1}},
    // 4 logical OR
    {"||", {4, 1}},
    // 3 tenary
    {"?", {3, 0}},
    // 2 assignment
    {"=", {2, 0}},
    {"+=", {2, 0}},
    {"-=", {2, 0}},
    {"*=", {2, 0}},
    {"/=", {2, 0}},
    {"%=", {2, 0}},
    {"<<=", {2, 0}},
    {">>=", {2, 0}},
    {"&=", {2, 0}},
    {"^=", {2, 0}},
    {"|=", {2, 0}},
    // 1 comma
    {",", {1, 1}}
};


// <exp> ::= <factor> 
//         | <exp> <binary-operator> <exp>
//         | <exp> "?" <exp> ":" <exp>
unique_ptr<Expression> Parser::expression(int min_prec) {
    unique_ptr<Expression> left = factor();
    while (is_binary()) {
        auto [prec, assoc_left] = map_prec[token.value];
        if (prec < min_prec) {
            break;
        }
        unique_ptr<Expression> new_left = make_unique<Expression>(std::move(left), token.value);
        left = std::move(new_left);
        if (consume().value == "?") {
            left->mid = expression();
            match(TT::COLON);
            left->right = expression(prec + assoc_left);
        } else {
            left->right = expression(prec + assoc_left);
        }
    }
    return left;
}

// <factor> ::= <int>
//            | <unary-operator> <factor>
//            | "(" <exp> ")"
//            | <identifier>
//            | <identifier> "(" [ <argument-list> ] ")"
unique_ptr<Expression> Parser::factor() {
    if (token.type == TT::NUMBER) {
        return make_unique<Constant>(stoi(consume().value));
    } else if (is_unary()) {
        unique_ptr<Expression> ret = make_unique<Expression>(consume().value);
        ret->left = factor();
        return ret;
    } else if (token.type == TT::L_PARENTHESIS) {
        consume();  // "("
        unique_ptr<Expression> ret = expression();
        match(TT::R_PARENTHESIS);
        return ret;
    } else {
        unique_ptr<Expression> ret = make_unique<Expression>(identifier());
        if (token.type == TT::L_PARENTHESIS) {
            consume();  // "("
            ret->call = argument_list();
            match(TT::R_PARENTHESIS);
        }
        return ret;
    }
}

// <argument-list> ::= <exp> { "," <exp> }
vector<unique_ptr<Expression>> Parser::argument_list() {
    vector<unique_ptr<Expression>> ret;
    while (true) {
        consume();  // "," or begining "("
        ret.push_back(expression());
    } while (token.type == TT::COMMA)
    return ret;
}

string Parser::identifier() {
    if (token.type != TT::IDENTIFIER) {
        parser_error("expected an identifier", token.row);
    }
    return consume().value;
}
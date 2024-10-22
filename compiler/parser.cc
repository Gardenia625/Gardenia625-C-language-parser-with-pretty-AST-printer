
#include "error.h"
#include "parser.h"

void Parser::match(TT t) {
    if (token.type == t) {
        consume();
        return;
    }
    string message = "expected a";
    parser_error("match error", token.row);
    // switch (t) {
    //     case 
    // }
}

// program ::= {<global-declaration>}
unique_ptr<Program> Parser::program() {
    unique_ptr<Program> ret = make_unique<Program>();
    while (token.type != TT::END) {
        *ret += std::move(declaration(true));
    }
    if (par_flag) {
        cout << COLOR_TIP << "AST" << COLOR_RESET << endl;
        ret->print(0);
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
    CDecl decl = declarator();
    if (decl.parameters.empty()) {
        // variable declaration
        unique_ptr<Variable> ret = make_unique<Variable>(type, std::move(decl));
        if (token.is_operator("=")) {
            consume();  // "="
            ret->init(std::move(initializer()));
        }
        match(TT::SEMICOLON);
        return ret;
    } else if (global) {
        // function declaration
        unique_ptr<Function> ret = make_unique<Function>(type, std::move(decl));
        switch(consume().type) {
            case TT::L_BRACE:
                ret->set_body(std::move(block()));
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
CDecl Parser::declarator() {
    CDecl ret;
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
        ret = CDecl(identifier());
    }
    // declarator suffix
    if (token.type == TT::L_PARENTHESIS) {
        // parameter list
        consume();  // "("
        ret.parameters = parameter_list();
    } else {
        while (token.type == TT::L_BRACKET) {
            consume();  // "["
            ret.indexes.push_back(std::move(expression()));
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
            ret.push_back(std::move(parameter()));
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
    CDecl d = CDecl(declarator());
    return Parameter(t, std::move(d));
}

// <initializer> ::= <exp> | "{" [ <initializer-list> ] "}"
// <initializer-list> ::= <initializer> { "," <initializer> } [ "," ]
unique_ptr<Initializer> Parser::initializer() {
    if (token.type == TT::L_BRACE) {
        unique_ptr<Initializer> ret = make_unique<Initializer>();
        do {
            consume();  // "," or begining "{"
            if (token.type == TT::COMMA) {
                break;
            }
            *ret += std::move(initializer());
        } while (token.type == TT::COMMA);
        match(TT::R_BRACE);
        return ret;
    }
    return make_unique<Initializer>(expression());
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
        return make_unique<Statement>();
    } else if (token.type == TT::RETURN) {
        consume();  // "return"
        unique_ptr<Expression> cond = expression();
        unique_ptr<Statement> ret = make_unique<ReturnStatement>(std::move(cond));
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
            ret->add_else(statement());
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
        unique_ptr<ForStatement> ret;
        if (is_specifier()) {
            ret->add_init(declaration(false));
        } else {
            ret->add_init(make_unique<ExpStatement>(expression()));
            match(TT::SEMICOLON);
        }
        if (token.type != TT::SEMICOLON) {
            ret->add_cond(expression());
        }
        match(TT::SEMICOLON);
        if (token.type != TT::R_PARENTHESIS) {
            ret->add_inc(expression());
        }
        match(TT::R_PARENTHESIS);
        ret->add_stmt(statement());
        return ret;        
    } else if (token.type == TT::CONTINUE) {
        return make_unique<Statement>(ST::CONTINUE);
    } else if (token.type == TT::BREAK) {
        return make_unique<Statement>(ST::BREAK);
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
            *ret += std::move(declaration(false));
        } else {
            *ret += std::move(statement());
        }
    }
    match(TT::R_BRACE);
    return ret;
}

// expression ::= int
// int ::= ? A constant token ?
unique_ptr<Expression> Parser::expression() {
    unique_ptr<Expression> ret = make_unique<Expression>(stoi(consume().value));
    return ret;
}

string Parser::identifier() {
    if (token.type != TT::IDENTIFIER) {
        parser_error("expected an identifier", token.row);
    }
    return consume().value;
}
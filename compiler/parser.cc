
#include "error.h"
#include "parser.h"


// program ::= {global-declaration}
unique_ptr<Program> Parser::program() {
    unique_ptr<Program> ret = make_unique<Program>();
    while (token.type != TokenType::END) {
        *ret += global_declaration();
    }
    if (par_flag) {
        cout << COLOR_TIP << "AST" << COLOR_RESET << endl;
        ret->print(0);
    }
    return ret;
}

// global-declaration ::= variable-declaration | function-declaration
// variable-declaration ::= {specifier}+ identifier [ "=" expression ] ";"
// function-declaration ::= {specifier}+ identifier "(" parameter-list ")" ( block | ";" )
// parameter-list ::= "void" | "int" identifier { "," "int" identifier }
unique_ptr<AST> Parser::global_declaration() {
    CType type = specifier();
    string name = identifier();
    switch (next().type) {
        case TokenType::L_PARENTHESIS:
            unique_ptr<Function> ret = make_unique<Function>(type, name);

            next();
            ret->add_parameter(make_unique<Parameter>());
            next(); // consume ")"
            // parameter_list();
            switch(next().type) {
                case TokenType::L_BRACE:
                    ret->set_body(block());
                case TokenType::SEMICOLON:
                    break;
                // default:
                    // error
            }
            return ret;
    }
}

// block ::= "{" block-item "}"
// block-item ::= statement
unique_ptr<Block> Parser::block() {
    unique_ptr<Block> ret = make_unique<Block>();
    while (token.type != TokenType::R_BRACE) {
        *ret += statement();
    }
    next();
    return ret;
}

// statement ::= "return" expression ";"
unique_ptr<Statement> Parser::statement() {
    switch(next().type) {
        case TokenType::RETURN:
            unique_ptr<Expression> exp = make_unique<Expression>(std::get<int>(next().value));
            unique_ptr<Statement> ret = make_unique<ReturnStatement>(std::move(exp));
            next(); // ";"
            return ret;
    }
}

// expression ::= int
// int ::= ? A constant token ?
// Parser::expression() {

// }

// specifier ::= "int"
CType Parser::specifier() {
    if (token.type == TokenType::INT) {
        next();
        return CType::INT;
    }
}


// identifier ::= ? An identifier token ?
string Parser::identifier() {
    if (token.type == TokenType::IDENTIFIER) {
        return std::get<string>(next().value);
    } else {
        // error
    }
}
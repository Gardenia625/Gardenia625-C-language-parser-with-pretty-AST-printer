
#include "error.h"
#include "parser.h"

void Parser::match(TokenType t) {
    if (token.type == t) {
        consume();
        return;
    }
    string message = "expected a";
    // switch (t) {
    //     case 
    // }
}

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
    if (token.type == TokenType::STRUCT) {
        // return struct_declaration();
    }
    CType type = specifier();
    string name = identifier();
    if (token.type == TokenType::L_PARENTHESIS) {
        consume(); // "("
        unique_ptr<Function> ret = make_unique<Function>(type, name);

        consume(); // void
        ret->add_parameter(std::move(make_unique<Parameter>()));
        // while (token.type != TokenType::R_PARENTHESIS) {
        //     ret->add_parameter(parameter());
        // }
        consume(); // ")"
        switch(consume().type) {
            case TokenType::L_BRACE:
                ret->set_body(block());
            case TokenType::SEMICOLON:
                break;
            default:
                parser_error("expected '{' or ';'", token.row);
        }
        return ret;
    } else {
        unique_ptr<Variable> ret = make_unique<Variable>(type, name);
        if (token.is_operator("=")) {
            consume();                   // "="
            ret->init(token.value);
        }
        match(TokenType::SEMICOLON);     // ";"
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
    consume();
    return ret;
}

// statement ::= "return" expression ";"
unique_ptr<Statement> Parser::statement() {
    if (token.type == TokenType::RETURN) {
        consume();
        unique_ptr<Expression> exp = make_unique<Expression>(std::get<int>(consume().value));
        unique_ptr<Statement> ret = make_unique<ReturnStatement>(std::move(exp));
        consume();
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
        consume();
        return CType::INT;
    }
}


// identifier ::= ? An identifier token ?
string Parser::identifier() {
    if (token.type == TokenType::IDENTIFIER) {
        return std::get<string>(consume().value);
    } else {
        // error
    }
}
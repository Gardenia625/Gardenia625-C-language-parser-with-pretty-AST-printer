#ifndef HEADER_LEXER
#define HEADER_LEXER

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <unordered_map>
#include <unordered_set>

#include <format>

#include "error.h"

// token type
enum class TT {
    END,
    COMMENT,
    HASH,
    NUMBER,
    // type
    STRUCT,
    STATIC,
    EXTERN,
    VOID,
    CHAR,
    INT,
    LONG,
    DOUBLE,
    UNSIGNED,

    STRING,
    IDENTIFIER,
    TYPE,
    // keywords
    RETURN,
    IF,
    ELSE,
    FOR,
    WHILE,
    DO,
    CONTINUE,
    BREAK,
    SWITCH,
    CASE,
    DEFAULT,
    // symbols
    OPERATOR,
    L_PARENTHESIS,
    R_PARENTHESIS,
    L_BRACKET,
    R_BRACKET,
    L_BRACE,
    R_BRACE,
    COMMA,
    SEMICOLON
};

struct Token {
    TT type;
    string value;
    int row;
    int col;
    void print();
    bool is_operator(string s) {
        return (type == TT::OPERATOR) && (s == value);
    }
    bool is_specifier() {
        return type == TT::STATIC
            || type == TT::EXTERN
            || type == TT::VOID
            || type == TT::CHAR
            || type == TT::INT
            || type == TT::LONG
            || type == TT::UNSIGNED
            || type == TT::DOUBLE
            || type == TT::STRUCT;
    }
};

class Lexer {
public:
    Lexer(string f, bool l);
    Token next();
private:
    std::ifstream file;
    char c;               // currend character, i.e., the next character to be used
    int row = 0;          // current row
    int col = -1;         // current column
    bool lex_flag;        // whether to print tokens
    void move_forward();
    Token next_token();
    Token next_identifier();
    Token next_number();
    Token next_char();
    Token next_string();
    Token next_comment();
    Token next_symbol();
};

inline Lexer::Lexer(string f, bool l) : file(f), lex_flag(l) {
    if (!file) {
        cerr << COLOR_ERROR << "error: " << COLOR_RESET
             << "failed to open the file" << endl;
        exit(1);
    }
    if (lex_flag) {
        cout << COLOR_TITLE << "[ row: col] token" << COLOR_RESET << endl;
    }
    move_forward();
}

#endif
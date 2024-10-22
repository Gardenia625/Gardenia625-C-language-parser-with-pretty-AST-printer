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
    SEMICOLON,
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
    Lexer(string filename, bool flag);
    Token next();
private:
    std::ifstream file;
    char c;               // 下一个待读取的字符
    int row;              // c 所在的行
    int col;              // c 所在的列
    bool lex_flag;        // 是否打印
    void move_forward();  // 读取下一个字符
    Token next_token();
    Token next_identifier();
    Token next_number();
    Token next_char();
    Token next_string();
    Token next_comment();
    Token next_symbol();
};


inline Lexer::Lexer(string filename, bool flag)
    : row(0), col(-1), file(filename), lex_flag(flag) {
    if (!file) {
        cerr << COLOR_ERROR << "error: " << COLOR_RESET
             << "Failed to open the file." << endl;
        exit(1);
    }
    if (lex_flag) {
        cout << COLOR_TIP << "[ row: col] token" << COLOR_RESET << endl;
    }
    move_forward();
}

#endif
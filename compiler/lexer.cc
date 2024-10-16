#include "lexer.h"
#include "error.h"

void Token::print() {
    std::cout << '[' << std::format("{:>4}", std::to_string(row)) << ':'
              << std::format("{:>4}", std::to_string(col)) << "] ";
    switch (type) {
    case TokenType::ERROR:
        std::cout << COLOR_ERROR << "error token" << COLOR_RESET;
        break;
    case TokenType::NUMBER:
        std::cout << std::get<int>(value);
        break;
    case TokenType::CHAR:
        std::cout << '\'' << std::get<char>(value) << '\'';
        break;
    case TokenType::STRING:
        std::cout << '"' << std::get<char>(value) << '"' ;
        break;
    default:
        std::cout << std::get<std::string>(value);
    }
    std::cout << std::endl;
}

std::unordered_map<std::string, TokenType> get_token_type {
    {"return", TokenType::KEYWORD}
};

std::unordered_map<char, char> get_escape_char {
    {'\\', '\\'},
    {'\'', '\''},
    {'"', '\"'},
    {'n', '\n'},
    {'r', '\r'},
    {'t', '\t'},
    {'b', '\b'},
    {'f', '\f'},
    {'a', '\a'},
    {'v', '\v'},
    {'0', '\0'}
};

// get the next character from the file
void Lexer::move_forward() {
    if (c == '\n') {
        ++row;
        col = 0;
    }
    else {
        ++col;
    }
    file.get(c);
}

// backtrack a character
void Lexer::move_backward() {
    file.seekg(-1, std::ios::cur);
    --col;
}

// get next token and print it if needed
Token Lexer::next() {
    Token token = next_token();
    if (flag) {
        token.print();
    }
    return token;
}

Token Lexer::next_token() {
    while (true) {
        move_forward();
        if (file.eof()) {              // EOF
            file.close();
            return Token(TokenType::END, 0, row, col);  
        }
        if (isspace(c)) {              // ignore whitespaces
            continue;
        }
        if (c == '/') {                // maybe a comment
            Token t = next_comment();
            if (t.type == TokenType::OPERATOR) {
                return t;
            }
            continue;
        }
        if (c == '#') {                // macro
            return next_macro();
        }
        if (isdigit(c)) {              // number
            return next_number();
        }
        if (isalpha(c) || c == '_') {  // identifier of keyword
            return next_identifier();
        }
        if (c == '\'') {               // char
            return next_char();
        }
        if (c == '"') {               // string
            return next_string();
        }
        return next_symbol();
    }
}

Token Lexer::next_comment() {
    move_forward();
    if (c == '/') {         // single-line comment
        while (c != '\n') {
            move_forward();
        }
        return Token(TokenType::COMMENT, 0, row, col);
    } else if (c == '*') {  // multi-line comment
        bool maybe_end = false;
        while (true) {
            move_forward();
            if (maybe_end && c == '/') {
                return Token(TokenType::COMMENT, 0, row, col);
            }
            maybe_end = (c == '*');
        }
    } else {                // not a comment
        if (!isspace(c)) {
            move_backward();
        }
        return Token(TokenType::OPERATOR, "/", row, col);
    }
}

Token Lexer::next_macro() {
    std::string name;
    move_forward();
    while (isalpha(c)) {
        name += c;
        move_forward();
    }
    if (!isspace(c)) {
        move_backward();
    }
    return Token(TokenType::MACRO, name, row, col);
}

Token Lexer::next_number() {
    int v = 0;
    do {
        v *= 10;
        v += c - '0';
        move_forward();
    } while (isdigit(c));
    if (!isspace(c)) {
        move_backward();
    }
    return Token(TokenType::NUMBER, v, row, col);
}

Token Lexer::next_identifier() {
    std::string name;
    do {
        name += c;
        move_forward();
    } while (isalnum(c) || c == '_');
    if (!isspace(c)) {
        move_backward();
    }
    auto it = get_token_type.find(name);
    if (it != get_token_type.end()) {  // keyword
        return Token(it->second, name, row, col);
    }
    else {
        return Token(TokenType::IDENTIFIER, name, row, col);
    }
}

Token Lexer::next_char() {
    move_forward();
    char v = c;
    move_forward();
    if (c != '\'') {
        make_error("expected a quotation", row, col);
        return Token(TokenType::ERROR, 0, row, col);
    }
    return Token(TokenType::CHAR, v, row, col);
}

Token Lexer::next_string() {
    std::string s;
    move_forward();
    while (c != '"') {
        if (c == '\\') {  // escape character
            move_forward();
            auto it = get_escape_char.find(c);
            if (it != get_escape_char.end()) {
                s += it->second;
            } else {
                make_error("unknown escape character", row, col);
                return Token(TokenType::ERROR, 0, row, col);
            }
        } else {
            s += c;
        } 
        move_forward();
    }
    return Token(TokenType::STRING, s, row, col);
}

Token Lexer::next_symbol() {
    return Token(TokenType::SEPARATOR, std::string{c}, row, col);
}
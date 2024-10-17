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
    // types
    // {"unsigned", TokenType::TYPE},
    // {"signed", TokenType::TYPE},
    // {"char", TokenType::TYPE},
    // {"short", TokenType::TYPE},
    {"int", TokenType::TYPE},
    // {"float", TokenType::TYPE},
    // {"double", TokenType::TYPE},
    // {"long", TokenType::TYPE},
    {"void", TokenType::TYPE},
    // {"struct", TokenType::TYPE},
    // {"union", TokenType::TYPE},
    // {"static", TokenType::TYPE},
    // keyword
    {"return", TokenType::KEYWORD},
    // {"include", TokenType::KEYWORD},
    // {"sizeof", TokenType::KEYWORD},
    // {"if", TokenType::KEYWORD},
    // {"else", TokenType::KEYWORD},
    // {"for", TokenType::KEYWORD},
    // {"while", TokenType::KEYWORD},
    // {"do", TokenType::KEYWORD},
    // {"continue", TokenType::KEYWORD},
    // {"break", TokenType::KEYWORD},
    // {"switch", TokenType::KEYWORD},
    // {"case", TokenType::KEYWORD},
    // {"default", TokenType::KEYWORD},
    // {"goto", TokenType::KEYWORD},
    // {"typedef", TokenType::KEYWORD},

    // {"__ignore_typecheck__", TokenType::KEYWORD},
    // {"const", TokenType::KEYWORD},
    // {"extern", TokenType::KEYWORD},
    // {"restrict", TokenType::KEYWORD},
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
    } else {
        ++col;
    }
    file.get(c);
}

// get next token and print it if needed
Token Lexer::next() {
    Token token = next_token();
    if (flag && token.type != TokenType::END) {
        token.print();
    }
    return token;
}

Token Lexer::next_token() {
    while (!file.eof()) {
        if (isspace(c)) {              // ignore whitespaces
            move_forward();
            continue;
        }
        if (c == '/') {                // maybe a comment
            Token t = next_comment();
            if (t.type != TokenType::COMMENT) {
                return t;
            }
            continue;
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
    file.close();
    return Token(TokenType::END, 0, row, col);
}

Token Lexer::next_comment() {
    Token ret(TokenType::COMMENT, "/", row, col);
    bool maybe_end = false;
    move_forward();
    switch(c) {
        case '/':                  // single-line comment
            while (c != '\n' && !file.eof()) {
                move_forward();
            }
            return ret;
        case '*':                  // multi-line comment
            while (true) {         // can only ends with "*/"
                move_forward();
                if (file.eof()) {
                    make_error("unterminated comment", ret.row, ret.col);
                    ret.type = TokenType::ERROR;
                    return ret;
                } else if (maybe_end || c == '/'){
                    move_forward();
                    return ret;
                }
                maybe_end = (c == '*');
            }
        case '=':                  // binary operator "/="
            ret.type = TokenType::BINARY_OPERATOR;
            ret.value = "/=";
            move_forward();
            return ret;
        default:                   // unary operator "/"
            ret.type = TokenType::UNARY_OPERATOR;
            return ret;
    }
}

std::unordered_set<char> end_of_number {
    '+', '-', '*', '/', '%', '&', '|', '^', '<', '=', '>', '!',
    ')', ']', '}', ',', ';'
};

Token Lexer::next_number() {
    Token ret(TokenType::NUMBER, 0, row, col);
    bool is_float = false;
    int v = 0;
    // if (c == '.' || c == 'e' || c == 'E' || c == 'p' || c == 'P') {
    //     is_float == true;
    // }
    // if (c == '0') {
    //     move_forward();
    //     if (c == 'x' || c == 'X') {  // base 16

    //     } else {                     // base 8
            
    //     }
    // } else {                         // base 10

    // }

    while (isdigit(c)) {
        v *= 10;
        v += c - '0';
        move_forward();
    }
    auto it = end_of_number.find(c);
    if (it == end_of_number.end()) {
        make_error("invalid number", ret.row, ret.col);
        ret.type = TokenType::ERROR;
        do {
            move_forward();
            it = end_of_number.find(c);
        } while (it == end_of_number.end() && !file.eof());
    }
    ret.value = v;
    return ret;
}

Token Lexer::next_identifier() {
    Token ret(TokenType::IDENTIFIER, "", row, col);
    std::string name;
    while((isalnum(c) || c == '_') && !file.eof()) {
        name += c;
        move_forward();
    }
    ret.value = name;
    auto it = get_token_type.find(name);
    if (it != get_token_type.end()) {  // keyword
        ret.type = it->second;
    }
    return ret;
}

Token Lexer::next_char() {
    Token ret(TokenType::CHAR, '\0', row, col);
    move_forward();
    ret.value = c;
    if (c == '\\') {  // escape character
        move_forward();
        auto it = get_escape_char.find(c);
        if (it != get_escape_char.end()) {
            ret.value = it->second;
        } else {
            make_error("unknown escape character", row, col);
            ret.type = TokenType::ERROR;
        }
    }
    move_forward();
    if (c != '\'') {
        make_error("expected a quotation", row, col);
        ret.type = TokenType::ERROR;
    }
    move_forward();
    return ret;
}

Token Lexer::next_string() {
    Token ret(TokenType::STRING, "", row, col);
    move_forward();
    std::string s;
    while (c != '"') {
        if (c == '\\') {  // escape character
            move_forward();
            if (file.eof()) {
                make_error("unterminated string", ret.row, ret.col);
                ret.type = TokenType::ERROR;
                break;
            }
            auto it = get_escape_char.find(c);
            if (it != get_escape_char.end()) {
                s += it->second;
            } else {
                make_error("unknown escape character", row, col);
                ret.type = TokenType::ERROR;
            }
        } else {
            s += c;
        } 
        move_forward();
        if (file.eof()) {
            make_error("unterminated string", row, col);
            ret.type = TokenType::ERROR;
            break;
        }
    }
    ret.value = s;
    move_forward();
    return ret;
}

Token Lexer::next_symbol() {
    Token ret(TokenType::SYMBOL, std::string{c}, row, col);
    char first = c;
    move_forward();
    char second = c;
    switch (first) {
        case '+':
        case '-':
        case '*':
        case '%':
        case '&':
        case '|':
        case '^':
        case '<':
        case '>':
        case '=':
            if (first == second || second == '=') {
                ret.type = TokenType::BINARY_OPERATOR;
                ret.value = std::string{first, second};
            } else {
                ret.type = TokenType::UNARY_OPERATOR;
            }
            break;
        case '!':
            if (second == '=') {
                ret.type = TokenType::BINARY_OPERATOR;
                ret.value = std::string{first, second};
            } else {
                ret.type = TokenType::UNARY_OPERATOR;
            }
            break;
        case '~':
        case '(':
        case '[':
        case ',':
        case '.':
        case '?':
            ret.type = TokenType::UNARY_OPERATOR;
            break;
        case '{':
        case '}':
        case ')':
        case ']':
        case ':':
        case ';':
        case '#':
        case '\\':
            while (isspace(c) && !file.eof()) {
                move_forward();
            }
            if (file.eof()) {
                make_error("stray '\\'", ret.row, ret.col);
                ret.type = TokenType::ERROR;
            }
            break;
        default:
            make_error("unknown symbol", ret.row, ret.col);
            ret.type = TokenType::ERROR;
    }
    if (ret.type != TokenType::UNARY_OPERATOR) {
        move_forward();
    }
    return ret;
}
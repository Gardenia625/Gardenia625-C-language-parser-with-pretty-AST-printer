#include "lexer.h"

void Token::print() {
    cout << COLOR_CLASS
         << "[" << std::format("{:>4}", std::to_string(row)) << ":"
         << std::format("{:>4}", std::to_string(col)) << "] "
         << COLOR_RESET;
    switch (type) {
        case TT::CHAR:
            cout << "'" << value << "'";
            break;
        case TT::STRING:
            cout << "\"" << value << "\"";
            break;
        default:
            cout << value;
    }
    cout << endl;
}

std::unordered_map<string, TT> get_token_type {
    // type
    {"static", TT::STATIC},
    {"extern", TT::EXTERN},
    {"void", TT::VOID},
    {"char", TT::CHAR},
    {"int", TT::INT},
    {"long", TT::LONG},
    {"double", TT::DOUBLE},
    {"unsigned", TT::UNSIGNED},
    {"struct", TT::STRUCT},
    
    // {"enum", TokenType::ENUM},
    // {"float", TokenType::TYPE},
    // {"union", TokenType::TYPE},

    // keyword
    {"return", TT::RETURN},
    {"if", TT::IF},
    {"else", TT::ELSE},
    {"for", TT::FOR},
    {"while", TT::WHILE},
    {"do", TT::DO},
    {"continue", TT::CONTINUE},
    {"break", TT::BREAK},
    {"switch", TT::SWITCH},
    {"case", TT::CASE},
    {"default", TT::DEFAULT},

    // other
    {"sizeof", TT::OPERATOR},
    
    // {"include", TokenType::KEYWORD},
    // {"goto", TokenType::KEYWORD},
    // {"typedef", TokenType::KEYWORD},
    // {"__ignore_typecheck__", TokenType::KEYWORD},
    // {"const", TokenType::KEYWORD},
    
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
    {'v', '\v'},
    {'a', '\a'},
    {'0', '\0'}
};

// Read the next character of the file.
void Lexer::move_forward() {
    if (c == '\n') {
        ++row;
        col = 0;
    } else {
        ++col;
    }
    file.get(c);
}

// Get the next token, and print it if required.
Token Lexer::next() {
    Token ret = next_token();
    if (lex_flag && ret.type != TT::END) {
        ret.print();
    }
    return ret;
}

Token Lexer::next_token() {
    while (!file.eof()) {
        if (isspace(c)) {              // skip all kinds of space
            move_forward();
            continue;
        }
        if (c == '/') {                // maybe a comment
            Token t = next_comment();
            if (t.type != TT::COMMENT) {
                return t;
            }
            continue;
        }
        if (isdigit(c)) {              // number
            return next_number();
        }
        if (isalpha(c) || c == '_') {  // identifier or keyword
            return next_identifier();
        }
        if (c == '\'') {               // char
            return next_char();
        }
        if (c == '"') {                // string
            return next_string();
        }
        return next_symbol();          // operator or symbol
    }
    file.close();
    return Token(TT::END, "", row, col);
}

Token Lexer::next_comment() {
    Token ret(TT::COMMENT, "/", row, col);
    bool maybe_end = false;
    move_forward();
    switch(c) {
        case '/':           // single-line comment
            while (c != '\n' && !file.eof()) {
                move_forward();
            }
            return ret;
        case '*':           // multi-line comment
            while (true) {  // can only ends with "*/"
                move_forward();
                if (file.eof()) {
                    lexer_error("unterminated comment", ret.row);
                } else if (maybe_end || c == '/'){
                    move_forward();
                    return ret;
                }
                maybe_end = (c == '*');
            }
        default:            // operator "/" or "/="
            ret.type = TT::OPERATOR;
            if (c == '=') {
                ret.value = "/=";
                move_forward();
            }
            return ret;
    }
}

std::unordered_set<char> end_of_number {
    '+', '-', '*', '/', '%', '&', '|', '^', '<', '=', '>', '!',
    ')', ']', '}', ',', ';'
};

Token Lexer::next_number() {
    Token ret(TT::NUMBER, "", row, col);
    bool is_float = false;
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
        ret.value += c;
        move_forward();
    }
    auto it = end_of_number.find(c);
    if (it == end_of_number.end()) {
        lexer_error("invalid number", ret.row);
        do {
            move_forward();
            it = end_of_number.find(c);
        } while (it == end_of_number.end() && !file.eof());
    }
    return ret;
}

Token Lexer::next_identifier() {
    Token ret(TT::IDENTIFIER, "", row, col);
    while((isalnum(c) || c == '_') && !file.eof()) {
        ret.value += c;
        move_forward();
    }
    auto it = get_token_type.find(ret.value);
    if (it != get_token_type.end()) {  // keyword
        ret.type = it->second;
    }
    return ret;
}

Token Lexer::next_char() {
    Token ret(TT::CHAR, "", row, col);
    move_forward();
    ret.value = c;
    if (c == '\\') {  // escape character
        move_forward();
        auto it = get_escape_char.find(c);
        if (it != get_escape_char.end()) {
            ret.value = it->second;
        } else {
            lexer_error(std::format("unknown escape sequence '\\{}'", c), row);
        }
    }
    move_forward();
    if (c != '\'') {
        lexer_error("missing terminating ' character", row);
    }
    move_forward();
    return ret;
}

Token Lexer::next_string() {
    Token ret(TT::STRING, "", row, col);
    move_forward();
    string s;
    while (c != '"') {
        if (c == '\\') {             // escape character
            move_forward();
            if (file.eof()) {
                lexer_error("missing terminating \" character", ret.row);
            } else if (c == '\n') {  // line continuation
                move_forward();
                continue;
            }
            auto it = get_escape_char.find(c);
            if (it != get_escape_char.end()) {
                s += it->second;
            } else {
                lexer_error(std::format("unknown escape sequence '\\{}'", c), row);
            }
        } else {
            s += c;
        } 
        move_forward();
        if (file.eof()) {
            lexer_error("missing terminating \" character", row);
        }
    }
    ret.value = s;
    move_forward();
    return ret;
}

Token Lexer::next_symbol() {
    Token ret(TT::OPERATOR, string{c}, row, col);
    bool unary = true;
    char first = c;
    move_forward();
    char second = c;
    switch (first) {
        // operator
        case '+':
        case '*':
        case '%':
        case '&':
        case '|':
        case '^':
        case '<':
        case '>':
        case '=':
            if (first == second || second == '=') {
                ret.value = string{first, second};
                unary = false;
            }
            break;
        case '!':
            if (second == '=') {
                ret.value = string{first, second};
                unary = false;
            }
            break;
        
        case '-':
            if (second == '-' || second == '=' || second == '>') {
                ret.value = string{first, second};
                unary = false;
            }
            break;
        case '~':
        case '.':
        case '?':
        case ':':
            break;
        // symbol
        case '(': { ret.type = TT::L_PARENTHESIS; break; }
        case ')': { ret.type = TT::R_PARENTHESIS; break; }
        case '[': { ret.type = TT::L_BRACKET; break; }
        case ']': { ret.type = TT::R_BRACKET; break; }
        case '{': { ret.type = TT::L_BRACE; break; }
        case '}': { ret.type = TT::R_BRACE; break; }
        case ',': { ret.type = TT::COMMA; break; }
        case ';': { ret.type = TT::SEMICOLON; break; }
        case '#': { ret.type = TT::HASH; break; }
        case '\\':
            move_forward();
            if (c == '\n') {  // line continuation
                move_forward();
                return next_token();
            } else {
                lexer_error("stray '\\'", ret.row);
            }
        default:
            lexer_error(std::format("unknown symbol '{}'", first), ret.row);
    }
    if (!unary) {
        move_forward();
    }
    return ret;
}
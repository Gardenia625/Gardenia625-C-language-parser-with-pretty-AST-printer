#include "lexer.h"


void Token::print() {
    cout << '[' << std::format("{:>4}", std::to_string(row)) << ':'
         << std::format("{:>4}", std::to_string(col)) << "] ";
    switch (type) {
        case TokenType::ERROR:
            cout << COLOR_ERROR << "error token" << COLOR_RESET;
            break;
        case TokenType::NUMBER:
            cout << std::get<int>(value);
            break;
        case TokenType::CHAR:
            cout << '\'' << std::get<char>(value) << '\'';
            break;
        case TokenType::STRING:
            cout << '"' << std::get<char>(value) << '"' ;
            break;
        default:
            cout << std::get<string>(value);
    }
    cout << endl;
}

std::unordered_map<string, TokenType> get_token_type {
    // global declaration
    // {"enum", TokenType::ENUM},
    // type
    // {"unsigned", TokenType::TYPE},
    // {"signed", TokenType::TYPE},
    // {"char", TokenType::TYPE},
    // {"short", TokenType::TYPE},
    {"void", TokenType::TYPE},
    {"int", TokenType::INT},
    // {"float", TokenType::TYPE},
    // {"double", TokenType::TYPE},
    // {"long", TokenType::TYPE},
    // {"struct", TokenType::TYPE},
    // {"union", TokenType::TYPE},
    // {"static", TokenType::TYPE},
    // keyword
    {"return", TokenType::RETURN},
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

// 移动到文件的下一个字符
void Lexer::move_forward() {
    if (c == '\n') {
        ++row;
        col = 0;
    } else {
        ++col;
    }
    file.get(c);
}

// 获取下一个 token, 如有需要还会打印它
Token Lexer::next() {
    Token ret = next_token();
    if (lex_flag && ret.type != TokenType::END) {
        ret.print();
    }
    return ret;
}

Token Lexer::next_token() {
    while (!file.eof()) {
        if (isspace(c)) {              // 跳过空格
            move_forward();
            continue;
        }
        if (c == '/') {                // 可能是注释
            Token t = next_comment();
            if (t.type != TokenType::COMMENT) {
                return t;
            }
            continue;
        }
        if (isdigit(c)) {              // 数字
            return next_number();
        }
        if (isalpha(c) || c == '_') {  // 标识符 或 关键词
            return next_identifier();
        }
        if (c == '\'') {               // 字符
            return next_char();
        }
        if (c == '"') {                // 字符串
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
        case '/':                  // 单行注释
            while (c != '\n' && !file.eof()) {
                move_forward();
            }
            return ret;
        case '*':                  // 多行注释 (只能以 "*/" 为结尾)
            while (true) {
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
        default:                   // 运算符 "/" 或 "/="
            ret.type = TokenType::BINARY_OPERATOR;
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
    string name;
    while((isalnum(c) || c == '_') && !file.eof()) {
        name += c;
        move_forward();
    }
    ret.value = name;
    auto it = get_token_type.find(name);
    if (it != get_token_type.end()) {  // 关键词
        ret.type = it->second;
    }
    return ret;
}

Token Lexer::next_char() {
    Token ret(TokenType::CHAR, '\0', row, col);
    move_forward();
    ret.value = c;
    if (c == '\\') {  // 转义符号
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
    string s;
    while (c != '"') {
        if (c == '\\') {  // 转义符号
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
    Token ret(TokenType::SYMBOL, string{c}, row, col);
    char first = c;
    move_forward();
    char second = c;
    switch (first) {
        // 运算符
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
                ret.value = string{first, second};
            } else {
                ret.type = TokenType::UNARY_OPERATOR;
            }
            break;
        case '!':
            if (second == '=') {
                ret.type = TokenType::BINARY_OPERATOR;
                ret.value = string{first, second};
            } else {
                ret.type = TokenType::UNARY_OPERATOR;
            }
            break;
        case '~':
        case '(': { ret.type = TokenType::L_PARENTHESIS; break; }
        case '[':
        case ',':
        case '.':
        case '?':
            ret.type = TokenType::UNARY_OPERATOR;
            break;
        // 符号
        case '{': { ret.type = TokenType::L_BRACE; break; }
        case '}': { ret.type = TokenType::R_BRACE; break; }
        case ')': { ret.type = TokenType::R_PARENTHESIS; break; }
        case ']':
        case ':':
        case ';': { ret.type = TokenType::SEMICOLON; break; }
        case '#':
            break;
        case '\\':
            while (isspace(c) && c != '\n' && !file.eof()) {
                move_forward();
            }
            if (c != '\n' or file.eof()) {
                make_error("stray '\\'", ret.row, ret.col);
                ret.type = TokenType::ERROR;
            }
            break;
        default:
            make_error("unknown symbol", ret.row, ret.col);
            ret.type = TokenType::ERROR;
    }
    if (ret.type == TokenType::BINARY_OPERATOR) {
        move_forward();
    }
    return ret;
}
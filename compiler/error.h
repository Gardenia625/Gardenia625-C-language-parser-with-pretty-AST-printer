#ifndef HEADER_ERROR
#define HEADER_ERROR

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <format>
#include <set>


// All files will include this header, so the following code is placed here.
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

using std::vector;
using std::string;

// ANSI color
// 1 红
// 2 绿
// 3 黄 (亮)
// 4 蓝
// 5 紫
// 6 青
#define COLOR_ERROR "\033[1;91m"      // bold high intensity red
#define COLOR_TITLE "\033[1;93m"      // bold high intensity yellow
#define COLOR_CLASS "\033[1;36m"      // bold cyan
#define COLOR_COMPONENT "\033[0;36m"  // cyan
#define COLOR_TYPE "\033[0;34m"       // blue
#define COLOR_NORMAL "\033[0;32m"
#define COLOR_OPERATOR "\033[0;35m"   // purple
#define COLOR_RESET "\033[0m"


// Compilation will be terminated upon encountering an error.
void lexer_error(std::string_view message, int line);
void parser_error(std::string_view message, int line);

#endif
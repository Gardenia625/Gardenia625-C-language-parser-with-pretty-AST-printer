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


#define COLOR_ERROR "\033[1;91m"      // bold high intensity red
#define COLOR_TITLE "\033[1;92m"      // bold high intensity green
#define COLOR_CLASS "\033[1;96m"      // bold high intensity cyan
#define COLOR_COMPONENT "\033[0;36m"  // cyan
#define COLOR_TYPE "\033[0;34m"       // blue
#define COLOR_OPERATOR "\033[0;93m"   // high intensity yellow
#define COLOR_CONST "\033[0;95m"      // high intensity purple
#define COLOR_RESET "\033[0m"


// Compilation will be terminated upon encountering an error.
void lexer_error(std::string_view message, int line);
void parser_error(std::string_view message, int line);

#endif
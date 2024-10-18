#ifndef HEADER_ERROR
#define HEADER_ERROR

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <format>
#include <set>


// 所有文件都会包含这个头文件, 故将这些代码放在这里
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

using std::vector;
using std::string;

// ANSI 颜色
#define COLOR_ERROR "\033[1;91m"  // bold high intensity red
#define COLOR_TIP "\033[1;96m"    // bold high intensity cyan
#define COLOR_RESET "\033[0m"

struct CError {
    string message;
    int row;
    int col;
    bool operator<(const CError& other) const;
};

inline bool CError::operator<(const CError& other) const {
    if (row != other.row) {
        return row < other.row;
    } else {
        return col < other.col;
    }
}

extern std::set<CError> error_record;
inline void make_error(const string& message, int row, int col) {
    error_record.insert(CError(message, row, col));
}
void error_summary(string filename);
void parsing_error(const string& message, int row, int col);

#endif
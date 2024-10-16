#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <format>
#include <set>

// ANSI color
#define COLOR_ERROR "\033[1;91m"
#define COLOR_TIP "\033[1;96m"
#define COLOR_RESET "\033[0m"

struct CError {
    std::string message;
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
inline void make_error(const std::string& message, int row, int col) {
    error_record.insert(CError(message, row, col));
}
void error_summary(std::string filename);
#include "ScalarConverter.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <climits>
#include <cctype>
#include <cerrno>
#include <limits>

namespace {

/* -------------------------------------------------------------------- */
/*  Detection helpers                                                   */
/* -------------------------------------------------------------------- */

bool isCharLiteral(const std::string& s, char& out) {
    // plain single displayable, non-digit character: e.g. "a"
    if (s.size() == 1
        && !std::isdigit(static_cast<unsigned char>(s[0]))
        && std::isprint(static_cast<unsigned char>(s[0]))) {
        out = s[0];
        return true;
    }
    // quoted form: 'a'
    if (s.size() == 3 && s[0] == '\'' && s[2] == '\'') {
        out = s[1];
        return true;
    }
    return false;
}

bool isIntLiteral(const std::string& s, long& out) {
    if (s.empty())
        return false;
    size_t i = 0;
    if (s[i] == '+' || s[i] == '-')
        i++;
    if (i >= s.size())
        return false;
    for (size_t j = i; j < s.size(); j++) {
        if (!std::isdigit(static_cast<unsigned char>(s[j])))
            return false;
    }
    errno = 0;
    char* end;
    long val = std::strtol(s.c_str(), &end, 10);
    if (errno == ERANGE || *end != '\0')
        return false;
    out = val;
    return true;
}

bool isFloatLiteral(const std::string& s, float& out) {
    if (s == "nanf") { out = std::numeric_limits<float>::quiet_NaN(); return true; }
    if (s == "+inff") { out = std::numeric_limits<float>::infinity(); return true; }
    if (s == "-inff") { out = -std::numeric_limits<float>::infinity(); return true; }

    if (s.size() < 2 || s[s.size() - 1] != 'f')
        return false;
    std::string num = s.substr(0, s.size() - 1);
    if (num.empty())
        return false;

    size_t i = 0;
    if (num[i] == '+' || num[i] == '-')
        i++;
    if (i >= num.size())
        return false;

    bool hasDot = false;
    size_t digitsBefore = 0, digitsAfter = 0;
    bool afterDot = false;
    for (; i < num.size(); i++) {
        if (num[i] == '.') {
            if (hasDot)
                return false;
            hasDot = true;
            afterDot = true;
            continue;
        }
        if (!std::isdigit(static_cast<unsigned char>(num[i])))
            return false;
        if (afterDot)
            digitsAfter++;
        else
            digitsBefore++;
    }
    (void)digitsAfter;
    if (!hasDot || digitsBefore == 0)
        return false;

    char* end;
    out = std::strtof(num.c_str(), &end);
    if (*end != '\0')
        return false;
    return true;
}

bool isDoubleLiteral(const std::string& s, double& out) {
    if (s == "nan") { out = std::numeric_limits<double>::quiet_NaN(); return true; }
    if (s == "+inf") { out = std::numeric_limits<double>::infinity(); return true; }
    if (s == "-inf") { out = -std::numeric_limits<double>::infinity(); return true; }
    if (s.empty())
        return false;

    size_t i = 0;
    if (s[i] == '+' || s[i] == '-')
        i++;
    if (i >= s.size())
        return false;

    bool hasDot = false;
    size_t digitsBefore = 0, digitsAfter = 0;
    bool afterDot = false;
    for (; i < s.size(); i++) {
        if (s[i] == '.') {
            if (hasDot)
                return false;
            hasDot = true;
            afterDot = true;
            continue;
        }
        if (!std::isdigit(static_cast<unsigned char>(s[i])))
            return false;
        if (afterDot)
            digitsAfter++;
        else
            digitsBefore++;
    }
    (void)digitsAfter;
    if (!hasDot || digitsBefore == 0)
        return false;

    char* end;
    out = std::strtod(s.c_str(), &end);
    if (*end != '\0')
        return false;
    return true;
}

/* -------------------------------------------------------------------- */
/*  Printing helpers (all take the value as a double "source of truth") */
/* -------------------------------------------------------------------- */

void printChar(double d) {
    if (std::isnan(d) || std::isinf(d)
        || d < static_cast<double>(CHAR_MIN) || d > static_cast<double>(CHAR_MAX)) {
        std::cout << "char: impossible" << std::endl;
        return;
    }
    char c = static_cast<char>(d);
    if (std::isprint(static_cast<unsigned char>(c)))
        std::cout << "char: '" << c << "'" << std::endl;
    else
        std::cout << "char: Non displayable" << std::endl;
}

void printInt(double d) {
    if (std::isnan(d) || std::isinf(d)
        || d < static_cast<double>(INT_MIN) || d > static_cast<double>(INT_MAX)) {
        std::cout << "int: impossible" << std::endl;
        return;
    }
    std::cout << "int: " << static_cast<int>(d) << std::endl;
}

void printFloat(double d) {
    if (std::isnan(d)) { std::cout << "float: nanf" << std::endl; return; }
    if (std::isinf(d)) { std::cout << "float: " << (d < 0 ? "-inff" : "+inff") << std::endl; return; }

    float f = static_cast<float>(d);
    std::cout << "float: " << f;
    if (std::fabs(f) < 1e18 && f == static_cast<long>(f))
        std::cout << ".0";
    std::cout << "f" << std::endl;
}

void printDouble(double d) {
    if (std::isnan(d)) { std::cout << "double: nan" << std::endl; return; }
    if (std::isinf(d)) { std::cout << "double: " << (d < 0 ? "-inf" : "+inf") << std::endl; return; }

    std::cout << "double: " << d;
    if (std::fabs(d) < 1e18 && d == static_cast<long>(d))
        std::cout << ".0";
    std::cout << std::endl;
}

} // namespace

/* -------------------------------------------------------------------- */
/*  ScalarConverter::convert                                            */
/* -------------------------------------------------------------------- */

void ScalarConverter::convert(const std::string& literal) {
    char c;
    long l;
    float f;
    double d;

    if (isCharLiteral(literal, c)) {
        double val = static_cast<double>(c);
        printChar(val);
        printInt(val);
        printFloat(val);
        printDouble(val);
    } else if (isIntLiteral(literal, l)) {
        double val = static_cast<double>(l);
        printChar(val);
        printInt(val);
        printFloat(val);
        printDouble(val);
    } else if (isFloatLiteral(literal, f)) {
        double val = static_cast<double>(f);
        printChar(val);
        printInt(val);
        printFloat(val);
        printDouble(val);
    } else if (isDoubleLiteral(literal, d)) {
        printChar(d);
        printInt(d);
        printFloat(d);
        printDouble(d);
    } else {
        std::cout << "Invalid literal." << std::endl;
    }
}
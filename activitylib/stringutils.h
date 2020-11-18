#pragma once

#include <iomanip>
#include <sstream>


template<typename T>
bool fromString(const std::string& str, T& value)
{
    std::stringstream ss(str);

    ss >> value;
    return !ss.bad() && ss.eof();
}

template<typename T>
std::string floatToString(T t, size_t precision)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << t;

    return ss.str();
}

static inline bool endsWith(const std::string& str, const std::string& ending)
{
    if (str.length() <  ending.length())
    {
        return false;
    }

    return str.substr(str.length() - ending.length()) == ending;
}
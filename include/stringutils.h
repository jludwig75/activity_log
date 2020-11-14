#pragma once

#include <sstream>


template<typename T>
bool fromString(const std::string& str, T& value)
{
    std::stringstream ss(str);

    ss >> value;
    return !ss.bad() && ss.eof();
}


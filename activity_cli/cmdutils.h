#pragma once

#include <string>


template<typename T>
std::string csvEntry(const T& t)
{
    return std::string(",") + std::to_string(t);
}

size_t commandThreadPoolCount();
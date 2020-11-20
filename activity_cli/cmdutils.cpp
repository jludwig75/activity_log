#include "cmdutils.h"

#include <algorithm>
#include <iostream>

#include <boost/format.hpp>

#include <sys/sysinfo.h>


size_t commandThreadPoolCount()
{
    auto nprocs = get_nprocs();

    return std::max(2 * nprocs, 4);
}

bool isValidFilePath(const std::string& path)
{
    return path.find('?') == std::string::npos && path.find('*') == std::string::npos;
}
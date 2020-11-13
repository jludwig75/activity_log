#include "file_utils.h"

#include <algorithm>
#include <cassert>
#include <cwctype>


#define MAX_CHARS_TO_COMPARE    256


namespace
{
    bool startsWith(const std::string& str, const std::string& begin)
    {
        if (str.length() < begin.length())
        {
            return false;
        }

        return str.substr(0, begin.length()) == begin;
    }
}


bool appearsToBeTextFileData(const std::string& fileData)
{
    for (char c : fileData.substr(0, MAX_CHARS_TO_COMPARE))
    {
        if (!std::isprint(c) && !iswspace(c))
        {
            return false;
        }
    }

    auto length = fileData.length();
    if (length > MAX_CHARS_TO_COMPARE)
    {
        for (char c : fileData.substr(length - MAX_CHARS_TO_COMPARE))
        {
            if (!std::isprint(c) && !iswspace(c))
            {
                return false;
            }
        }
    }

    return true;
}

bool apperasToBeXmlFileData(const std::string& fileData)
{
    if (!appearsToBeTextFileData(fileData))
    {
        return false;
    }

    auto itFirst = std::find_if(fileData.begin(), fileData.end(), [](char c){ return !std::iswspace(c); });
    if (itFirst == fileData.end() || *itFirst != '<')
    {
        return false;
    }

    if (startsWith(fileData.substr(itFirst - fileData.begin()), "<?xml"))
    {
        return true;
    }

    if (startsWith(fileData.substr(itFirst - fileData.begin()), "<?XML"))
    {
        return true;
    }

    if (startsWith(fileData.substr(itFirst - fileData.begin()), "<!--"))
    {
        return true;
    }

    auto itLast = std::find_if(fileData.rbegin(), fileData.rend(), [](char c){ return !std::iswspace(c); });
    // We know there is a non-whitespace character in the file data, because the test at the begining passed.
    assert(itLast != fileData.rend());

    return *itLast == '>';
}

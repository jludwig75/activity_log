#pragma once

#include <string>


bool readFile(const std::string& fileName, std::string& fileData);
bool writeFile(const std::string& fileName, const std::string& fileData);

#pragma once

#include <chrono>
#include <string>


std::string toTimeString(std::chrono::system_clock::time_point time);

std::string toDuration(std::chrono::system_clock::duration duration);
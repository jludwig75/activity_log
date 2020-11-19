#include "datetime.h"

#include <iomanip>
#include <sstream>


namespace
{

const std::string TIME_FORMAT_STRING = "%B %e, %Y at %I:%M: %p";

} // namespace


std::string toTimeString(std::chrono::system_clock::time_point time)
{
    auto t = std::chrono::system_clock::to_time_t(time);
    auto tm = *std::localtime(&t);  // TODO: this may change when timezone 
                                    // support is added. Likely we will apply
                                    // a time offset and use std::gmtime.
    std::stringstream ss;
    ss << std::put_time(&tm, TIME_FORMAT_STRING.c_str());
    return ss.str();
}

std::string toDuration(std::chrono::system_clock::duration duration)
{
    auto durationCount = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

    auto seconds = durationCount % 60;
    durationCount /= 60;
    auto minutes = durationCount % 60;
    auto hours = durationCount / 60;

    std::stringstream ss;
    ss << hours << ":" << std::setfill('0') << std::setw(2) << minutes << ":" << seconds;

    return ss.str();
}
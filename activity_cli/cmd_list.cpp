#include "cmd_list.h"

#include <boost/format.hpp>

#include "activity_log.h"
#include "units.h"


ListCommandHandler::ListCommandHandler(const std::string& name)
    :
    CommandHandler(name)
{
}

int ListCommandHandler::runCommand(const std::vector<std::string>& args)
{
    auto activityLog = createActivityLog();

    std::vector<Activity> activities;
    if (!activityLog->listActivities(std::chrono::system_clock::from_time_t(0), std::chrono::system_clock::now(), activities).ok())
    {
        std::cout << "Failed to list activities\n";
        return -1;
    }

    std::cout << boost::format("Retrieved %1% activities:\n") % activities.size();
    for (const auto& activity : activities)
    {
        std::cout << boost::format("\t%1% - \"%2%\": %3% miles, %4% feet ascent\n") % activity.id() % activity.name() % m_to_miles(activity.totalDistance()) % m_to_ft(activity.totalAscent());
    }

    return 0;
}

std::string ListCommandHandler::syntax() const
{
    return name();
}

std::string ListCommandHandler::description() const
{
    return "lists stored activities";
}

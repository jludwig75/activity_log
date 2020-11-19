
#include "cmd_list.h"

#include <boost/format.hpp>

#include "activity_log.h"
#include "datetime.h"
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
        std::cout << boost::format("\t%1% - %2% - \"%3%\": %4% miles, %5% feet, %6%\n") %
                activity.id() %
                toTimeString(activity.startTime()) %
                activity.name() %
                m_to_miles(activity.totalDistance()) %
                m_to_ft(activity.totalAscent()) %
                toDuration(activity.duration());
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

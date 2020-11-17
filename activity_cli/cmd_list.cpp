#include "cmd_list.h"

#include "grpcinit.h"

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

    std::cout << "Retrieved " << std::to_string(activities.size()) << " activities:\n";
    for (const auto& activity : activities)
    {
        std::cout << "\t" <<  activity.id() << " - \"" << activity.name() << "\": " << m_to_miles(activity.totalDistance()) << " miles, " << m_to_ft(activity.totalAscent()) << " feet ascent\n";
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

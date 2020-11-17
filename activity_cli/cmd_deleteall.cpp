#include "cmd_deleteall.h"

#include "grpcinit.h"


DeleteAllCommandHandler::DeleteAllCommandHandler(const std::string& name)
    :
    CommandHandler(name)
{
}

int DeleteAllCommandHandler::runCommand(const std::vector<std::string>& args)
{
    auto activityLog = createActivityLog();

    std::vector<Activity> activities;
    if (!activityLog->listActivities(std::chrono::system_clock::from_time_t(0), std::chrono::system_clock::now(), activities).ok())
    {
        std::cout << "Failed to list activities\n";
        return -1;
    }

    for (auto& activity : activities)
    {
        if (!activity.del().ok())
        {
            std::cout << "Failed to delete activity " << args[0] << std::endl;
            return -1;
        }
    }

    return 0;
}

std::string DeleteAllCommandHandler::syntax() const
{
    return name();
}
std::string DeleteAllCommandHandler::description() const
{
    return "deletes all activities";
}

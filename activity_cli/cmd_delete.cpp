#include "cmd_delete.h"

#include "grpcinit.h"


DeleteCommandHandler::DeleteCommandHandler(const std::string& name)
    :
    CommandHandler(name)
{
}

int DeleteCommandHandler::runCommand(const std::vector<std::string>& args)
{
    auto activityLog = createActivityLog();

    Activity activity;
    auto status = activityLog->getActivity(args[0], activity);
    if (!status.ok())
    {
        std::cout << "Failed to find activity " << args[0] << std::endl;
        return -1;
    }

    if (!activity.del().ok())
    {
        std::cout << "Failed to delete activity " << args[0] << std::endl;
        return -1;
    }

    return 0;
}

std::string DeleteCommandHandler::syntax() const
{
    return name() + " <activity_id>";
}

std::string DeleteCommandHandler::description() const
{
    return "deletes and activity";
}

size_t DeleteCommandHandler::minNumberOfArgs() const
{
    return 1;
}

size_t DeleteCommandHandler::maxNumberOfArgs() const
{
    return 1;
}

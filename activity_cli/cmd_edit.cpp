#include "cmd_edit.h"

#include "activity_log.h"


EditCommandHandler::EditCommandHandler(const std::string& name)
    :
    CommandHandler(name)
{
}

int EditCommandHandler::runCommand(const std::vector<std::string>& args)
{
    auto activityLog = createActivityLog();

    Activity activity;
    auto status = activityLog->getActivity(args[0], activity);
    if (!status.ok())
    {
        std::cout << "Failed to find activity " << args[0] << std::endl;
        return -1;
    }

    status = activity.edit(args[1]);
    if (!status.ok())
    {
        std::cout << "Failed to update activity name " << args[0] << std::endl;
        return -1;
    }

    return 0;
}

std::string EditCommandHandler::syntax() const
{
    return name() + " <activity_id> <new_activity_name>";
}

std::string EditCommandHandler::description() const
{
    return "Update the name of an activity";
}

int EditCommandHandler::minNumberOfArgs() const
{
    return 2;
}

int EditCommandHandler::maxNumberOfArgs() const
{
    return 2;
}



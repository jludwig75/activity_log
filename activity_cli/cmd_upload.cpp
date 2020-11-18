#include "cmd_upload.h"

#include <iostream>

#include "activity_log.h"


UploadCommandHandler::UploadCommandHandler(const std::string& name)
    :
    CommandHandler(name)
{
}

int UploadCommandHandler::runCommand(const std::vector<std::string>& args)
{
    auto activityLog = createActivityLog();

    Activity newActivity;
    if (!activityLog->uploadActivity(args[0], newActivity).ok())
    {
        std::cout << "Failed to upload activity \"" << args[0] << "\"\n";
        return -1;
    }

    std::cout << "Successfully uploaded activity file \"" << args[0] << "\" as activity " << newActivity.id() + "\n";

    return 0;
}
std::string UploadCommandHandler::syntax() const
{
    return UploadCommandHandler::name() + " <activity_file_name>";
}
std::string UploadCommandHandler::description() const
{
    return "uploads an activity file (currently only supports gpx)";
}
size_t UploadCommandHandler::minNumberOfArgs() const
{
    return 1;
}
size_t UploadCommandHandler::maxNumberOfArgs() const
{
    return 1;
}

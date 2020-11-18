#include "cmd_download.h"

#include <iostream>

#include "activity_log.h"


DownloadCommandHandler::DownloadCommandHandler(const std::string& name)
    :
    CommandHandler(name)
{
}

int DownloadCommandHandler::runCommand(const std::vector<std::string>& args)
{
    auto activityLog = createActivityLog();

    Activity activity;
    auto status = activityLog->getActivity(args[0], activity);
    if (!status.ok())
    {
        std::cout << "Failed to find activity " << args[0] << std::endl;
        return -1;
    }

    return activity.downloadToGpx(args[1]).ok() ? 0 : -1;
}

std::string DownloadCommandHandler::syntax() const
{
    return name() + " <activity_id> <output_file_name>";
}

std::string DownloadCommandHandler::description() const
{
    return "download activity GPX file";
}

int DownloadCommandHandler::minNumberOfArgs() const
{
    return 2;
}

int DownloadCommandHandler::maxNumberOfArgs() const
{
    return 2;
}

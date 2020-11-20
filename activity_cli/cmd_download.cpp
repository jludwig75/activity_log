#include "cmd_download.h"

#include <iostream>

#include <boost/format.hpp>

#include "activity_log.h"
#include "cmdutils.h"


DownloadCommandHandler::DownloadCommandHandler(const std::string& name)
    :
    CommandHandler(name)
{
}

int DownloadCommandHandler::runCommand(const std::vector<std::string>& args)
{
    auto activityId = args[0];
    auto fileName = args[1];
    if (!isValidFilePath(fileName))
    {
        std::cout << boost::format("Invalid file path \"%1%\"\n") % fileName;
        return false;
    }

    auto activityLog = createActivityLog();

    Activity activity;
    auto status = activityLog->getActivity(activityId, activity);
    if (!status.ok())
    {
        std::cout << "Failed to find activity " << activityId << std::endl;
        return -1;
    }

    return activity.downloadToGpx(fileName).ok() ? 0 : -1;
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

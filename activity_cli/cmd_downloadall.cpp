#include "cmd_downloadall.h"

#include <iostream>

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <boost/format.hpp>

#include "activity_log.h"
#include "cmdutils.h"


DownloadAllCommandHandler::DownloadAllCommandHandler(const std::string& name)
    :
    CommandHandler(name)
{
}

std::string activityNameToFileName(const std::string& activityName, const std::string& activityId)
{
    std::string fileName;
    for (auto c: activityName)
    {
        if (std::isalnum(c))
        {
            fileName += c;
        }
        else if (fileName.back() != '_')
        {
            fileName += '_';
        }
    }

    return fileName + "_" + activityId + ".gpx";
}

int DownloadAllCommandHandler::runCommand(const std::vector<std::string>& args)
{
    auto activityLog = createActivityLog();

    std::vector<Activity> activities;
    if (!activityLog->listActivities(std::chrono::system_clock::from_time_t(0), std::chrono::system_clock::now(), activities).ok())
    {
        std::cout << "Failed to list activities\n";
        return -1;
    }

    boost::asio::thread_pool threadPool(commandThreadPoolCount());

    for (auto& activity : activities)
    {
        boost::asio::post(threadPool, [activity] {
            auto activityFileName = activityNameToFileName(activity.name(), activity.id());
            std::cout << boost::format("Downloading activity %1% to %2%...\n") % activity.id() % activityFileName;
            if (!activity.downloadToGpx(activityFileName).ok())
            {
                std::cout << boost::format("Failed to download activity %1% - \"%2%\"\n") % activity.id() % activity.name();
            }
        });
    }

    threadPool.join();

    return 0;
}

std::string DownloadAllCommandHandler::syntax() const
{
    return name();
}

std::string DownloadAllCommandHandler::description() const
{
    return "download all activities to GPX files";
}

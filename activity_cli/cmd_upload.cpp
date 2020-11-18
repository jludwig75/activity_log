#include "cmd_upload.h"

#include <iostream>

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

#include "activity_log.h"


UploadCommandHandler::UploadCommandHandler(const std::string& name)
    :
    CommandHandler(name)
{
}

int UploadCommandHandler::runCommand(const std::vector<std::string>& args)
{
    auto activityLog = createActivityLog();

    boost::asio::thread_pool threadPool(6);

    for (const auto& fileName: args)
    {
        boost::asio::post(threadPool, [activityLog, fileName] {
            Activity newActivity;
            if (!activityLog->uploadActivity(fileName, newActivity).ok())
            {
                std::cout << "Failed to upload activity \"" << fileName << "\"\n";
                return -1;
            }

            std::cout << "Successfully uploaded activity file \"" << fileName << "\" as activity " << newActivity.id() + "\n";
            return 0;
        });
    }

    threadPool.join();

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

int UploadCommandHandler::minNumberOfArgs() const
{
    return 1;
}
int UploadCommandHandler::maxNumberOfArgs() const
{
    return -11;
}

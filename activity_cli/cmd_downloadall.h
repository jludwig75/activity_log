#pragma once

#include "cmd.h"


class DownloadAllCommandHandler : public CommandHandler
{
public:
    DownloadAllCommandHandler(const std::string& name);

    int runCommand(const std::vector<std::string>& args) override;
    std::string syntax() const override;
    std::string description() const override;
};

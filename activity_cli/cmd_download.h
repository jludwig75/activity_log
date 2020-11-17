#pragma once

#include "cmd.h"


class DownloadCommandHandler : public CommandHandler
{
public:
    DownloadCommandHandler(const std::string& name);

    int runCommand(const std::vector<std::string>& args) override;
    std::string syntax() const override;
    std::string description() const override;
    size_t minNumberOfArgs() const override;
    size_t maxNumberOfArgs() const override;
};

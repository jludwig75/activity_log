#pragma once

#include "cmd.h"


class UploadCommandHandler : public CommandHandler
{
public:
    UploadCommandHandler(const std::string& name);

    int runCommand(const std::vector<std::string>& args);
    std::string syntax() const;
    std::string description() const;
    int minNumberOfArgs() const override;
    int maxNumberOfArgs() const override;
};

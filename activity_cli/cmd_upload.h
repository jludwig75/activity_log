#pragma once

#include "cmd.h"


class UploadCommandHandler : public CommandHandler
{
public:
    UploadCommandHandler(const std::string& name);

    int runCommand(const std::vector<std::string>& args);
    std::string syntax() const;
    std::string description() const;
    size_t minNumberOfArgs() const override;
    size_t maxNumberOfArgs() const override;
};

#pragma once

#include <string>
#include <vector>


class CommandHandler
{
public:
    CommandHandler(const std::string& name);
    virtual ~CommandHandler() {}
    virtual int runCommand(const std::vector<std::string>& args) = 0;
    virtual std::string syntax() const = 0;
    virtual std::string description() const = 0;
    virtual int minNumberOfArgs() const
    {
        return 0;
    }
    virtual int maxNumberOfArgs() const
    {
        return 0;
    }
protected:
    const std::string& name() const;
private:
    const std::string _name;
};

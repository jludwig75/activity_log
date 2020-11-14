#pragma once

#include <functional>


class OnExit
{
public:
    using ExitFunc = std::function<void(void)>;
    OnExit(ExitFunc&& exitFunc)
        :
        _exitFunc(exitFunc)
    {

    }
    ~OnExit()
    {
        _exitFunc();
    }
private:
    ExitFunc _exitFunc;
};
#define ON_EXIT(func)   OnExit onExitBloc_##__FILE__##__LINE__(func)

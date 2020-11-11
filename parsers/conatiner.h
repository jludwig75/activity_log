#pragma once

#include <cassert>
#include <condition_variable>
#include <deque>
#include <mutex>

template<typename DataType>
class Container
{
public:
    Container()
        :
        _done_pushing(false)
    {
    }

    bool pop(DataType& data)
    {
        std::unique_lock lock(_lock);
        _cond.wait(lock, [this] { return !_deque.empty() || (_done_pushing && _deque.empty()); } );
        if (_done_pushing && _deque.empty())
        {
            return false;
        }
        assert(!_deque.empty());

        data = _deque.front();
        _deque.pop_front();
        return true;
    }

    void push(const DataType& data)
    {
        {
            std::unique_lock lock(_lock);
            assert(!_done_pushing);
            _deque.push_back(data);
        }
        _cond.notify_one();
    }

    void done_pushing()
    {
        {
            std::unique_lock lock(_lock);
            _done_pushing = true;
        }
        _cond.notify_one();
    }

    size_t size() const
    {
        std::unique_lock lock(_lock);
        return _deque.size();
    }

private:
    std::deque<DataType> _deque;
    mutable std::mutex _lock;
    mutable std::condition_variable _cond;
    bool _done_pushing;
};
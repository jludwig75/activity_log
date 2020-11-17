#pragma once

#include <cassert>
#include <condition_variable>
#include <deque>
#include <mutex>

template<typename DataType>
class InterThreadQueue
{
public:
    InterThreadQueue()
        :
        _done_pushing(false)
    {
    }

    InterThreadQueue(const InterThreadQueue& other) = delete;
    InterThreadQueue& operator=(const InterThreadQueue& other) = delete;

    // I don't think this class should be moveable. std::mutex is not.
    // I think it would be a big problem if the queue got moved in one thread
    // but not another.
    // This seems inherently bad for an object that is shared between multiple
    // clients.
    InterThreadQueue(InterThreadQueue&& other) = delete;
    InterThreadQueue& operator=(InterThreadQueue&& other) = delete;

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

private:
    std::deque<DataType> _deque{};
    mutable std::mutex _lock{};
    mutable std::condition_variable _cond{};
    bool _done_pushing;
};
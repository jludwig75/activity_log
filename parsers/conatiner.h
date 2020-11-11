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

    Container(const Container& other)
        :
        _deque(other._deque),
        _done_pushing(true)
    {
        // TODO: Not sure if it makse sense to always assert this.
        // Might be too obscure, but it could be a bug.
        assert(other._done_pushing);
    }
    Container& operator=(const Container& other)
    {
        // TODO: Not sure if it makse sense to always assert this.
        // Might be too obscure, but it could be a bug.
        assert(other._done_pushing);
        _deque = other._deque;
        _done_pushing = true;   // Can't handle this, because there will be no writer.
        return *this;
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

    bool empty() const
    {
        return _deque.empty();
    }

    typename std::deque<DataType>::iterator begin()
    {
        return _deque.begin();
    }

    typename std::deque<DataType>::iterator end()
    {
        return _deque.end();
    }

    typename std::deque<DataType>::const_iterator begin() const
    {
        return _deque.begin();
    }

    typename std::deque<DataType>::const_iterator end() const
    {
        return _deque.end();
    }

    typename std::deque<DataType>::const_iterator cbegin() const
    {
        return _deque.cbegin();
    }

    typename std::deque<DataType>::const_iterator cend() const
    {
        return _deque.cend();
    }

    DataType& front()
    {
        return _deque.front();
    }

    DataType& back()
    {
        return _deque.back();
    }

    const DataType& front() const
    {
        return _deque.front();
    }

    const DataType& back() const
    {
        return _deque.back();
    }

private:
    std::deque<DataType> _deque{};
    mutable std::mutex _lock{};
    mutable std::condition_variable _cond{};
    bool _done_pushing;
};
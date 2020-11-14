#include "threadfile.h"

#include <fcntl.h>
 #include <string.h>
 #include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "scopeguard.h"

namespace threadfile
{

FileChunk::FileChunk(size_t maxChunkSize)
    :
    _buffer(nullptr),
    _maxChunkSize(maxChunkSize),
    _size(0)
{
    _buffer = new uint8_t[_maxChunkSize];
}

FileChunk::FileChunk(const FileChunk& other)
    :
    _buffer(nullptr),
    _maxChunkSize(other._maxChunkSize),
    _size(other._size)
{
    _buffer = new uint8_t[_maxChunkSize]{};
    if (_size > 0)
    {
        memcpy(_buffer, other._buffer, _size);
    }
}

FileChunk& FileChunk::operator=(const FileChunk& other)
{
    _buffer = nullptr;
    _maxChunkSize = other._maxChunkSize;
    _size = other._size;
    _buffer = new uint8_t[_maxChunkSize]{};
    if (_size > 0)
    {
        memcpy(_buffer, other._buffer, _size);
    }

    return *this;
}

FileChunk::FileChunk(FileChunk&& other)
    :
    _buffer(other._buffer),
    _maxChunkSize(other._maxChunkSize),
    _size(other._size)
{
    other._buffer = nullptr;
    other._maxChunkSize = 0;
    other._size = 0;
}

FileChunk& FileChunk::operator=(FileChunk&& other)
{
    _buffer = other._buffer;
    _maxChunkSize = other._maxChunkSize;
    _size = other._size;
    other._buffer = nullptr;
    other._maxChunkSize = 0;
    other._size = 0;

    return *this;
}


FileChunk::~FileChunk()
{
    delete[] _buffer;
}

uint8_t* FileChunk::data()
{
    return _buffer;
}

const uint8_t* FileChunk::data() const
{
    return _buffer;
}

size_t FileChunk::maxSize() const
{
    return _maxChunkSize;
}


size_t FileChunk::size() const
{
    return _size;
}

void FileChunk::setSize(size_t size)
{
    _size = size;
}

bool readFile(const std::string& fileName, size_t maxChunkSize, Container<FileChunk>& stream)
{
    auto fd = open(fileName.c_str(), O_RDONLY);
    if (fd < 0)
    {
        return false;
    }
    ON_EXIT(([fd, &stream]{
        close(fd);
        stream.done_pushing();
    }));

    while (true)
    {
        FileChunk chunk(maxChunkSize);

        auto bytesRead = read(fd, chunk.data(), chunk.maxSize());
        if (bytesRead < 0)
        {
            return false;
        }
        if (bytesRead == 0)
        {
            break;
        }
        chunk.setSize(bytesRead);
        stream.push(std::move(chunk));
    }

    return true;
}

bool writeFile(const std::string& fileName, Container<FileChunk>& stream)
{
    auto fd = open(fileName.c_str(), O_CREAT | O_WRONLY, DEFFILEMODE);
    if (fd < 0)
    {
        return false;
    }
    ON_EXIT([fd]{ close(fd); });

    for (const auto& chunk : stream)
    {
        size_t writeOffset = 0;
        while (writeOffset < chunk.size())
        {
            auto bytesWritten = write(fd, chunk.data() + writeOffset, chunk.size() - writeOffset);
            if (bytesWritten <= 0)
            {
                return false;
            }

            writeOffset += bytesWritten;
        }
    }

    return true;
}


} // namespace threadfile
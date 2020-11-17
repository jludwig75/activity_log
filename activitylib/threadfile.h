#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>

#include "interthreadqueue.h"


namespace threadfile
{

class FileChunk
{
public:
    FileChunk();
    FileChunk(size_t maxChunkSize);
    ~FileChunk();
    FileChunk(const FileChunk& other);
    FileChunk& operator=(const FileChunk& other);
    FileChunk(FileChunk&& other);
    FileChunk& operator=(FileChunk&& other);
    uint8_t* data();
    const uint8_t* data() const;
    size_t maxSize() const;
    size_t size() const;
    void setSize(size_t size);
private:
    uint8_t* _buffer;
    size_t _maxChunkSize;
    size_t _size;
};


bool readFile(const std::string& fileName, size_t maxChunkSize, InterThreadQueue<FileChunk>& stream);
bool writeFile(const std::string& fileName, InterThreadQueue<FileChunk>& stream);


} // namespace threadfile
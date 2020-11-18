#include "fileutils.h"

#include <thread>

#include <string.h>

#include "threadfile.h"


// TODO: Re-write this without threadfile

const size_t kTestFileChunkSize = 1024 * 1024;

bool readFile(const std::string& fileName, std::string& fileData)
{
    InterThreadQueue<threadfile::FileChunk> chunks;
    bool readerSuccess = false;
    std::thread reader([fileName, &chunks, &readerSuccess]{
        readerSuccess = threadfile::readFile(fileName, kTestFileChunkSize, chunks);
    });

    threadfile::FileChunk chunk;
    while (chunks.pop(chunk))
    {
        fileData += std::string(chunk.data(), chunk.data() + chunk.size());
    }

    reader.join();
    if (!readerSuccess)
    {
        return false;
    }

    return true;
}

bool writeFile(const std::string& fileName, const std::string& fileData)
{
    InterThreadQueue<threadfile::FileChunk> chunks;
    std::thread chunker([fileName, fileData, &chunks]{
        std::string::size_type offset = 0;
        while (offset < fileData.length())
        {
            threadfile::FileChunk chunk(kTestFileChunkSize);
            auto chunkString = fileData.substr(offset, chunk.maxSize());
            memcpy(chunk.data(), chunkString.data(), chunkString.length());
            chunk.setSize(chunkString.length());
            offset += chunkString.length();

            chunks.push(std::move(chunk));
        }
        chunks.done_pushing();
    });

    auto ret = threadfile::writeFile(fileName, chunks);
    chunker.join();
    return ret;
}

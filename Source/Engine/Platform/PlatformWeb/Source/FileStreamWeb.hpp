#pragma once
#if FARO_OS_WEB
#include <FileStream.hpp>
#include <Containers/Path.hpp>

namespace Faro
{
    class FileStreamWeb : public FileStream
    {
    public:
        bool IsOpen() override;

        void Init(Path file, EFileMode mode);
        void Init() override;
        uint32 Read(void* destination, uint16 elementSize, uint32 elementCount) override;
        uint32 Write(void* source, uint16 elementSize, uint32 elementCount) override;
        uint32 Size() override;
        uint32 Tell() override;
        void Seek(EStreamSeekOrigin origin, int32 offset) override;
    };
}
#endif

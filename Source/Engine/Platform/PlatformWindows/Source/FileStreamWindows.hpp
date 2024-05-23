#pragma once
#if FARO_PLATFORM_WINDOWS
#include <FileStream.hpp>
#include <Containers/Path.hpp>

namespace Faro
{
    class FileStreamWindows : public FileStream
    {
    public:
        uint32 Read(void* destination, uint16 elementSize, uint32 elementCount) override;
        uint32 Write(void* source, uint16 elementSize, uint32 elementCount) override;
        uint32 Size() override;
        uint32 Tell() override;
        void Seek(EStreamSeekOrigin origin, int32 offset) override;

        bool IsOpen() override;

        void Init(Path file, EFileMode mode);
        void Init() override;

    protected:
        void Destroy() override;

        Path file = String("");
        EFileMode mode;

        FILE* stream = nullptr;
        uint32 size = 0;
    };
}
#endif

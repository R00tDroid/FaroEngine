#if FARO_OS_WEB
#include "FileStreamWeb.hpp"

namespace Faro
{
    bool FileStreamWeb::IsOpen()
    {
        return false;
    }

    void FileStreamWeb::Init(Path, EFileMode)
    {
    }

    void FileStreamWeb::Init()
    {
        FileStream::Init();
    }

    uint32 FileStreamWeb::Read(void*, uint16, uint32)
    {
        return 0;
    }

    uint32 FileStreamWeb::Write(void*, uint16, uint32)
    {
        return 0;
    }

    uint32 FileStreamWeb::Size()
    {
        return 0;
    }

    uint32 FileStreamWeb::Tell()
    {
        return 0;
    }

    void FileStreamWeb::Seek(EStreamSeekOrigin, int32)
    {
    }
}
#endif

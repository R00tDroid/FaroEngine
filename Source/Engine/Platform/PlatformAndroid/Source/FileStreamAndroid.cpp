#if FARO_OS_ANDROID
#include "FileStreamAndroid.hpp"

namespace Faro
{
    bool FileStreamAndroid::IsOpen()
    {
        return false;
    }

    void FileStreamAndroid::Init(Path, EFileMode)
    {
    }

    void FileStreamAndroid::Init()
    {
        FileStream::Init();
    }

    uint32 FileStreamAndroid::Read(void*, uint16, uint32)
    {
        return 0;
    }

    uint32 FileStreamAndroid::Write(void*, uint16, uint32)
    {
        return 0;
    }

    uint32 FileStreamAndroid::Size()
    {
        return 0;
    }

    uint32 FileStreamAndroid::Tell()
    {
        return 0;
    }

    void FileStreamAndroid::Seek(EStreamSeekOrigin, int32)
    {
    }
}
#endif

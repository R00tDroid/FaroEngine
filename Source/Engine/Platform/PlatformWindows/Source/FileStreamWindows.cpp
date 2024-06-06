#if FARO_OS_WINDOWS
#include "FileStreamWindows.hpp"
#include "PlatformWindowsLog.hpp"

namespace Faro
{
    uint32 FileStreamWindows::Read(void* destination, uint16 elementSize, uint32 elementCount)
    {
        return fread(destination, elementSize, elementCount, stream);
    }

    uint32 FileStreamWindows::Write(void* source, uint16 elementSize, uint32 elementCount)
    {
        uint32 written = fwrite(source, elementSize, elementCount, stream);
        size += written;
        return written;
    }

    uint32 FileStreamWindows::Size()
    {
        return size;
    }

    uint32 FileStreamWindows::Tell()
    {
        return ftell(stream);
    }

    void FileStreamWindows::Seek(EStreamSeekOrigin inOrigin, int32 offset)
    {
        int origin = 0;

        switch (inOrigin)
        {
            case SSO_Start: { origin = SEEK_SET; break; }
            case SSO_End: { origin = SEEK_END; break; }
            case SSO_Current: { origin = SEEK_CUR; break; }
        }

        fseek(stream, offset, origin);
    }

    bool FileStreamWindows::IsOpen()
    {
        return stream != nullptr;
    }

    void FileStreamWindows::Init(Path inFile, EFileMode inMode)
    {
        file = inFile;
        mode = inMode;
        Init();
    }

    void FileStreamWindows::Init()
    {
        FileStream::Init();

        String fileMode = "";
        switch (mode)
        {
            case FO_Overwrite: { fileMode = "wb"; break; }
            case FO_Append: { fileMode = "ab"; break; }
            case FO_Read: { fileMode = "rb"; break; }
        }

        fopen_s(&stream, file.Get().Data(), fileMode.Data());

        if (IsOpen())
        {
            Seek(SSO_End, 0);
            size = Tell();
            Seek(SSO_Start, 0);
        }
        else
        {
            PlatformWindowsLog.Log(LC_Error, "Failed to open: %s", file.Get().Data());
        }
    }

    void FileStreamWindows::Destroy()
    {
        if (stream != nullptr)
        {
            fclose(stream);
            stream = nullptr;
        }
        FileStream::Destroy();
    }
}
#endif

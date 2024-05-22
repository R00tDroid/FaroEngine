#include "Stream.hpp"
#include "Memory/MemoryManager.hpp"

namespace Faro
{
    String DataStream::ReadToString()
    {
        Array<char> string = ReadToArray<char>();
        return String(string);
    }

    void DataStream::Close()
    {
        Debug_AssertMessage(copies == 0, "Closing DataStream that has open copies");
        Destroy();
    }

    DataStreamCopy* DataStream::OpenCopy()
    {
        DataStreamCopy* copy = MemoryManager::New<DataStreamCopy>();
        copy->Init(this);
        return copy;
    }

    void DataStream::Destroy()
    {
        IObject::Destroy();
    }

    void DataStream::AddCopy(DataStream* otherStream)
    {
        otherStream->copies++;
    }

    void DataStream::RemoveCopy(DataStream* otherStream)
    {
        Debug_AssertMessage(otherStream->copies > 0, "Mismatch between adding and removing copies");
        otherStream->copies--;
    }

    uint32 DataStreamCopy::Read(void* destination, uint16 elementSize, uint32 elementCount)
    {
        stream->Seek(SSO_Start, cursor);
        uint32 size = stream->Read(destination, elementSize, elementCount);
        cursor += size;
        return size;
    }

    uint32 DataStreamCopy::Write(void* source, uint16 elementSize, uint32 elementCount)
    {
        stream->Seek(SSO_Start, cursor);
        uint32 size = stream->Write(source, elementSize, elementCount);
        cursor += size;
        return size;
    }

    uint32 DataStreamCopy::Size()
    {
        return stream->Size();
    }

    uint32 DataStreamCopy::Tell()
    {
        return cursor;
    }

    void DataStreamCopy::Seek(EStreamSeekOrigin origin, int32 offset)
    {
        switch (origin)
        {
            case SSO_Start: { cursor = offset; break; }
            case SSO_End: { cursor = Size() + offset; break; }
            case SSO_Current: { cursor += offset; break; }
        }
    }

    void DataStreamCopy::Init(DataStream* inStream)
    {
        stream = inStream;
    }

    void DataStreamCopy::Init()
    {
        AddCopy(stream);
        DataStream::Init();
    }

    void DataStreamCopy::Destroy()
    {
        RemoveCopy(stream);
        DataStream::Destroy();
    }
}

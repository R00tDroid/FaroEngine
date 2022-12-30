#pragma once
#include <Primitives.hpp>
#include <Memory/Object.hpp>

namespace Faro
{
    /// @brief Origin point of a seek operation.
    enum EStreamSeekOrigin
    {
        SSO_Start, ///< Seek relative to the beginning of a stream.
        SSO_End, ///< Seek relative to the end of a stream.
        SSO_Current ///< Seek relative to the current position.
    };

    /// @brief Interface that reads and writes data to and from a stream.
    class DataStream : public IObject
    {
    public:
        /**
         * @brief Read data from the stream.
         * @param destination Destination memory. This must be a valid pointer.
         * @param elementSize Size of a single element
         * @param elementCount Amount of elements to read
         * @return uint32 Amount of bytes read
         */
        virtual uint32 Read(void* destination, uint16 elementSize, uint32 elementCount) = 0;

        /**
         * @brief Write data to the stream.
         * @param destination Source memory
         * @param elementSize Size of a single element
         * @param elementCount Amount of elements to write
         * @return uint32 Amount of bytes written
         */
        virtual uint32 Write(void* source, uint16 elementSize, uint32 elementCount) = 0;

        /**
         * @brief Get the byte-size of this stream.
         * @return uint32 Amount of bytes in this stream
         */
        virtual uint32 Size() = 0;

        /**
         * @brief Get the current position of the stream.
         * @return uint32 Current position
         */
        virtual uint32 Tell() = 0;

        /**
         * @brief Move the position relative to the specified origin.
         * @param origin Start point of the seeking operation
         * @param offset Amount of bytes to seek relative to the origin 
         */
        virtual void Seek(EStreamSeekOrigin origin, int32 offset) = 0;

        /**
         * @brief Read data from the stream. 
         * @tparam T Element type to read
         * @param destination Destination memory. This must be a valid pointer.
         * @param elementCount Amount of elements to read
         * @return uint32 Amount of elements read
         */
        template<typename T>
        uint32 Read(T* destination, uint32 elementCount)
        {
            return Read(destination, sizeof(T), elementCount) / sizeof(T);
        }

        /**
         * @brief Read en element from the stream.
         * @tparam T Element type to read
         * @return T Element that was read
         */
        template<typename T>
        T Read()
        {
            T returnValue;
            Read<T>(&returnValue, 1);
            return returnValue;
        }

        /**
         * @brief Write data to the stream.
         * @tparam T Element type to write
         * @param source Source memory
         * @param elementCount Amount of elements to write
         * @return uint32 Amount of elements written
         */
        template<typename T>
        uint32 Write(T* source, uint32 elementCount)
        {
            return Write(source, sizeof(T), elementCount) / sizeof(T);
        }

        /**
         * @brief Write an element to the stream
         * @tparam T Element type to write
         * @param value Element to write
         */
        template<typename T>
        void Write(T value)
        {
            Write<T>(&value, 1);
        }

        /// @brief Close the stream. This also releases it from memory.
        void Close() { Destroy(); }

    protected:
        void Destroy() override { IObject::Destroy(); }
    };
}

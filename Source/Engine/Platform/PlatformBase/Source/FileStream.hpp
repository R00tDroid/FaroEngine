#pragma once
#include <Containers/Stream.hpp>

namespace Faro
{
    /// @brief Mode in which to open a file
    enum EFileMode
    {
        FO_Overwrite, ///< Open a file for writing. Will overwrite any existing data.
        FO_Append, ///< Open a file for writing. Will append to any existing data.
        FO_Read ///< Open a file for reading.
    };

    class FileStream : public DataStream
    {
    public:
        /**
         * @brief Check if the file stream was successfully opened.
         * @return true The stream is open
         * @return false The stream is not open
         */
        virtual bool IsOpen() = 0;
    };
}

#include "Utility.hpp"

bool Utility::ReadEnvVariable(std::string variableName, std::string& value)
{
#ifdef WIN32
    size_t size = 0;
    if (getenv_s(&size, nullptr, 0, variableName.c_str()) != 0)
    {
        return false;
    }
    value.resize(size);
    if (getenv_s(&size, value.data(), size, variableName.c_str()) != 0)
    {
        return false;
    }
#else
    value = std::string(getenv(variableName.c_str()));
#endif

    // Remove null-terminator at end of string
    if (value.back() == '\0')
    {
        value.pop_back();
    }

    return true;
}

#pragma once
#include <string>

class Console
{
public:
    static void ExecuteCommand(std::wstring command);

private:
    static void Clear();
    static void Append(std::wstring string);

};

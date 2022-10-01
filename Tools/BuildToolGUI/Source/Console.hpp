#pragma once
#include <filesystem>
#include <string>

class Console
{
public:
    static void Init();

    static void ExecuteCommand(std::wstring command);

private:
    static void InvokeCommand(std::wstring command);

    static void Clear();
    static void Append(std::wstring string);
    static void AppendLine(std::wstring string);

    static std::filesystem::path buildTool;
};

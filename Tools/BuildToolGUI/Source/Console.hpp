#pragma once
#include <filesystem>
#include <string>

class Console
{
public:
    static void Init();

    static void ExecuteCommand(std::wstring command, std::wstring platform, std::wstring config, std::wstring project);

private:
    static void InvokeCommand(std::wstring command);

    static void Clear();
    static void Append(std::wstring string);
    static void AppendLine(std::wstring string);

    static void LockConsole(bool lock);

    static std::filesystem::path buildTool;
};

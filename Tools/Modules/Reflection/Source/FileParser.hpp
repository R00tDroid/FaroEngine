#pragma once
#include <string>
#include <vector>

enum ReflectedType
{
    RT_Class,
    RT_Struct,
    RT_Variable,
    RT_Function,
};

struct ReflectedEntry
{
    ReflectedEntry(ReflectedType type) : type(type) {}
    virtual ~ReflectedEntry() = default;

    ReflectedType type;
    std::string name;
};

struct ReflectedClass : ReflectedEntry
{
    ReflectedClass(std::vector<std::string> baseClasses) : ReflectedEntry(RT_Class), baseClasses(baseClasses) {}
    ~ReflectedClass() override { for (auto* entry : members) delete entry; }

    std::vector<ReflectedEntry*> members;
    std::vector<std::string> baseClasses;
};

struct ReflectedStruct : ReflectedEntry
{
    ReflectedStruct(std::vector<std::string> baseStructs) : ReflectedEntry(RT_Struct), baseStructs(baseStructs) {}
    ~ReflectedStruct() override { for (auto* entry : members) delete entry; }

    std::vector<ReflectedEntry*> members;
    std::vector<std::string> baseStructs;
};

struct ReflectedVariable : ReflectedEntry
{
    ReflectedVariable() : ReflectedEntry(RT_Variable) {}
    std::string valueType;
};

struct ReflectedFunction : ReflectedEntry
{
    ReflectedFunction() : ReflectedEntry(RT_Function) {}
    std::string returnType;
    std::vector<std::string> arguments;
};

class FileReflector
{
public:
    FileReflector();
    ~FileReflector();

    bool reflect(const std::string& file, std::vector<ReflectedEntry*>& entries);

private:
    struct Implementation;
    Implementation* impl = nullptr;
};

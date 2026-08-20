#pragma once
#include <vector>

enum ReflectionTypeId
{
    Class,
    ClassVariable,
    ClassFunction,
};

class ReflectedType {
protected:
    ReflectedType(const char* name, ReflectionTypeId typeId): name(name), typeId(typeId) {}

private:
    const char* name;
    ReflectionTypeId typeId;

public:
    const char* GetName() const
    {
        return name;
    }

    ReflectionTypeId GetTypeId() const
    {
        return typeId;
    }
};

class ReflectedClassMember : public ReflectedType {
public:
    ReflectedClassMember(const char* name, ReflectionTypeId typeId): ReflectedType(name, typeId) {}
};

class ReflectedClassMemberVariable : public ReflectedClassMember {
public:
    ReflectedClassMemberVariable(const char* name): ReflectedClassMember(name, ClassVariable) {}
};

class ReflectedClassMemberFunction : public ReflectedClassMember {
public:
    ReflectedClassMemberFunction(const char* name): ReflectedClassMember(name, ClassFunction) {}
};

class ReflectedClass : public ReflectedType {
public:
    ReflectedClass(const char* name, std::vector<const char*> baseTypes, std::vector<ReflectedClassMember*> members) : ReflectedType(name, Class), baseTypes(baseTypes) {}

    std::vector<const char*> baseTypes;
};

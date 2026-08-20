#pragma once
#include <vector>

class ReflectedType {
protected:
    ReflectedType(const char* name): name(name) {}

private:
    const char* name;

public:
    const char* GetName() const
    {
        return name;
    }
};

class ReflectedClassMember : public ReflectedType {
public:
    ReflectedClassMember(const char* name): ReflectedType(name) {}
};

class ReflectedClassMemberVariable : public ReflectedClassMember {
public:
    ReflectedClassMemberVariable(const char* name): ReflectedClassMember(name) {}
};

class ReflectedClassMemberFunction : public ReflectedClassMember {
public:
    ReflectedClassMemberFunction(const char* name): ReflectedClassMember(name) {}
};

class ReflectedClass : public ReflectedType {
public:
    ReflectedClass(const char* name, std::vector<const char*> baseTypes, std::vector<ReflectedClassMember*> members) : ReflectedType(name), baseTypes(baseTypes) {}

    std::vector<const char*> baseTypes;
};

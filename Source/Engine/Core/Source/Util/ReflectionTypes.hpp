#pragma once
#include <vector>

class ReflectedType {

};

class ReflectedClassMember : public ReflectedType {

};

class ReflectedClassMemberVariable : public ReflectedType {
public:
    ReflectedClassMemberVariable(char* name) {}
};

class ReflectedClassMemberFunction : public ReflectedType {
public:
    ReflectedClassMemberFunction(char* name) {}
};

class ReflectedClass : public ReflectedType {
public:
    ReflectedClass(char* name, std::vector<ReflectedClassMember*> members) {}
};

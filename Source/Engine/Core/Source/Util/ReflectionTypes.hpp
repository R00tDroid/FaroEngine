#pragma once
#include <vector>

class ReflectedType { 
    
};

class ReflectedClassMember : public ReflectedType {

};

class ReflectedClassMemberVariable : public ReflectedClassMember {
public:
    ReflectedClassMemberVariable(char* name) {}
};

class ReflectedClassMemberFunction : public ReflectedClassMember {
public:
    ReflectedClassMemberFunction(char* name) {}
};

class ReflectedClass : public ReflectedType { //TODO Implement member registration
public:
    ReflectedClass(char* name, std::vector<ReflectedClassMember*> members) {}
};

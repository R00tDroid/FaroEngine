#pragma once

#include "../Containers/Map.h"
#include "../Containers/String.h"
#include <functional>

#define DEFINE_INSTANCE_REGISTRY(NAME, TYPE)    namespace InstanceRegistry_##NAME { static Faro::Map<Faro::String, TYPE*> & Get() { static Faro::Map<Faro::String, TYPE*> instances; return instances; } } extern void InstanceRegistrationFunction_##NAME(Faro::String, TYPE*); template<class T> struct InstanceRegistration_##NAME { T instance; InstanceRegistration_##NAME(Faro::String id){ InstanceRegistrationFunction_##NAME(id, &instance); } };
#define IMPLEMENT_INSTANCE_REGISTRY(NAME, TYPE)    void InstanceRegistrationFunction_##NAME(Faro::String name, TYPE* ptr){ InstanceRegistry_##NAME::Get().Add(name, ptr); }
#define REGISTER_INSTANCE(NAME, CLASS)            namespace { InstanceRegistration_##NAME<CLASS> NAME##_##CLASS##_instance_reg(#CLASS); };

#define DEFINE_CLASS_REGISTRY(NAME, TYPE)        namespace ClassRegistry_##NAME { static Map<String, std::function<TYPE*(void)>> & Get() { static Map<String,  std::function<TYPE*(void)>> instances; return instances; } static TYPE* Instantiate(String id) { if(!Get().Contains(id)) return nullptr; else return Get()[id](); } } template<class T> struct ClassRegistration_##NAME { ClassRegistration_##NAME(String id){ ClassRegistry_##NAME::Get().Add(id, [](){ return new T(); }); } };
#define REGISTER_CLASS(NAME, CLASS)                namespace { ClassRegistration_##NAME<CLASS> NAME##_##CLASS##_class_reg(#CLASS); };

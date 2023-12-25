#pragma once
#include "../Containers/String.hpp"
#include <typeinfo>
#include <Memory/MemoryManager.hpp>

namespace Faro
{
    struct ClassReflection;
    typedef std::type_info TypeId;

    template <class Type>
    const TypeId& GetTypeId()
    {
        return typeid(Type);
    }

    class ReflectionManager
    {
    public:
        static ReflectionManager& Get();

        void Register(ClassReflection* instancePointer)
        {
            classes.Add(instancePointer);
        }

        const Array<ClassReflection*>& GetClasses()
        {
            return classes;
        }

    private:
        ReflectionManager() { }
        Array<ClassReflection*> classes;
    };

    class ClassReflection
    {
    public:
        ClassReflection(const TypeId& typeId):
            typeId(typeId),
            typeName(typeId.name())
        {}  

        String typeName;
        const TypeId& typeId;

        template<class T>
        T* Construct(...)
        {
            va_list arguments;
            va_start(arguments, b);
            T* instance = (T*)ConstructInstance(arguments);
            va_end(arguments);
            return instance;
        }

    private:
        virtual void* ConstructInstance(va_list arguments) = 0;
    };

    template <class T>
    class TypedClassReflection : public ClassReflection
    {
    public:
        TypedClassReflection() : ClassReflection(GetTypeId<T>())
        {
            ReflectionManager::Get().Register(this);
        }

        void* ConstructInstance(va_list arguments) override
        {
            return MemoryManager::NewArgList<T>(arguments);
        }
    };
}

/*#define DEFINE_INSTANCE_REGISTRY(NAME, TYPE) namespace InstanceRegistry_##NAME { static Faro::Map<Faro::String, TYPE*> & Get() { static Faro::Map<Faro::String, TYPE*> instances; return instances; } } extern void InstanceRegistrationFunction_##NAME(Faro::String, TYPE*); template<class T> struct InstanceRegistration_##NAME { T instance; InstanceRegistration_##NAME(Faro::String id){ InstanceRegistrationFunction_##NAME(id, &instance); } };
#define IMPLEMENT_INSTANCE_REGISTRY(NAME, TYPE) void InstanceRegistrationFunction_##NAME(Faro::String name, TYPE* ptr){ InstanceRegistry_##NAME::Get().Add(name, ptr); }
#define REGISTER_INSTANCE(NAME, CLASS) namespace { volatile InstanceRegistration_##NAME<CLASS> NAME##_##CLASS##_instance_reg(#CLASS); };

#define DEFINE_CLASS_REGISTRY(NAME, TYPE) namespace ClassRegistry_##NAME { static Map<String, std::function<TYPE*(void)>> & Get() { static Map<String,  std::function<TYPE*(void)>> instances; return instances; } static TYPE* Instantiate(String id) { if(!Get().Contains(id)) return nullptr; else return Get()[id](); } } template<class T> struct ClassRegistration_##NAME { ClassRegistration_##NAME(String id){ ClassRegistry_##NAME::Get().Add(id, [](){ return new T(); }); } };*/
#define REGISTER_CLASS(CLASS) namespace { TypedClassReflection<CLASS> Reflection_##CLASS; }

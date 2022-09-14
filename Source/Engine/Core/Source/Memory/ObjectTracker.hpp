#ifndef OBJECTTRACKER_HEADER
#define OBJECTTRACKER_HEADER
#include "../Containers/String.h"
#include "../Containers/Map.h"
#include "../Math/MathUtil.h"

namespace Faro
{
    class LiveObjectTracker
    {
    public:
        template<class T> 
        void AddCount()
        {
            std::string class_name = typeid(T).name();
            if(!objects.Contains(class_name))
            {
                objects.Add(class_name, 0);
            }

            objects[class_name]++;
        }

        template<class T>
        void RemoveCount()
        {
            std::string class_name = typeid(T).name();
            if (objects.Contains(class_name))
            {
                objects[class_name]--;
                if (objects[class_name] == 0) objects.Remove(class_name);
            }
        }

        inline String Report()
        {
            Array<String> names;
            uint16 widest = 0;

            String out;
            for(String& name : objects.GetKeys())
            {
                if (objects[name] != 0)
                {
                    names.Add(name);
                    widest = Max(widest, (uint16)name.Length());
                }
            }

            for (String& name : names)
            {
                String out_name = name;
                out_name.FillToWidth(widest + 2);
                out += out_name + " " + ToString(objects[name]) + "\n";
            }

            return out;
        }

    private:
        Map<String, uint32> objects;
    };
}

#endif
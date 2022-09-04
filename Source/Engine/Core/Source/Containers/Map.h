#ifndef MAP_HEADER
#define MAP_HEADER
#include <map>
#include "Array.h"

namespace Faro{

    template<class KeyType, class ValueType>
    class Map
    {
    public:
        void Add(KeyType k, ValueType v)
        {
            data.insert(std::pair<KeyType, ValueType>(k, v));
        }

        void Remove(KeyType k)
        {
            auto it = data.find(k);
            if (it != data.end()) data.erase(k);
        }

        bool Contains(KeyType k)
        {
            return data.find(k) != data.end();
        }

        void Clear()
        {
            data.clear();
        }

        Array<KeyType> GetKeys()
        {
            Array<KeyType> keys;

            for (auto it = data.begin(); it != data.end(); ++it)
            {
                keys.Add(it->first);
            }

            return keys;
        }

        Array<ValueType> GetValues()
        {
            Array<ValueType> values;

            for (auto it = data.begin(); it != data.end(); ++it)
            {
                values.Add(it->second);
            }
            
            return values;
        }

        uint32 Size()
        {
            return data.size();
        }

        ValueType& operator [](KeyType k)
        {
            return data[k];
        }

    private:
        std::map<KeyType, ValueType> data;
    };
}

#endif
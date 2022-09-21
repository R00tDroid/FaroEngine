#pragma once
#include <map>
#include "Array.hpp"

namespace Faro
{
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

        bool Contains(KeyType k) const
        {
            return data.find(k) != data.end();
        }

        void Clear()
        {
            data.clear();
        }

        Array<KeyType> GetKeys() const
        {
            Array<KeyType> keys;

            for (auto it = data.begin(); it != data.end(); ++it)
            {
                keys.Add(it->first);
            }

            return keys;
        }

        Array<ValueType> GetValues() const
        {
            Array<ValueType> values;

            for (auto it = data.begin(); it != data.end(); ++it)
            {
                values.Add(it->second);
            }
            
            return values;
        }

        uint32 Size() const
        {
            return data.size();
        }

        ValueType& operator [](KeyType k)
        {
            return data[k];
        }

        typedef typename std::map<KeyType, ValueType>::iterator Iterator;
        typedef typename std::map<KeyType, ValueType>::const_iterator CIterator;

        Iterator begin() { return data.begin(); }
        CIterator cbegin() const { return data.cbegin(); }
        Iterator end() noexcept { return data.end(); }
        CIterator cend() const { return data.cend(); }

    private:
        std::map<KeyType, ValueType> data;
    };
}

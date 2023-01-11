#pragma once
#include "../Primitives.hpp"
#include <vector>
#include <algorithm>
#include <functional>

namespace Faro 
{
    template<class T>
    class Array
    {
        typedef typename std::vector<T>::iterator iterator;
        typedef typename std::vector<T>::const_iterator const_iterator;

    public:
        Array(){}

        Array(std::initializer_list<T> list)
        {
            data = list;
        }

        Array(const Array<T>& copy) { data = copy.data; }
        Array(Array<T>& copy) { data = copy.data; }

        void Add(T object) { data.push_back(object); }

        void Add(Array<T>& otherArray) { for (T& value : otherArray) Add(value); }

        void Insert(T object, uint32 index) { data.insert(object, index); }

        void RemoveAt(uint32 index)
        {
            data.erase(begin() + index);
        }

        void RemoveAt(uint32 index, uint32 amount)
        {
            data.erase(begin() + index, amount);
        }

        void Remove(T object)
        {
            int32 id = Find(object);
            if (id != -1) data.erase(data.begin() + id);
        }

        void MoveTo(T object, uint32 index)
        {
            int32 current_position = Find(object);
            if (current_position == -1 || current_position == index) { return; }

            if(current_position - (int32)index > 0)
            {
                std::rotate(begin() + index, begin() + current_position, begin() + current_position + 1);
            }
            else
            {
                std::rotate(begin() + current_position, begin() + current_position + 1, begin() + index + 1);
            }
        }

        bool Contains(T object)
        {
            return std::find(data.begin(), data.end(), object) != data.end();
        }

        int32 Find(T object)
        {
            iterator pos = std::find(data.begin(), data.end(), object);
            if (pos == data.end()) return -1;
            return pos - data.begin();
        }

        uint32 Size() { return data.size(); }

        void Resize(uint16 size) { data.resize(size); }

        bool Empty() { return Size() == 0; }

        bool IsValidIndex(int32 index)
        {
            return index >= 0 && index < Size() - 1;
        }

        void Invert()
        {
            std::reverse(begin(), end());
        }

        Array<T> Reverse()
        {
            Array<T> copy(data);
            copy.Invert();
            return copy;
        }

        template<class NewType>
        Array<NewType> Cast()
        {
            Array<NewType> copy;
            for(T& value : data)
            {
                copy.Add((NewType)value);
            }
            return copy;
        }

        void Clear()
        {
            data.clear();
        }

        void Sort(std::function<bool(T& , T&)> function)
        {
            std::sort(data.begin(), data.end(), function);
        }

        T operator [](int index) const
        {
            return data[index];
        }

        T& operator [](int index)
        {
            return data[index];
        }

        T* Data()
        {
            return &data[0];
        }

        T& First()
        {
            return data.front();
        }

        T& Last()
        {
            return data.back();
        }

        // for loop iteration
        iterator begin() { return data.begin(); }
        iterator end() { return data.end(); }
        const_iterator cbegin() const { return data.cbegin(); }
        const_iterator cend() const { return data.cend(); }

    private:
        std::vector<T> data;
    };

}

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
			data_.insert(std::pair<KeyType, ValueType>(k, v));
		}

		void Remove(KeyType k)
		{
			auto it = data_.find(k);
			if (it != data_.end()) data_.erase(k);
		}

		bool Contains(KeyType k)
		{
			return data_.find(k) != data_.end();
		}

		void Clear()
		{
			data_.clear();
		}

		Array<KeyType> GetKeys()
		{
			Array<KeyType> keys;

			for (auto it = data_.begin(); it != data_.end(); ++it)
			{
				keys.Add(it->first);
			}

			return keys;
		}

		Array<ValueType> GetValues()
		{
			Array<ValueType> values;

			for (auto it = data_.begin(); it != data_.end(); ++it)
			{
				values.Add(it->second);
			}
			
			return values;
		}

		uint32 Size()
		{
			return data_.size();
		}

		ValueType& operator [](KeyType k)
		{
			return data_[k];
		}

	private:
		std::map<KeyType, ValueType> data_;
	};
}

#endif
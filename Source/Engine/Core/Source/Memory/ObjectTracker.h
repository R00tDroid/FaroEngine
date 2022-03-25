#ifndef OBJECTTRACKER_HEADER
#define OBJECTTRACKER_HEADER
#include "../Containers/String.h"
#include "../Containers/Map.h"
#include "../Math/Util.h"

namespace Faro
{
	class LiveObjectTracker
	{
	public:
		template<class T> 
		void AddCount()
		{
			std::string class_name = typeid(T).name();
			if(!objects_.Contains(class_name))
			{
				objects_.Add(class_name, 0);
			}

			objects_[class_name]++;
		}

		template<class T>
		void RemoveCount()
		{
			std::string class_name = typeid(T).name();
			if (objects_.Contains(class_name))
			{
				objects_[class_name]--;
				if (objects_[class_name] == 0) objects_.Remove(class_name);
			}
		}

		inline String Report()
		{
			Array<String> names;
			uint16 widest = 0;

			String out;
			for(String& name : objects_.GetKeys())
			{
				if (objects_[name] != 0)
				{
					names.Add(name);
					widest = Math::Max(widest, (uint16)name.Length());
				}
			}

			for (String& name : names)
			{
				String out_name = name;
				out_name.FillToWidth(widest + 2);
				out += out_name + " " + ToString(objects_[name]) + "\n";
			}

			return out;
		}

	private:
		Map<String, uint32> objects_;
	};
}

#endif
#ifndef STRING_HEADER
#define STRING_HEADER

#include "../Primitives.h"
#include "../Version.h"
#include <string>

namespace Faro {

	class String
	{
		typedef char* iterator;

	public:

		String() { data = ""; }
		String(String& str) { data = str.data; }
		String(const String& str) { data = str.data; }
		String(std::string str) { data = str; }
		String(char* str) { data = str; }
		String(const char* str) { data = str; }

		uint32 Length() { return data.size(); }

		void Resize(uint32 size) { data.resize(size); }

		bool Empty() { return Length() == 0; }

		void Erase(uint32 index, uint32 amount = 1) { data.erase(index, amount); }

		String Sub(uint32 Length)
		{
			return data.substr(0, Length);
		}

		String Sub(uint32 Start, uint32 Length)
		{
			return data.substr(Start, Length);
		}

		int16 Find(String sub) { size_t pos = data.find(*sub); return pos == std::string::npos ? -1 : pos; }

		void FillToWidth(uint32 width, char fill = ' ')
		{
			if (Length() == width) return;

			if (Length() < width)
			{
				while (Length() < width) data += fill;
			}
			else 
			{
				data = data.substr(0, width);
			}
		}

		void Replace(String search, String replace)
		{
			size_t pos = 0;
			while ((pos = data.find(search.data, pos)) != std::string::npos) 
			{
				data.replace(pos, search.Length(), replace.data);
				pos += replace.Length();
			}
		}

		int32 FindLastOf(String find)
		{
			size_t pos = data.find_last_of(*find);

			if (pos == std::string::npos) return -1;

			return pos;
		}

		char operator [](int index) const
		{
			return data[index];
		}

		char& operator [](int index)
		{
			return data[index];
		}

		void operator += (String other)
		{
			data += other.data;
		}

		void operator += (char other)
		{
			data += other;
		}

		bool Equals(String other)
		{
			return data == other.data;
		}

		bool operator <(const String& other) const
		{
			return data < other.data;
		}

		// for loop iteration
		iterator begin() { return &data[0]; }
		iterator end() { return &data[Length()]; }

		const char* Data() { return data.c_str(); }

		const char* operator * () { return data.c_str(); }

	private:
		std::string data = "";
	};

	inline String operator +(String a, String b)
	{
		String res(a);
		res += b;
		return res;
	}

	inline bool operator == (String a, String b)
	{
		return a.Equals(b);
	}

	template<class T>
	inline String ToString(T value)
	{
		return std::to_string(value);
	}

	template<>
	inline String ToString<Version>(Version value)
	{
		return ToString(value.Major) + "." + ToString(value.Minor) + "." + ToString(value.Revision);
	}

	template<class T>
	inline T ParseString(String value)
	{
		return std::stoi(value.Data());
	}

	template<>
	inline float ParseString<float>(String value)
	{
		return std::stof(value.Data());
	}
}
#endif
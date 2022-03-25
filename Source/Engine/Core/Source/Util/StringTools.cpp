#include "StringTools.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <string>
#include <functional>

namespace Faro 
{
	bool MatchPattern(String pattern, String source)
	{
		char* first = const_cast<char*>(*pattern);
		char* second = const_cast<char*>(*source);

		if (*first == '\0' && *second == '\0')
			return true;

		if (*first == '*' && *(first + 1) != '\0' && *second == '\0')
			return false;

		if (*first == '?' || *first == *second)
			return MatchPattern(first + 1, second + 1);

		if (*first == '*')
			return MatchPattern(first + 1, second) || MatchPattern(first, second + 1);
		return false;
	}

	Array<String> ParsePattern(String pattern, String source)
	{
		if(!MatchPattern(pattern, source)) return Array<String>();

		Array<String> out;

		Array<uint16> wildcards;
		for (uint16 i = 0; i < pattern.Length(); i++)
		{
			if (pattern[i] == '?' || pattern[i] == '*') wildcards.Add(i);
		}

		if (wildcards.Size() > 0) 
		{
			Array<String> pattern_sections;
			pattern_sections.Add(pattern.Sub(0, wildcards[0]));

			for (uint16 i = 1; i < wildcards.Size(); i++)
			{
				pattern_sections.Add(pattern.Sub(wildcards[i - 1] + 1, wildcards[i] - wildcards[i - 1] - 1));
			}

			pattern_sections.Add(pattern.Sub(wildcards[wildcards.Size() - 1] + 1, pattern.Length() - wildcards[wildcards.Size() - 1] - 1));

			for(String& section : pattern_sections)
			{
				if (section.Empty()) continue;

				uint16 index = source.Find(section);
				out.Add(source.Sub(index));
				source.Erase(0, index + section.Length());
			}

			if (!source.Empty()) out.Add(source);
		}

		return out;
	}

	Array<String> SplitString(String source, char delimiter)
	{
		Array<String> out;

		std::stringstream ss(*source);
		std::string item;
		std::vector<std::string> tokens;
		while (std::getline(ss, item, delimiter)) 
		{
			out.Add(item);
		}

		return out;
	}

	Array<String> SplitString(String source, String delimiter)
	{
		Array<String> out;

		while(true)
		{
			if (source.Empty()) break;

			int16 index = source.Find(delimiter);
			if (index == -1) break;
			if (index > 0) out.Add(source.Sub(0, index));
			source.Erase(0, index + delimiter.Length());
		}

		if (!source.Empty()) out.Add(source);

		return out;
	}

	String ToLowerCase(String str)
	{
		std::string data = str.Data();
		std::transform(data.begin(), data.end(), data.begin(), ::tolower);
		return String(data);
	}

	String ToUpperCase(String str)
	{
		std::string data = str.Data();
		std::transform(data.begin(), data.end(), data.begin(), ::toupper);
		return String(data);
	}
}

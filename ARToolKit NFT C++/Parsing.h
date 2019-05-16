//========================================================//
// Parsing.h                           
//--------------------------------------------------------//
// Functions for parsing strings.                 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// AUTHOR: Glen Straughn                     
// COMPILER: G++ (GCC)                       
// DATE: 03.21.2015                          
//========================================================//

#pragma once

#include<string>
#include<cctype>
#include<locale>

using namespace std;

static string tokenize(string& originString, string& remainder, const string& delimiters, bool returnEmpty = false)
{
	int startIndex = 0;

	if (originString.empty()) // DEFAULT CASE
	{
		remainder = "";
		return "";
	}

	if (!returnEmpty)
	{
		while ((delimiters.find(originString[startIndex]) != string::npos) && (startIndex < originString.length()))
		{
			startIndex++;
		}
	}

	int splitIndex = originString.length();

	for (int i = startIndex; i < originString.length(); i++)
	{
		if (delimiters.find(originString[i]) != string::npos)
		{
			splitIndex = i;
			break;
		}
	}

	string token = originString.substr(startIndex, splitIndex - startIndex);

	if (splitIndex == originString.length())
	{
		remainder = "";
	}
	else
	{
		remainder = originString.substr(splitIndex + 1, originString.length() - 1 - splitIndex);
	}

	return token;
}


//--------------------------------------------------------------------------------//


static bool isNumber(std::string str)
{
	if (str.empty()) // Check if string is empty or a negative number
	{
		return false;
	}

	int neg = 0;
	if (str[0] == '-')
	{
		neg = 1;
	}

	bool decimal = false;

	for (int i = neg; i < str.length(); i++)
	{
		if (!isdigit(str[i]) && (str[i] == '.' && decimal))
		{
			return false;
		}
		else if (!decimal && str[i] == '.')
		{
			decimal = true;
		}
	}

	return true;
}


//--------------------------------------------------------------------------------//


static std::string getFirstRegionBetween(const string &source, const string &start, const string &end, string *remainder = NULL, bool includeEnds = false)
{
	size_t i = source.find(start);
	size_t j = source.substr(i + start.length(), string::npos).find(end) + (i + 1);

	if (i == string::npos || j == string::npos)
	{
		return "";
	}

	if (remainder != NULL)
	{
		*remainder = source.substr(j + end.length(), string::npos);
	}

	if (includeEnds)
	{
		return source.substr(i, j + end.length() - i);
	}
	else
	{
		return source.substr(i + start.length(), j - i - start.length());
	}
}


//--------------------------------------------------------------------------------//


static std::string toLower(const std::string &str)
{
	std::string output(str);

	for (int i = 0; i < output.size(); i++)
	{
		output[i] = tolower(output[i]);
	}

	return output;
}
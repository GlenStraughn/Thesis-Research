//=================================================================//
// StringAndNumberConversion.hpp
//-----------------------------------------------------------------//
// Provides definitions for classses that convert numbers to
// strings and vice-versa.
//-----------------------------------------------------------------//
// AUTHOR: Glen K. Straughn
// DATE: 05.24.2016
//-----------------------------------------------------------------//
// USAGE: This header file is free to use, modify, and redistribute
// free of charge to the recipient. Parties other than the original
// author are not permitted to redistribute this code for profit.
// Altering this comment block would be distasteful.
//=================================================================//

#pragma once

#include <string>
#include <sstream>
#include <iomanip>

template <class type>
static std::string numberToString(type num)
{
	std::stringstream stream;
	std::string s;

	stream << num;
	stream >> s;

	return s;
}

template <class type>
static std::string numberToHexString(type num)
{
	std::stringstream stream;
	std::string s;

	stream << std::hex << num;
	stream >> s;

	return s;
}

//=================================================================//


template <class type>
static type stringToNumber(std::string str)
{
	std::stringstream stream;
	type num;

	stream << str;
	stream >> num;

	return num;
}

template <class type>
static type hexStringToNumber(std::string str)
{
	std::stringstream stream;
	type num;

	stream << str;
	stream >> std::hex >> num;

	return num;
}
// NOTE: Negative numbers wreak havoc on std::hex.
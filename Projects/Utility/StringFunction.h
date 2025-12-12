#pragma once
#include <string>

class StringFunction
{
public:
	/// <summary>
	/// マルチバイト文字列をワイド文字列に変換する
	/// </summary>
	/// <param name="str">マルチバイト文字列</param>
	/// <returns>ワイド文字列</returns>
	static std::wstring WStringFromString(const std::string& str);
};


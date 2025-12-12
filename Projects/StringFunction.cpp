#include "StringFunction.h"
#include "DxLib.h"

std::wstring StringFunction::WStringFromString(const std::string& str)
{
	std::wstring result;

	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), nullptr, 0);

	result.resize(len);

	len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), result.data(), result.size());

    return result;
}

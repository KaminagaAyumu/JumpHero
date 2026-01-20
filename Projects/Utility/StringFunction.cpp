#include "StringFunction.h"
#include "DxLib.h"

std::wstring StringFunction::WStringFromString(const std::string& str)
{
	// 変換して返り値にするワイド文字列
	std::wstring result;
	
	// 受け取った引数の文字列のサイズを取得
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.length()), nullptr, 0);

	// 文字列サイズ分の領域を確保
	result.resize(len);

	// もう一度関数を呼んで変換を実行
	// 受け取りができる引数にワイド文字列のdata()とsize()を使う
	len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.length()), result.data(), static_cast<int>(result.size()));

	// 変換結果を返す
    return result;
}

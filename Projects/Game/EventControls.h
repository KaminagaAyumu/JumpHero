#pragma once
#include <functional>
#include <string>

/// <summary>
/// イベントの内容をまとめた構造体
/// </summary>
struct EventControls
{
	// ヒントを表示する関数
	std::function<void(const std::string& textId)> showHintFunc;

};
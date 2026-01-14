#pragma once
#include <functional>
#include <string>
#include <memory>
#include "../TextManager.h"
#include "../../Utility/Geometry.h"

class UITextWindow;

/// <summary>
/// イベントの内容をまとめた構造体
/// </summary>
struct EventControls
{
	// ヒントを表示する関数
	std::function<void(const std::string& textId)> showHintFunc;

	// テキストウィンドウを表示する関数
	std::function<std::weak_ptr<UITextWindow>(const std::string& id, const std::vector<TextData>& pages, const Size& size, const Position2& pos, float duration)> showTextWindowFunc;

	// アイテムをドロップする関数
	std::function<void(int chestNo, const std::string& itemType)> dropItemFunc;

};
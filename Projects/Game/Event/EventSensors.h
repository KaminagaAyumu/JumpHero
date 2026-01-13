#pragma once
#include <functional>
#include <string>

/// <summary>
/// イベントを起こす条件をまとめた構造体
/// </summary>
struct EventSensors
{
	// プレイヤーが指定されたエリアに入ったかどうかの関数
	std::function<bool(int areaNo)> isEnterAreaFunc;

	// 指定された宝箱を開けたかどうかの関数
	std::function<bool(int chestNo)> isOpenChestFunc;

	// 指定されたアイテムを取得したかどうかの関数
	std::function<bool(const std::string& id)> isGetItemFunc;
};
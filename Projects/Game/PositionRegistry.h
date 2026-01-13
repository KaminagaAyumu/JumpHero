#pragma once
#include "../Utility/Geometry.h"
#include <unordered_map>

/// <summary>
/// 座標情報をレジストリ(キーと値)で管理するクラス
/// </summary>
class PositionRegistry
{
public:


private:
	std::unordered_map<int, Position2> m_areaPos;
	std::unordered_map<int, Position2> m_chestPos;
	std::unordered_map<int, ActivePosition2> m_barrierPos;
	std::unordered_map<int, Position2> m_cameraPos;
	std::vector<Position2> m_spawnPos;

};


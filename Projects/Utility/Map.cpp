#include "Map.h"
#include "Camera.h"
#include "Game.h"
#include <fstream>
#include "DxLib.h"

namespace
{
	// マップのチップを置く数
	constexpr int kChipNumX = 97;
	constexpr int kChipNumY = 16;

	constexpr int kChipSize = 45; // マップ1つの大きさ
	constexpr float kChipScale = 1.0f; // マップの拡大率
	constexpr int kSpaceChipNo = 79; // マップチップの透明部分
	constexpr int kChestChipNo = 46; // マップチップの宝箱部分

	constexpr float kMoveRangeMargin = 1.0f; // マップとの位置の補正用
}

Map::Map() : 
	m_chipData{},
	m_graphChipNumX(0),
	m_graphChipNumY(0)
{
	m_mapHandle = LoadGraph(L"data/mapChip_default.png");

	// マップのデータと画像の切り取り位置を合わせるための処理
	int graphW = 0;
	int graphH = 0;
	GetGraphSize(m_mapHandle, &graphW, &graphH); // 画像のサイズを取得

	// マップの切り取り部分の数を取得
	m_graphChipNumX = graphW / kChipSize;
	m_graphChipNumY = graphH / kChipSize;

	// マップの初期データをロード
	LoadMapdata("data/map_new.csv");
}

Map::~Map()
{
	DeleteGraph(m_mapHandle);
}

void Map::Init()
{

}

void Map::Update()
{

}

void Map::Draw(Camera* camera)
{
	// 画面のスクロール量
	float scrollX = camera->scroll.x;
	float scrollY = camera->scroll.y;

	// 1マスの描画サイズ
	float tileSize = kChipSize * kChipScale;

	// 表示範囲を計算
	int startX = static_cast<int>(scrollX / tileSize);
	int startY = static_cast<int>(scrollY / tileSize);
	int endX = static_cast<int>((scrollX + Game::kScreenWidth) / tileSize) + 1;
	int endY = static_cast<int>((scrollY + Game::kScreenHeight) / tileSize) + 1;

	if (startX < 0) startX = 0;
	if (startY < 0) startY = 0;
	if (endX > m_width) endX = m_width;
	if (endY > m_height) endY = m_height;

	for (int y = startY; y < endY; y++)
	{
		for (int x = startX; x < endX; x++)
		{
			float posX = x * tileSize - scrollX;
			float posY = y * tileSize - scrollY;

			int chipNo = m_chipData[y * m_width + x];

			int srcX = kChipSize * (chipNo % m_graphChipNumX);
			int srcY = kChipSize * (chipNo / m_graphChipNumX);

			DrawRectRotaGraph(static_cast<int>(posX + tileSize * 0.5f), static_cast<int>(posY + tileSize * 0.5f), srcX, srcY, kChipSize, kChipSize, kChipScale, 0.0f, m_mapHandle, true);
		}
	}
}

bool Map::IsCollision(const Rect2D& rect, Rect2D& mapRect)
{
	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			int chipNo = m_chipData[y * m_width + x];
			if (chipNo == kSpaceChipNo)
			{
				continue; // マップチップの透明部分は当たり判定をしないようにする
			}

			// 当たり判定を表示する座標
			// xとyは当たり判定の左端なので中央に補正
			float posX = x * kChipSize * kChipScale + kChipSize * 0.5f;
			float posY = y * kChipSize * kChipScale + kChipSize * 0.5f;

			Rect2D chipRect = { {posX,posY},kChipSize,kChipSize };
			if (chipRect.GetLeft() > rect.GetRight())
			{
				continue;
			}
			if (chipRect.GetRight() < rect.GetLeft())
			{
				continue;
			}
			if (chipRect.GetTop() > rect.GetBottom())
			{
				continue;
			}
			if (chipRect.GetBottom() < rect.GetTop())
			{
				continue;
			}

			// マップの矩形を返す
			mapRect = chipRect;
			return true;
		}
	}
	return false;
}

Rect2D Map::GetCanMoveRange(const Rect2D& rect)
{
	Position2 rectCenter = rect.pos;
	// 返す矩形の上下左右の座標
	float retRectLeft = rectCenter.x;
	float retRectRight = rectCenter.x;
	float retRectTop = rectCenter.y;
	float retRectBottom = rectCenter.y;

	float minLeftDist = 100000.0f;
	float minRightDist = 100000.0f;
	float minTopDist = 100000.0f;
	float minBottomDist = 100000.0f;

	// 矩形の上下左右の座標が設定されたかどうか
	bool isLeftSet = false;
	bool isRightSet = false;
	bool isTopSet = false;
	bool isBottomSet = false;
	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			int chipNo = m_chipData[y * m_width + x];
			if (chipNo == kSpaceChipNo)
			{
				continue; // マップチップの透明部分は当たり判定をしないようにする
			}

			// マップの判定の座標を設定
			// xとyは当たり判定の左端なので中央に補正
			float posX = x * kChipSize * kChipScale + kChipSize * 0.5f;
			float posY = y * kChipSize * kChipScale + kChipSize * 0.5f;

			// x軸の距離が近い時(大体縦並びになっているとき)
			if (abs(posX - rectCenter.x) <= (rect.width / 2 + kChipSize / 2))
			{
				float distY = abs(posY - rectCenter.y);
				// マップが矩形より下にある時かつ矩形と一番近い時
				if (posY > rectCenter.y && distY < minBottomDist)
				{
					// 返す矩形の下端の座標をセット
					retRectBottom = posY - kChipSize * 0.5f + kMoveRangeMargin;

					// 矩形との最短距離を設定
					minBottomDist = distY;
				}
				else if (posY < rectCenter.y && distY < minTopDist) // マップが矩形より上にある時
				{
					retRectTop = posY + kChipSize * 0.5f - kMoveRangeMargin;

					minTopDist = distY;
				}
			}

			// y軸との距離が近いとき(大体横並びになっているとき)
			if (abs(posY - rectCenter.y) <= (rect.height / 2 + kChipSize / 2))
			{
				float distX = abs(posX - rectCenter.x);
				if (posX > rectCenter.x && distX < minRightDist)
				{
					retRectRight = posX - kChipSize * 0.5f - kMoveRangeMargin;

					minRightDist = distX;
				}
				else if (posX < rectCenter.x && distX < minLeftDist)
				{
					retRectLeft = posX + kChipSize * 0.5f + kMoveRangeMargin;

					minLeftDist = distX;
				}
			}
			if (isLeftSet && isRightSet && isTopSet && isBottomSet)
			{
				break;
			}
		}
	}

	retRectLeft += kMoveRangeMargin;
	retRectRight -= kMoveRangeMargin;
	retRectTop += kMoveRangeMargin;
	retRectBottom -= kMoveRangeMargin;

	Position2 newPos = {
		(retRectLeft + retRectRight) / 2.0f,
		(retRectTop + retRectBottom) / 2.0f
	};
	float newWidth = retRectRight - retRectLeft;
	float newHeight = retRectBottom - retRectTop;


	return Rect2D(newPos, newWidth, newHeight);
}

int Map::GetMapChipNum(int x, int y)
{
	return m_chipData[y * m_width + x];
}

void Map::SetMapChip(int x, int y, int value)
{
	m_chipData[y * m_width + x] = value;
}

void Map::LoadMapdata(const std::string& fileName)
{
	std::ifstream file(fileName);
	std::string line;
	std::vector<std::vector<int>> temp; // 格納用の二次元配列

	while (std::getline(file, line))
	{

		std::istringstream stream(line);
		std::string field;
		std::vector<int> row; // マップの行を格納する用の配列

		while (getline(stream, field, ','))
		{
			row.push_back(std::stoi(field)); // 行に要素を格納
		}
		temp.push_back(row); // 列に要素を格納
	}

	m_height = static_cast<int>(temp.size()); // 列の要素数を取得
	m_width = static_cast<int>(temp[0].size()); // 行の要素数を取得
	m_chipData.resize(m_width * m_height); // 実際に格納するメンバ変数のサイズを取得

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			m_chipData[y * m_width + x] = temp[y][x];
		}
	}
}

#pragma once
#include <sstream>
#include <memory>
#include <vector>
#include "Geometry.h"

class Camera;

/// <summary>
/// マップを管理するクラス
/// </summary>
class Map
{
public:
	Map();
	virtual ~Map();

	void Init();
	void Update();
	/// <summary>
	/// マップを描画する
	/// </summary>
	/// <param name="">スクロール用カメラの情報</param>
	void Draw(Camera*);

	/// <summary>
	/// 矩形とマップとの当たり判定
	/// </summary>
	/// <param name="rect">対象の矩形</param>
	/// <param name="mapRect">マップの矩形</param>
	/// <returns>true: 当たっている false: 当たっていない</returns>
	bool IsCollision(const Rect2D& rect, Rect2D& mapRect);

	/// <summary>
	/// 対象の矩形の行動可能範囲を返す
	/// </summary>
	/// <param name="rect">対象の矩形</param>
	/// <returns>行動可能範囲</returns>
	Rect2D GetCanMoveRange(const Rect2D& rect);

	// ゲッター
	int GetGraphChipNumX() const { return m_graphChipNumX; }
	int GetGraphChipNumY() const { return m_graphChipNumY; }

	int GetMapWidth() const { return m_width; }
	int GetMapHeight() const { return m_height; }
	Size GetMapSize() const;
	int GetMapChipNum(int x, int y);
	int GetChestPosToMap(int x, int y);

	/// <summary>
	/// マップチップの指定の座標のデータを変更する
	/// </summary>
	/// <param name="x">マップのX座標</param>
	/// <param name="y">マップのY座標</param>
	/// <param name="value">変更後のマップチップ番号</param>
	void SetMapChip(int x, int y, int value);

private:

	int m_mapHandle; // マップの画像ハンドル

	int m_width; // マップの横幅(マップチップ数)
	int m_height; // マップの縦幅(マップチップ数)
	int m_layerCount; // マップレイヤーの数
	std::vector<int> m_chipData; // マップチップのデータを格納する配列
	std::vector<std::vector<int>> m_layerMapData; // レイヤーごとのデータを格納する配列

	// マップチップを描画する画像を区切った縦横の数
	int m_graphChipNumX;
	int m_graphChipNumY;

private:
	// メンバ関数

	/// <summary>
	/// マップデータを読み込む(マップ番号を配列に格納)
	/// </summary>
	void LoadMapdata(const std::string& fileName);

	/// <summary>
	/// ステージ別のデータを読み込む
	/// </summary>
	/// <param name="stageNo">ステージ番号</param>
	/// <returns>true : 読み込み成功 false : 読み込みに失敗</returns>
	bool LoadStageData(int stageNo);

	/// <summary>
	/// ゲーム内座標をマップ基準の座標に変換
	/// </summary>
	/// <param name="pos">変換したい座標</param>
	/// <param name="tileSize">マップチップ1枚の大きさ</param>
	/// <returns>マップチップ座標</returns>
	int WorldPosToMapPos(float pos, float tileSize);

};


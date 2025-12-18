#pragma once
#include "Geometry.h"

/// <summary>
/// ターゲットを基準にして描画位置を設定するカメラクラス
/// </summary>
class Camera
{
public:
	Camera(const Size& size);
	virtual ~Camera();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="size">マップのサイズ</param>
	void Init(const Size& size);
	/// <summary>
	/// ターゲットの座標を基準にして画面のスクロール量を計算する処理
	/// </summary>
	void Update();

	Vector2 scroll; // 画面上のオブジェクトのスクロール量

	/// <summary>
	/// カメラが見るターゲットをセットする
	/// </summary>
	/// <param name="pos">ターゲットの座標のポインタ</param>
	void SetTarget(const Position2* pos) { m_pTargetPos = pos; }

	/// <summary>
	/// 現在の画面の表示範囲を取得
	/// 画面外処理などに使用
	/// </summary>
	/// <returns>画面の表示範囲</returns>
	const Rect2D& GetNowScreenArea()const;

	/// <summary>
	/// カメラの補正が終わったかどうか
	/// </summary>
	/// <returns>true : 終わった false : 終わっていない</returns>
	bool IsLeapEnd()const;

private:
	Position2 m_pos; // カメラの座標
	Size m_mapSize; // マップのサイズ
	const Position2* m_pTargetPos; // カメラが見るターゲットの座標

};


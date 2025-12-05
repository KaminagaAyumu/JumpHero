#pragma once
#include "Geometry.h"

class Actor;

/// <summary>
/// ターゲットを基準にして描画位置を設定するカメラクラス
/// </summary>
class Camera
{
public:
	Camera(const Size& size);
	virtual ~Camera();

	/// <summary>
	/// ターゲットの座標を基準にして画面のスクロール量を計算する処理
	/// </summary>
	void Update();

	Vector2 scroll; // 画面上のオブジェクトのスクロール量

	/// <summary>
	/// カメラが見るターゲットをセットする
	/// </summary>
	/// <param name="actor">ターゲットのポインタ</param>
	void SetTarget(Actor* actor) { m_pTargetActor = actor; }

	/// <summary>
	/// 現在の画面の表示範囲を取得
	/// 画面外処理などに使用
	/// </summary>
	/// <returns>画面の表示範囲</returns>
	const Rect2D& GetNowScreenArea()const;

private:
	Position2 m_pos; // カメラの座標
	Size m_mapSize; // マップのサイズ
	Actor* m_pTargetActor; // カメラが見るターゲットオブジェクト

};


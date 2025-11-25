#pragma once
#include "Geometry.h"

class Actor;

class Camera
{
public:
	Camera();
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

private:
	Position2 m_pos; // カメラの座標
	Actor* m_pTargetActor; // カメラが見るターゲットオブジェクト

};


#pragma once

class Actor;

// オブジェクト(Actor)の区別をつける列挙体など
namespace Types
{
	/// <summary>
	/// オブジェクトが何なのかを説明するための列挙体
	/// </summary>
	enum class ActorType
	{
		Player,
		Enemy,
		Chest,
		Item
	};

	/// <summary>
	/// 当たり判定が何なのかを説明するための列挙体
	/// </summary>
	enum class ColliderType
	{
		Rect,
		Circle
	};

	/// <summary>
	/// 当たり判定の内容を格納する構造体
	/// </summary>
	struct CollisionInfo
	{
		Actor* other; // 当たったオブジェクト
		ActorType otherType; // 当たったオブジェクトの種類
		ColliderType ownColliderType; // 自身の当たり判定のタイプ
		ColliderType otherColliderType; // 当たったオブジェクトの当たり判定のタイプ
	};

	/// <summary>
	/// アイテムの種類
	/// </summary>
	enum class ItemType
	{
		Balloon, // 風船
		UpgradeMedal, // 強化メダル
		LifeUp, // 1UP
		Coin, // スコア加算用コイン
		ChangeToCoin, // 敵をコインに変える
	};
}
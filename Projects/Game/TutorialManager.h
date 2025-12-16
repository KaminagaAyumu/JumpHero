#pragma once
#include <sstream>

class TutorialManager
{
public:
	TutorialManager();
	virtual ~TutorialManager();


private:
	// イベントが始まる条件
	enum class TriggerType
	{
		EnterArea, // エリアに入った
		GetItem, // アイテムを取得した
		EnemySpawn, // 敵がスポーンした
	};

	// イベントで何を行うか
	enum class ActionType
	{
		ShowText, // テキストを表示する
		FreezeGame, // ゲームを一時停止する
		UnfreezeGame, // ゲームを再開する
		PowerUp, // プレイヤーの強化
		WaitInput, // ボタン入力待ち
	};

	/// <summary>
	/// チュートリアルで行われるイベントをまとめる構造体
	/// </summary>
	struct EventData
	{
		int id; // 識別ID
		TriggerType triggerType; // イベントが始まる条件
		std::string triggerParam; // イベントの場合分けで必要なデータ
		ActionType actionType; // 行われるイベント
		std::string actionParam; // イベントで必要なデータ
	};

};


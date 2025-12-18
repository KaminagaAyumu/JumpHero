#pragma once
#include <sstream>
#include <unordered_map>
#include "TextManager.h"
#include "../Utility/Geometry.h"

class Input;
class GameManager;
class TextManager;
class Map;
class Player;

// イベントが始まる条件
enum class TriggerType
{
	GameStart, // ゲームが始まった
	EnterArea, // エリアに入った
	OpenChest, // 宝箱を開けた
	GetItem, // アイテムを取得した
	EnemySpawn, // 敵がスポーンした
};

// イベントで何を行うか
enum class ActionType
{
	ShowText, // テキストを表示する
	DropItem, // アイテムを生成する
	FreezeGame, // ゲームを一時停止する
	UnfreezeGame, // ゲームを再開する
	PowerUp, // プレイヤーの強化
	WaitInput, // ボタン入力待ち
};

class TutorialManager
{
public:
	TutorialManager(GameManager* gameManager, TextManager* textManager, Map* map, Player* player);
	virtual ~TutorialManager();

	void Update(Input& input);
	void Draw() const; 
	void DrawEventData(int id);

	/// <summary>
	/// イベントデータをロードする
	/// </summary>
	/// <returns>true : ロード成功 false : ロード失敗</returns>
	bool LoadEventData();

	size_t GetEventNum();

	bool IsFreezeGame()const { return m_isFreezeGame; };

private:
	

	/// <summary>
	/// チュートリアルで行われるイベントをまとめる構造体
	/// </summary>
	struct EventData
	{
		int id = 0; // 識別ID
		TriggerType triggerType; // イベントが始まる条件
		std::string triggerParam; // イベントの場合分けで必要なデータ
		ActionType actionType; // 行われるイベント
		std::string actionParam; // イベントで必要なデータ
	};

	/// <summary>
	/// テキストデータをページで管理する構造体
	/// </summary>
	struct TextPager
	{
		std::string id; // 識別ID
		std::vector<TextData> pages; // ページごとのテキストデータ
		int index = 0; // ページ数
		bool isActive = false; // ページ送りが進行中か
	};

	int m_eventIndex; // イベントの進行状況

	bool m_isInput; // 入力があるかどうか
	bool m_isFreezeGame; // ゲームを止めるかどうか
	bool m_isShowTextWindow; // テキストウィンドウを表示するかどうか

	std::vector<EventData> m_eventData; // イベントデータ群
	TextPager m_textPager; // テキストを管理する構造体

	GameManager* m_pGameManager;	// ゲームマネージャーのポインタ
	TextManager* m_pTextManager;	// テキストデータのポインタ
	Map* m_pMap;					// マップデータのポインタ
	Player* m_pPlayer;				// プレイヤーのポインタ(座標取得用)

	std::unordered_map<int, Position2> m_areaPos;
	std::unordered_map<int, Position2> m_chestPos;

	bool InitEventPos();

	const Position2* FindAreaPos(int areaId) const;

	bool IsEnterArea(int areaNum);
	int GetAreaNum(std::string param);

	/// <summary>
	/// テキストデータをウィンドウとともに表示する
	/// </summary>
	void DrawTextWindow() const;

	/// <summary>
	/// イベント条件を文字列から変換する用の関数
	/// </summary>
	/// <param name="strData">文字列データ</param>
	/// <returns>変換後のイベント条件</returns>
	TriggerType ToTriggerType(const std::string strData);

	/// <summary>
	/// イベントで行う内容を文字列から変換する用の関数
	/// </summary>
	/// <param name="strData">文字列データ</param>
	/// <returns>変換後のイベント内容</returns>
	ActionType ToActionType(const std::string strData);

	/// <summary>
	/// イベントトリガーを判定
	/// </summary>
	/// <param name="data">イベントデータ</param>
	/// <returns>true : トリガー発火 false : トリガーが発火していない</returns>
	bool CheckTrigger(const EventData& data);

	/// <summary>
	/// 指定されたイベントを行う
	/// </summary>
	/// <param name="data">イベントデータ</param>
	void RunAction(const EventData& data);

};


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
	TimeElapsed, // 時間が経過した
	EventEnd, // イベントが終わった
	OpenChest, // 宝箱を開けた
	GetItem, // アイテムを取得した
	EnemySpawn, // 敵がスポーンした
	PlayerPowerUp, // プレイヤーがパワーアップ状態になった
	NoTrigger, // 条件なし
};

// イベントで何を行うか
enum class ActionType
{
	ShowText, // テキストを表示する
	DropItem, // アイテムを生成する
	SpawnEnemy, // 敵をスポーンさせる
	FreezeGame, // ゲームを一時停止する
	UnFreezeGame, // ゲームを再開する
	FreezePlayer, // プレイヤーを止める
	UnFreezePlayer, // プレイヤーを止める
	LookCamera, // カメラが特定の場所を見る
	ReturnCamera, // カメラが戻ってくる
	PowerUp, // プレイヤーの強化
	WaitInput, // ボタン入力待ち
	UnlockBarrier, // 移動制限バリアを開放
	ActiveGoal, // ゴールを開く
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

	struct CommonEventData : EventData
	{
		bool isOnce; // 一度しか行わないかどうか
		bool isInvoked = false; // 行われたかどうか(onceの際使う)
	};

	int m_timeCount; // 時間経過用フレーム数のカウンタ
	int m_eventIndex; // イベントの進行状況

	bool m_isInput; // 入力があるかどうか
	bool m_isFreezeGame; // ゲームを止めるかどうか
	bool m_isShowTextWindow; // テキストウィンドウを表示するかどうか

	std::vector<EventData> m_eventData; // イベントデータ群
	std::vector<CommonEventData> m_commonEventData; // 共通イベントデータ群
	TextPager m_textPager; // テキストを管理する構造体

	GameManager* m_pGameManager;	// ゲームマネージャーのポインタ
	TextManager* m_pTextManager;	// テキストデータのポインタ
	Map* m_pMap;					// マップデータのポインタ
	Player* m_pPlayer;				// プレイヤーのポインタ(座標取得用)

	std::unordered_map<int, Position2> m_areaPos;
	std::unordered_map<int, Position2> m_barrierPos;
	std::unordered_map<int, Position2> m_chestPos;
	std::vector<Position2> m_spawnPos;
	std::unordered_map<int, Position2> m_cameraPos;

	/// <summary>
	/// 共通イベントデータをロードする
	/// </summary>
	/// <returns>true : ロード成功 false: ロード失敗</returns>
	bool LoadCommonEventData();

	bool InitEventPos();

	const Position2* FindAreaPos(int areaId) const;

	bool IsEnterArea(int areaNum); // イベント発火フラグの場所に到達したか
	bool IsOpenChest(int chestNum); // 指定された宝箱を開けたか
	bool IsGetItem(std::string param); // 指定されたアイテムを取得したか
	int GetParamNum(std::string param); // パラメータの数値を整数型に変換

	void LookCamera(std::string param); // パラメータの座標にカメラを動かす


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


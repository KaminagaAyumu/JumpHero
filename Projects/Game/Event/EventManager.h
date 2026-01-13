#pragma once
#include "EventStructs.h"

using namespace Events;

/// <summary>
/// ゲーム中のイベントを管理するクラス
/// </summary>
class EventManager
{
public:
	EventManager();
	virtual ~EventManager();

	void Update();
	void Draw() const;

	/// <summary>
	/// イベントをセットする
	/// </summary>
	/// <param name="controls"></param>
	/// <param name="sensors"></param>
	void SetEvents(const std::shared_ptr<EventControls>& controls, const std::shared_ptr<EventSensors>& sensors);

private:

	int m_eventIndex; // イベントの進行状況

	std::weak_ptr<EventControls> m_pControls;
	std::weak_ptr<EventSensors> m_pSensors;

	std::vector<EventData> m_eventData; // イベントデータ群
	std::vector<CommonEventData> m_commonEventData; // 共通イベントデータ群

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

	/// <summary>
	/// 指定された共通イベントを行う
	/// </summary>
	/// <param name="data">イベントデータ</param>
	void RunCommonAction(const EventData& data);

};


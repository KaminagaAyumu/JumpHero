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

private:

};


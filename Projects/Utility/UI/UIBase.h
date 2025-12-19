#pragma once

/// <summary>
/// UI関連の基底クラス
/// </summary>
class UIBase
{
public:
	virtual ~UIBase() = default;
	virtual void Update()abstract;
	virtual void Draw()const abstract;
};


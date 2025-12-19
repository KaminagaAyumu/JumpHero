#pragma once
#include "UIBase.h"
class UIText : public UIBase
{
public:
	UIText();
	virtual ~UIText();

	void Update()override;
	void Draw()const override;

};


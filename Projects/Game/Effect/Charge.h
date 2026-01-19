#pragma once
#include "EffectBase.h"
class Charge : public EffectBase
{
public:
	Charge(int handle, const Position2& pos);
	virtual ~Charge();

	void Update()override;
	void Draw(const Camera& camera)override;

private:

};


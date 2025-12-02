#include "ItemBase.h"
#include "DxLib.h"

namespace
{
	constexpr float kItemDefaultWidth = 45.0f;
	constexpr float kItemDefaultHeight = 45.0f;

	constexpr int kAddScore = 1000;
}

ItemBase::ItemBase() : 
	Actor(Types::ActorType::Item),
	m_isExist(true)
{
}

ItemBase::~ItemBase()
{
}

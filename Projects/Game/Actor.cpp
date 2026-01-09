#include "Actor.h"
#include "../Utility/Camera.h"

Actor::Actor(Types::ActorType t) :
	m_pos{},
	m_colCircle{},
	m_colRect{},
	m_type(t),
	m_pCamera(nullptr),
	m_currentAnim{}
{
}

Actor::~Actor()
{
}

void Actor::SetCamera(Camera* camera)
{
	m_pCamera = camera;
}

void Actor::ChangeAnimation(const Animation& anim)
{
	// 現在のアニメーションと違う場合のみ変更を行う
	if (m_currentAnim != anim)
	{
		m_currentAnim = anim;
	}
}




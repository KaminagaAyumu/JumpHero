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




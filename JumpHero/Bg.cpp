#include "Bg.h"
#include <DxLib.h>
#include "Game.h"
#include "Camera.h"

Bg::Bg() :
	m_pos{}
{
	m_bgHandle = LoadGraph(L"data/BlockDestroyer_mainVisual.png");



}

Bg::~Bg()
{
	DeleteGraph(m_bgHandle);
}

void Bg::Init()
{
}

void Bg::Update()
{
}

void Bg::Draw()
{
	DrawGraph(static_cast<int>(m_pos.x), static_cast<int>(m_pos.y), m_bgHandle, true);
}


void Bg::Draw(std::shared_ptr<Camera> camera)
{
	Size bgSize = {};
	GetGraphSize(m_bgHandle, &bgSize.x, &bgSize.y);

	Position2 scrollPos = {};
	scrollPos.x = static_cast<int>(camera->scroll.x) % bgSize.x;
	scrollPos.y = static_cast<int>(camera->scroll.y) / bgSize.y;

	DrawGraph(static_cast<int>(-scrollPos.x), static_cast<int>(-scrollPos.y), m_bgHandle, true);

	if (scrollPos.x > 0)
	{
		DrawGraph(static_cast<int>(bgSize.x) - static_cast<int>(scrollPos.x),
			Game::kScreenHeight - bgSize.y,
			m_bgHandle, true);
	}
}



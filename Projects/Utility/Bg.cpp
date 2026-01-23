#include <vector>
#include <memory>
#include "Bg.h"
#include "Game.h"
#include "Camera.h"
#include "DxLib.h"
#include <cassert>

Bg::Bg() :
	m_pos{}
{
	m_bgHandle = LoadGraph(L"data/BlockDestroyer_mainVisual.png");
	
	auto handle = LoadGraph(L"data/background_1.png");
	assert(handle != -1 && "画像の読み込みに失敗しました");
	m_bgHandles.push_back(handle);
	handle = LoadGraph(L"data/background_2.png");
	assert(handle != -1 && "画像の読み込みに失敗しました");
	m_bgHandles.push_back(handle);
	handle = LoadGraph(L"data/background_3.png");
	assert(handle != -1 && "画像の読み込みに失敗しました");
	m_bgHandles.push_back(handle);
	handle = LoadGraph(L"data/background_4.png");
	assert(handle != -1 && "画像の読み込みに失敗しました");
	m_bgHandles.push_back(handle);
	handle = LoadGraph(L"data/background_5.png");
	assert(handle != -1 && "画像の読み込みに失敗しました");
	m_bgHandles.push_back(handle);
	handle = LoadGraph(L"data/background_6.png");
	assert(handle != -1 && "画像の読み込みに失敗しました");
	m_bgHandles.push_back(handle);
	handle = LoadGraph(L"data/img/bg_underground.png");
	assert(handle != -1 && "画像の読み込みに失敗しました");
	m_bgHandles.push_back(handle);

}

Bg::~Bg()
{
	DeleteGraph(m_bgHandle);
	for (auto handle : m_bgHandles)
	{
		DeleteGraph(handle);
	}
}

void Bg::Init()
{
}

void Bg::Update()
{
}

void Bg::Draw()
{
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x448899, true);
	//DrawGraph(static_cast<int>(m_pos.x), static_cast<int>(m_pos.y), m_bgHandle, true);
}


void Bg::Draw(std::weak_ptr<Camera> camera)
{
	Size bgSize = {};
	GetGraphSize(m_bgHandle, &bgSize.width, &bgSize.height);

	Position2 scrollPos = {};
	auto pCamera = camera.lock();

	// ↓の場合、さらにfloatにキャストしないといけないのでfmodfを使う
	//scrollPos.x = static_cast<int>(camera->scroll.x) % bgSize.width;
	scrollPos.x = fmodf(pCamera->scroll.x, static_cast<float>(bgSize.width));
	// 縦もスクロールする場合は↓を使う
	// scrollPos.y = static_cast<int>(camera->scroll.y) % bgSize.y;
	scrollPos.y = pCamera->scroll.y / bgSize.height;

	DrawGraph(static_cast<int>(-scrollPos.x), static_cast<int>(-scrollPos.y), m_bgHandle, true);

	// 横にのみ表示する時
	/*if (scrollPos.x > 0)
	{
		DrawGraph(static_cast<int>(bgSize.x) - static_cast<int>(scrollPos.x),
			Game::kScreenHeight - bgSize.y,
			m_bgHandle, true);
	}*/

	// 上下左右に表示する時
	/*if(bgSize.x - scrollPos.x < Game::kScreenWidth)
	{
		DrawGraph(static_cast<int>(-scrollPos.x) + static_cast<int>(bgSize.x), 
			static_cast<int>(-scrollPos.y), 
			m_bgHandle, true);
	}

	if(bgSize.y - scrollPos.y < Game::kScreenHeight)
	{
		DrawGraph(static_cast<int>(-scrollPos.x), 
			static_cast<int>(-scrollPos.y) + static_cast<int>(bgSize.y), 
			m_bgHandle, true);
	}

	if(bgSize.x - scrollPos.x < Game::kScreenWidth &&
		bgSize.y - scrollPos.y < Game::kScreenHeight)
	{
		DrawGraph(static_cast<int>(-scrollPos.x) + static_cast<int>(bgSize.x), 
			static_cast<int>(-scrollPos.y) + static_cast<int>(bgSize.y), 
			m_bgHandle, true);
	}*/

	for(int handle : m_bgHandles)
	{
		DrawGraph(static_cast<int>(-scrollPos.x), static_cast<int>(-scrollPos.y), handle, true);
		if (scrollPos.x > 0)
		{
			Size size = {};
			GetGraphSize(handle, &size.width, &size.height);
			DrawGraph(static_cast<int>(size.width) - static_cast<int>(scrollPos.x),
				Game::kScreenHeight - size.height,
				handle, true);
		}
	}
}



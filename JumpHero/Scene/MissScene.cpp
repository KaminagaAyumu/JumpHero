#include "MissScene.h"
#include "SceneController.h"
#include "DxLib.h"
#include <string>

namespace
{
	constexpr int kMaxFrame = 300;
}

MissScene::MissScene(SceneController& controller) : SceneBase(controller)
{
}

MissScene::~MissScene()
{
}

void MissScene::Update(Input& input)
{
	m_frameCount++;

	if (m_frameCount >= kMaxFrame)
	{
		m_controller.PopScene();
		return;
	}
	
}

void MissScene::Draw()
{
	//const int& life = Application::GetInstance().GetLife();

	DrawBox(0, 0, 800, 600, 0x000000, true);
	//DrawFormatString(300, 300, 0xffffff, L"残機: %d", life);

#ifdef _DEBUG
	DrawString(0, 0, L"MissScene: Draw", 0xFFFFFF);
#endif
}

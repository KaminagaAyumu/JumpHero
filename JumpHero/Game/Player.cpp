#include "Player.h"
#include "../Input.h"
#include "../Game.h"
#include "../Map.h"
#include "../Camera.h"
#include <DxLib.h>
#include <cassert>

namespace
{
	constexpr float kGravity = 0.5f;
	constexpr float kGroundY = 570.0f;
	constexpr float kJumpPower = -15.0f;
	constexpr float kMoveSpeed = 3.5f;

	constexpr int kGraphWidth = 45;
	constexpr int kGraphHeight = 45;

	constexpr float kPlayerWidth = 45.0f;
	constexpr float kPlayerHeight = 45.0f;

	constexpr int kMissFreezeTime = 10;
	constexpr int kMissEndTime = 180;
}

Player::Player() : 
	Actor(Types::ActorType::Player),
	m_direction{},
	m_velocity{},
	m_graphHandle(-1),
	m_jumpCount(0),
	m_missCount(0),
	m_isGround(false),
	m_isHover(false),
	m_isMiss(false),
	m_isOffsetX(true),
	m_currentFloorY(0.0f),
	m_pMap(nullptr),
	m_update(&Player::JumpUpdate),
	m_draw(&Player::JumpDraw)
{
#ifdef _DEBUG
	m_maxPosY = m_pos.y; 
	m_lastMaxPos = {};
#endif
}

Player::~Player()
{
	DeleteGraph(m_graphHandle);
}

void Player::Init()
{
	m_pos = {100.0f,300.0f};
	m_direction = {};
	m_velocity = {};
	m_graphHandle = LoadGraph(L"data/Idle.png");
	m_colCircle = { m_pos,kGraphWidth * 0.5f };
	m_colRect = { m_pos,kGraphWidth,kGraphHeight };
	m_jumpCount = 0;
	m_missCount = 0;
	m_isGround = false;
	m_isHover = false;
	m_isMiss = false;
	m_currentFloorY = 0.0f;
	m_update = &Player::JumpUpdate;
	m_draw = &Player::JumpDraw;
}

void Player::Update(Input& input)
{
	(this->*m_update)(input);
}

void Player::Draw()
{ 
	(this->*m_draw)();
}

void Player::IsCollision(const Types::CollisionInfo& info)
{
	// 敵と衝突したらミス処理を行う
	if(info.otherType == Types::ActorType::Enemy && !m_isMiss)
	{
		printfDx(L"Player : 敵と衝突しました\n");
		MissStart();
	}

	// 宝箱と衝突した時
	if (info.otherType == Types::ActorType::Chest)
	{
		printfDx(L"Player : 宝箱と衝突しました\n");
	}
}

void Player::JumpUpdate(Input& input)
{
	m_jumpCount++;
	m_pos.y += m_velocity.y * m_direction.y + kGravity * m_jumpCount * 0.5f;

	//m_velocity.y = kJumpPower;

	if (m_isHover)
	{

	}
	else
	{
		if (input.IsPressed("Up")) // 上ボタンが押されたとき
		{
			m_velocity.y = kJumpPower - 0.5f;
		}
		if (input.IsPressed("Down")) // 下ボタンが押されたとき
		{
			m_velocity.y = kJumpPower + 0.5f;
		}
	}

	if (input.IsTriggered("Jump")) // 再びジャンプボタンが押されたら
	{
		m_isHover = true;
		m_velocity.y = 0.0f;
		m_jumpCount = 0;
	}

	if (m_pos.y <= m_maxPosY)
	{
		m_maxPosY = m_pos.y;
	}
	else // 最高点に達したら
	{
		m_lastMaxPos.x = m_pos.x;
		m_lastMaxPos.y = m_maxPosY;
	}

	

	bool movingLeft = input.IsPressed("Left");
	bool movingRight = input.IsPressed("Right");

	if (movingLeft)
	{
		m_pos.x -= kMoveSpeed;
	}
	if (movingRight)
	{
		m_pos.x += kMoveSpeed;
	}

	float playerLeft = m_pos.x - kPlayerWidth / 2.0f;
	float playerRight = m_pos.x + kPlayerWidth / 2.0f;
	float playerTop = m_pos.y - kPlayerHeight / 2.0f;
	float playerBottom = m_pos.y + kPlayerHeight / 2.0f;

	//if (input.IsPressed("Left")) // 左入力の時
	//{
	//	m_pos.x -= 3.5f;
	//}
	//if (input.IsPressed("Right")) // 右入力の時
	//{
	//	m_pos.x += 3.5f;
	//}

	assert(m_pMap != nullptr && L"Player:マップの取得ができていません");
	Rect2D moveRange = m_pMap->GetCanMoveRange(m_colRect);
	

	// 天井に着いた時の処理
	if (playerTop < moveRange.GetTop())
	{
		m_pos.y = moveRange.GetTop() + kPlayerHeight / 2.0f;
		// 再びジャンプボタンを押した際と同じ処理をする
		m_isHover = true; 
		m_velocity.y = 0.0f;
		m_jumpCount = 0;
	}

	

	// 床に着地した時の処理
	if (playerBottom > moveRange.GetBottom())
	{
		m_pos.y = moveRange.GetBottom() - kPlayerHeight / 2.0f;
		m_velocity = {};
		m_direction = {};
		m_isGround = true;
		//m_currentFloorY = moveRange.GetBottom();
		m_update = &Player::GroundUpdate;
		m_draw = &Player::GroundDraw;
		m_isOffsetX = false;
		//printfDx(L"空中から床へ");
		return;
	}

	// 左右の壁判定
	if (movingLeft && playerLeft < moveRange.GetLeft())
	{
		m_pos.x = moveRange.GetLeft() + kPlayerWidth / 2.0f;
		//printfDx(L"空中の左右補正です");
	}
	if (movingRight && playerRight > moveRange.GetRight())
	{
		m_pos.x = moveRange.GetRight() - kPlayerWidth / 2.0f;
		//printfDx(L"空中の左右補正です");
	}
	

	m_colRect.pos = m_pos;
	m_colCircle.pos = m_pos;

	
	//m_velocity.y * m_direction.y + kGravity * m_jumpCount * 0.5f;
}

void Player::GroundUpdate(Input& input)
{
	if (input.IsTriggered("Jump") && m_isGround) // ジャンプボタンが押されたとき
	{
		JumpStart();
		m_update = &Player::JumpUpdate;
		m_draw = &Player::JumpDraw;
		return;
	}


	

	bool movingLeft = input.IsPressed("Left");
	bool movingRight = input.IsPressed("Right");

	if (movingLeft)
	{
		m_pos.x -= kMoveSpeed;
	}
	if (movingRight)
	{
		m_pos.x += kMoveSpeed;
	}

	m_velocity.y += kGravity;
	m_pos.y += m_velocity.y;

	float playerLeft = m_pos.x - kPlayerWidth / 2.0f;
	float playerRight = m_pos.x + kPlayerWidth / 2.0f;
	float playerBottom = m_pos.y + kPlayerHeight / 2.0f;

	//if (input.IsPressed("Left")) // 左入力の時
	//{
	//	m_pos.x -= 3.5f;
	//}
	//if (input.IsPressed("Right")) // 右入力の時
	//{
	//	m_pos.x += 3.5f;
	//}

	assert(m_pMap != nullptr && L"Player:マップの取得ができていません");
	Rect2D moveRange = m_pMap->GetCanMoveRange(m_colRect);

	//
	//// 問題点その1
	//// こっちの場合、常にジャンプ状態に更新されてしまう
	////if (playerBottom > m_currentFloorY - 1.0f)
	//// こっちの場合、そもそもここを通らない
	//if (playerBottom > m_currentFloorY + 1.0f)
	//{
	//	m_jumpCount = 0;
	//	m_isGround = false;
	//	m_isLandingCheck = false;
	//	m_update = &Player::JumpUpdate;
	//	m_draw = &Player::JumpDraw;
	//	return;
	//}
	//else
	//{
	//	m_pos.y = moveRange.GetBottom() - kPlayerHeight / 2.0f;
	//	m_isGround = true;
	//	m_currentFloorY = moveRange.GetBottom(); // 現在の床を記録
	//}

	//// 問題点その2
	//// 位置更新
	//m_pos.y = moveRange.GetBottom() - kPlayerHeight / 2.0f;
	////m_pos.y += m_velocity.y * m_direction.y + kGravity * m_jumpCount * 0.5f;
	//m_currentFloorY = moveRange.GetBottom(); // 現在の床を記録

	// 左右の壁判定
	if (m_isOffsetX)
	{
		if (movingLeft && playerLeft < moveRange.GetLeft())
		{
			m_pos.x = moveRange.GetLeft() + kPlayerWidth / 2.0f;
			//printfDx(L"床の左右補正です");
		}
		if (movingRight && playerRight > moveRange.GetRight())
		{
			m_pos.x = moveRange.GetRight() - kPlayerWidth / 2.0f;
			//printfDx(L"床の左右補正です");
		}
	}

	// 地面に接している時
	if (playerBottom >= moveRange.GetBottom())
	{
		m_pos.y = moveRange.GetBottom() - kPlayerHeight / 2.0f;
		m_velocity.y = 0.0f;
		m_isGround = true;
	}
	else // 地面についていない
	{
		m_isGround = false;
		m_jumpCount = 0;
		m_update = &Player::JumpUpdate;
		m_draw = &Player::JumpDraw;
		//printfDx(L"床から空中へ\n");
		return;
	}
	
	m_isOffsetX = true;
	
	Rect2D t = {};
	m_pMap->IsCollision(m_colRect, t);
	

	m_colRect.pos = m_pos;
	m_colCircle.pos = m_pos;
}

void Player::MissUpdate(Input&)
{
	m_missCount++;
	// 一定時間たつまで座標の更新を行わない(止まる)
	if (m_missCount <= kMissFreezeTime)
	{
		return;
	}
	

	m_pos.y += m_velocity.y * m_direction.y + kGravity * m_missCount * 0.5f;
	
	if (m_pos.y >= Game::kScreenHeight)
	{
		m_pos.y = Game::kScreenHeight + kPlayerHeight / 2;
		m_isGround = true;
	}


	if (m_missCount >= kMissEndTime && m_isGround)
	{
		Init();
		return;
	}
	m_colRect.pos = m_pos;
	m_colCircle.pos = m_pos;
}

void Player::JumpDraw()
{
	int drawX = m_pos.x - m_pCamera->scroll.x;
	int drawY = m_pos.y - m_pCamera->scroll.y;
	DrawBox(drawX - 16, drawY - 16, drawX + 16, drawY + 16, 0xaaffff, true);
	DrawRectRotaGraph(drawX, drawY, kGraphWidth * 3, 0, kGraphWidth, kGraphHeight, 1.0f, 0.0f, m_graphHandle, true, false);


#ifdef _DEBUG
	/*Position2 drawPos = { drawX,drawY };
	m_colCircle.pos = drawPos;
	m_colRect.pos = drawPos;*/
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX,drawY);
#endif
}

void Player::GroundDraw()
{
	int drawX = m_pos.x - m_pCamera->scroll.x;
	int drawY = m_pos.y - m_pCamera->scroll.y;
	DrawBox(drawX - 16, drawY - 16, drawX + 16, drawY + 16, 0x22ff00, true);
	DrawRectRotaGraph(drawX, drawY, 0, 0, kGraphWidth, kGraphHeight, 1.0f, 0.0f, m_graphHandle, true, false);
	
#ifdef _DEBUG
	/*Position2 drawPos = { drawX,drawY };
	m_colCircle.pos = drawPos;
	m_colRect.pos = drawPos;*/
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void Player::MissDraw()
{
	int drawX = m_pos.x - m_pCamera->scroll.x;
	int drawY = m_pos.y - m_pCamera->scroll.y;
	DrawBox(drawX - 16, drawY - 16, drawX + 16, drawY + 16, 0xff00aa, true);
	DrawRectRotaGraph(drawX, drawY, kGraphWidth, 0, kGraphWidth, kGraphHeight, 1.0f, 0.0f, m_graphHandle, true, false);
	
	DrawString(Game::kScreenWidth / 2, Game::kScreenHeight / 2, L"Miss!", 0xffffff);
#ifdef _DEBUG
	/*Position2 drawPos = { drawX,drawY };
	m_colCircle.pos = drawPos;
	m_colRect.pos = drawPos;*/
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void Player::JumpStart()
{
	m_direction = { 0.0f,1.0f };
	m_velocity = { 0.0f, kJumpPower };
	m_isHover = false;
	m_isGround = false;
	m_jumpCount = 0;
#ifdef _DEBUG
	m_maxPosY = m_pos.y;
#endif
}

void Player::MissStart()
{
	m_direction = { 0.0f,1.0f };
	m_velocity = { 0.0f, kJumpPower * 1.01f }; // 普通のジャンプより高い
	m_isHover = false;
	m_isGround = false;
	m_isMiss = true;
	m_jumpCount = 0;
	m_missCount = 0;
	m_update = &Player::MissUpdate;
	m_draw = &Player::MissDraw;
}

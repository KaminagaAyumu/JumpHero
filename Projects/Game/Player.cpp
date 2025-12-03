#include "Player.h"
#include "../Utility/Input.h"
#include "../Utility/Game.h"
#include "../Utility/Map.h"
#include "../Utility/Camera.h"
#include "GameManager.h"
#include "Chest.h"
#include "DxLib.h"
#include <cassert>

namespace
{
	constexpr float kGravity				= 0.5f;					// プレイヤーにかかる重力
	constexpr float kGroundY				= 570.0f;				// 床の座標
	constexpr float kJumpPower				= -15.0f;				// ジャンプ時の上に上がる力
	constexpr float kMissJumpPower			= kJumpPower * 1.01f;	// ミスしたときの上に上がる力
	constexpr float kNormalMoveSpeed		= 3.5f;					// 左右に動くスピード

	constexpr int	kGraphWidth				= 45;					// プレイヤー画像の幅
	constexpr int	kGraphHeight			= 45;					// プレイヤー画像の高さ

	constexpr float kPlayerWidth			= 45.0f;				// プレイヤーの実際の幅
	constexpr float kPlayerHeight			= 45.0f;				// プレイヤーの実際の高さ

	constexpr int	kEntryTextDispTime		= 60;					// 登場テキストを表示する時間
	constexpr float	kEntryMoveSpeed			= 0.05f;				// プレイヤー登場のスピード

	constexpr int	kMissFreezeTime			= 10;					// ミスしたときに止まるフレーム数
	constexpr int	kMissEndTime			= 180;					// ミス処理が終わるフレーム数

	constexpr int	kJumpAddScore			= 10;					// ジャンプしたときの加算スコア

	constexpr int	kPowerUpLevelOne		= 1;					// パワーアップ1段階目
	constexpr int	kJumpLimitNumLevelOne	= 20;					// パワーアップが解除されるまでのジャンプ回数

	constexpr int	kPowerUpLevelMax		= 2;					// パワーアップ最大値
	constexpr int	kJumpLimitNumLevelMax	= 25;					// パワーアップが解除されるまでのジャンプ回数

	// プレイヤーの登場の初期位置
	// 複数マップになった際に使用しなくなるはず
	inline const static Vector2 kFirstPos = { -10.0f,Game::kScreenHeight - 135.0f - kPlayerHeight / 2 };
	// プレイヤーの登場の終了位置
	inline const static Vector2 kEntryEndPos = { 90.0f, Game::kScreenHeight - 135.0f - kPlayerHeight / 2 };
}

Player::Player(Map* map, GameManager* gameManager) :
	Actor(Types::ActorType::Player),
	m_direction{},
	m_velocity{},
	m_graphHandle(-1),
	m_frameCount(0),
	m_jumpCount(0),
	m_level(0),
	m_isGround(false),
	m_isHover(false),
	m_isMiss(false),
	m_isOffsetX(true),
	m_isOpenChest(false),
	m_isLevelDown(false),
	m_currentFloorY(0.0f),
	m_pMap(map),
	m_pGameManager(gameManager),
	m_update(&Player::EntryUpdate),
	m_draw(&Player::EntryDraw)
{
}

Player::~Player()
{
	DeleteGraph(m_graphHandle);
}

void Player::Init()
{
	m_pos = kFirstPos;
	m_direction = {};
	m_velocity = {};
	m_graphHandle = LoadGraph(L"data/Idle.png");
	m_colCircle = { m_pos,kGraphWidth * 0.5f };
	m_colRect = { m_pos,kGraphWidth,kGraphHeight };
	m_frameCount = 0;
	m_jumpCount = 0;
	m_level = 0;
	m_isGround = false;
	m_isHover = false;
	m_isMiss = false;
	m_isOpenChest = false;
	m_isLevelDown = false;
	m_currentFloorY = 0.0f;
	m_update = &Player::EntryUpdate;
	m_draw = &Player::EntryDraw;
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
#ifdef _DEBUG
		printfDx(L"Player : 敵と衝突しました\n");
#endif 

		//MissStart();
	}

	// 宝箱と衝突した時
	if (info.otherType == Types::ActorType::Chest)
	{
#ifdef _DEBUG
		printfDx(L"Player : 宝箱と衝突しました\n");
#endif 

		if (!m_isGround && m_isOpenChest)
		{
			// 当たっている宝箱を取得
			auto chest = dynamic_cast<Chest*>(info.other);
			if (chest) // 宝箱が存在したら
			{
				// どこから当たったか、プレイヤーが横から開けるかを判定
				// chest->GetColRect(); // 宝箱の矩形を取得


				chest->OpenChest();
				m_isOpenChest = false;
			}
		}
		m_isOpenChest = true;
	}
}

void Player::CheckPowerDown()
{
	if (m_jumpCount <= 0 && !m_isLevelDown) // ジャンプカウンタが0の時
	{
#ifdef _DEBUG
		printfDx(L"レベルが下がった\n");
#endif
		m_level--; // レベルダウン
		if (m_level == kPowerUpLevelOne) // レベルダウンした結果レベルが1になったら
		{
			m_jumpCount = kJumpLimitNumLevelOne;
		}
		else // レベルが0になったら
		{
			m_jumpCount = 0; // ジャンプカウンタをそれ以上下がらないようにする
			m_isLevelDown = true; // レベルが0になったのでここを通らないようにする
		}
	}
}

void Player::EntryUpdate(Input&)
{
	m_frameCount++;
	if (m_frameCount < kEntryTextDispTime) // テキストを表示している間は動かない
	{
		return;
	}
	// 位置を初期位置から登場終了位置まで線形補完で動かす
	m_pos = Geometry::LerpVec2(m_pos, kEntryEndPos, kEntryMoveSpeed);

	if (m_pos.x >= kEntryEndPos.x - 1.0f) // 終了位置と大体同じになったら終了
	{
		m_pos = kEntryEndPos; // プレイヤーの位置を終了位置に動かす
		m_update = &Player::JumpUpdate; // 更新処理をジャンプ状態に
		m_draw = &Player::JumpDraw; // 描画処理をジャンプ状態に
		return;
	}
}

void Player::JumpUpdate(Input& input)
{
	m_frameCount++;
	m_pos.y += m_velocity.y * m_direction.y + kGravity * m_frameCount * 0.5f;

	if (m_level > 0) // レベルが上がっている時
	{
		CheckPowerDown(); // プレイヤーのパワーダウンをするかどうか判定する
	}

	//m_velocity.y = kJumpPower;

	// 空中で浮いたかどうかの判定
	if (m_isHover) // すでに浮いている場合
	{
		// 何もしない
	}
	else // 浮いていない場合
	{
		// ジャンプの高さを上下に調整できる
		if (input.IsPressed("Up")) // 上ボタンが押されたとき
		{
			m_velocity.y = kJumpPower - 0.5f;
		}
		if (input.IsPressed("Down")) // 下ボタンが押されたとき
		{
			m_velocity.y = kJumpPower + 0.5f;
		}
	}

	if (input.IsTriggered("PowerUp")) // パワーアップボタンが押されたとき
	{
		// パワーアップ処理を行う
		m_pGameManager->PowerUpPlayer();
	}

	if (input.IsTriggered("Jump")) // 再びジャンプボタンが押されたら
	{
		m_isHover = true;
		m_velocity.y = 0.0f;
		m_frameCount = 0;
	}


	bool movingLeft = input.IsPressed("Left");
	bool movingRight = input.IsPressed("Right");

	if (movingLeft)
	{
		m_pos.x -= kNormalMoveSpeed;
	}
	if (movingRight)
	{
		m_pos.x += kNormalMoveSpeed;
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
		m_frameCount = 0;
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
	if (input.IsTriggered("PowerUp")) // パワーアップボタンが押されたとき
	{
		// パワーアップ処理を行う
		m_pGameManager->PowerUpPlayer();
	}
	if (input.IsTriggered("Jump") && m_isGround) // ジャンプボタンが押されたとき
	{
		JumpStart(); // ジャンプする際の処理を行う
		return;
	}
	
	// 左右移動フラグの設定
	bool movingLeft = input.IsPressed("Left");
	bool movingRight = input.IsPressed("Right");

	if (movingLeft) // 左に移動している場合
	{
		m_pos.x -= kNormalMoveSpeed; // 左に力を加える
	}
	if (movingRight) // 右に移動している場合
	{
		m_pos.x += kNormalMoveSpeed; // 右に力を加える
	}

	m_velocity.y += kGravity; // 重力をかける
	m_pos.y += m_velocity.y; // Y座標の更新

	// プレイヤーの下、左、右端座標を定義(上端座標は使わないので定義していない)
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

	// マップが取得できていない場合止める
	assert(m_pMap != nullptr && L"Player:マップの取得ができていません");
	// 移動可能範囲の矩形を取得
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
			// 移動可能範囲の左端座標からプレイヤーの幅の半分左にずらしたところに補正
			m_pos.x = moveRange.GetLeft() + kPlayerWidth / 2.0f;
			//printfDx(L"床の左右補正です");
		}
		if (movingRight && playerRight > moveRange.GetRight())
		{
			// 移動可能範囲の右端座標からプレイヤーの幅の半分右にずらしたところに補正
			m_pos.x = moveRange.GetRight() - kPlayerWidth / 2.0f;
			//printfDx(L"床の左右補正です");
		}
	}

	// 地面に接している時
	if (playerBottom >= moveRange.GetBottom())
	{
		// 移動可能範囲の下端座標からプレイヤーの高さの半分上にあげたところに補正
		m_pos.y = moveRange.GetBottom() - kPlayerHeight / 2.0f;
		m_velocity.y = 0.0f; // 地面にいるのでY方向の力をなくす
		m_isGround = true; // 地面についている
	}
	else // 地面についていない時
	{
		// ジャンプはしないが、空中でのUpdateに変更
		m_isGround = false; // 地面についていない
		m_frameCount = 0; // 時間経過をリセット
		m_update = &Player::JumpUpdate; // 更新処理をジャンプ状態に
		m_draw = &Player::JumpDraw; // 描画処理をジャンプ状態に
		//printfDx(L"床から空中へ\n");
		return;
	}

	// 次のフレームからは左右の補正を行えるようにする 
	m_isOffsetX = true;

	m_colRect.pos = m_pos; // 矩形の座標更新
	m_colCircle.pos = m_pos; // 円の座標更新
}

void Player::MissUpdate(Input&)
{
	m_frameCount++;
	// 一定時間たつまで座標の更新を行わない(止まる)
	if (m_frameCount <= kMissFreezeTime)
	{
		return;
	}

	// 重力とジャンプの速度を加える
	m_pos.y += m_velocity.y * m_direction.y + kGravity * m_frameCount * 0.5f;
	
	// プレイヤーの中心Y座標が画面下を超えたら
	if (m_pos.y >= Game::kScreenHeight)
	{
		// Y座標を画面下+プレイヤーの大きさの半分の場所に固定
		m_pos.y = Game::kScreenHeight + kPlayerHeight / 2;
		// 床にいるフラグをtrueにする
		m_isGround = true;
	}

	// 地面についたかつミス状態が終わる時間になったら
	if (m_frameCount >= kMissEndTime && m_isGround)
	{
		Init(); // 初期化処理を行う
		return;
	}
	m_colRect.pos = m_pos; // 矩形の座標更新
	m_colCircle.pos = m_pos; // 円の座標更新
}

void Player::EntryDraw()
{
	// 経過時間が登場時間を超えたら
	if (m_frameCount >= kEntryTextDispTime)
	{
		// ReadyとGOを表示する
		DrawString(Game::kScreenWidth / 2, Game::kScreenHeight / 2, L"Ready、GO!", 0xff0000);
	}
	else // 登場準備中なら
	{
		// Readyを表示する
		DrawString(Game::kScreenWidth / 2, Game::kScreenHeight / 2, L"Ready、", 0xff0000);
	}

	int drawX = static_cast<int>(m_pos.x - m_pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - m_pCamera->scroll.y);
	DrawBox(static_cast<int>(drawX - kPlayerWidth * 0.5f), static_cast<int>(drawY - kPlayerHeight * 0.5f), static_cast<int>(drawX + kPlayerWidth * 0.5f), static_cast<int>(drawY + kPlayerHeight * 0.5f), 0x22ff00, true);
	DrawRectRotaGraph(drawX, drawY, 0, 0, kGraphWidth, kGraphHeight, 1.0f, 0.0f, m_graphHandle, true, false);

#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void Player::JumpDraw()
{
	int drawX = static_cast<int>(m_pos.x - m_pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - m_pCamera->scroll.y);
	DrawBox(static_cast<int>(drawX - kPlayerWidth * 0.5f), static_cast<int>(drawY - kPlayerHeight * 0.5f), static_cast<int>(drawX + kPlayerWidth * 0.5f), static_cast<int>(drawY + kPlayerHeight * 0.5f), 0xaaffff, true);
	DrawRectRotaGraph(drawX, drawY, kGraphWidth * 3, 0, kGraphWidth, kGraphHeight, 1.0f, 0.0f, m_graphHandle, true, false);

#ifdef _DEBUG

	Rect2D moveRange = m_pMap->GetCanMoveRange(m_colRect);
	moveRange.Draw(drawX, drawY);

	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX,drawY);
#endif
}

void Player::GroundDraw()
{
	int drawX = static_cast<int>(m_pos.x - m_pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - m_pCamera->scroll.y);
	DrawBox(static_cast<int>(drawX - kPlayerWidth * 0.5f), static_cast<int>(drawY - kPlayerHeight * 0.5f), static_cast<int>(drawX + kPlayerWidth * 0.5f), static_cast<int>(drawY + kPlayerHeight * 0.5f), 0x22ff00, true);
	DrawRectRotaGraph(drawX, drawY, 0, 0, kGraphWidth, kGraphHeight, 1.0f, 0.0f, m_graphHandle, true, false);
	

#ifdef _DEBUG

	// 当たっているマップチップを表示(スクロールは考えない)
	Rect2D collisionMapChip = {};
	if (m_pMap->IsCollision(m_colRect, collisionMapChip))
	{
		collisionMapChip.Draw();
	}

	Rect2D moveRange = m_pMap->GetCanMoveRange(m_colRect);
	moveRange.Draw(drawX,drawY);

	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void Player::MissDraw()
{
	int drawX = static_cast<int>(m_pos.x - m_pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - m_pCamera->scroll.y);
	DrawBox(static_cast<int>(drawX - kPlayerWidth * 0.5f), static_cast<int>(drawY - kPlayerHeight * 0.5f), static_cast<int>(drawX + kPlayerWidth * 0.5f), static_cast<int>(drawY + kPlayerHeight * 0.5f), 0xff00aa, true);
	DrawRectRotaGraph(drawX, drawY, kGraphWidth, 0, kGraphWidth, kGraphHeight, 1.0f, 0.0f, m_graphHandle, true, false);
	
	DrawString(Game::kScreenWidth / 2, Game::kScreenHeight / 2, L"Miss!", 0xffffff);
#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

bool Player::IsOpenChestX() const
{
	// レベルが1以上なら宝箱が横から開けられる
	return m_level <= kPowerUpLevelOne;
}

void Player::JumpStart()
{
	m_direction = { 0.0f,1.0f }; // ジャンプの方向を上向きにする
	m_velocity = { 0.0f, kJumpPower }; // ジャンプの力を加える
	m_isHover = false; // 空中で浮いたかどうかをリセット
	m_isGround = false; // ジャンプしたので地面についていないとする
	m_frameCount = 0; // 時間経過をリセット
	m_jumpCount--; // ジャンプ回数を減らす
	m_pGameManager->AddScore(kJumpAddScore); // スコアを加算
	m_update = &Player::JumpUpdate; // 更新処理をジャンプ状態に
	m_draw = &Player::JumpDraw; // 描画処理をジャンプ状態に
}

void Player::MissStart()
{
	m_direction = { 0.0f,1.0f }; // ジャンプの方向を上向きにする
	m_velocity = { 0.0f, kMissJumpPower }; // 普通のジャンプより高い力を加える
	m_isHover = false; // 空中で浮いたかどうかをリセット
	m_isGround = false; // 一応ジャンプするので地面についていないとする
	m_isMiss = true; // ミスフラグをtrueにする
	m_frameCount = 0; // 時間経過をリセット
	m_update = &Player::MissUpdate; // 更新処理をミス状態に
	m_draw = &Player::MissDraw;// 描画処理をミス状態に
}

bool Player::PowerUp()
{
	// レベルが最大値だったら
	if (m_level >= kPowerUpLevelMax)
	{
#ifdef _DEBUG
		printfDx(L"強化状態はマックスです\n");
#endif
		return false; // パワーアップできないのでfalseを返す
	}
#ifdef _DEBUG
	printfDx(L"プレイヤーの強化に成功\n");
#endif
	m_level++; // レベルを1増やす
	if (m_level == kPowerUpLevelOne) // 1段階目なら
	{
		m_jumpCount = kJumpLimitNumLevelOne; // 20回ジャンプするまでパワーアップ継続
	}
	else if (m_level == kPowerUpLevelMax) // 2段階目なら
	{
		m_jumpCount = kJumpLimitNumLevelMax; // 25回ジャンプするまでパワーアップ継続
		m_pGameManager->ChangeEnemyToCoin(); // 敵をアイテムに変える処理を呼ぶ
	}
	m_isLevelDown = false; // レベルが下がったかどうかの判定を可能にする

	return true; // パワーアップできたのでtrueを返す
}

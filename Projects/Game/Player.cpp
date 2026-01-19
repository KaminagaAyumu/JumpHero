#include "Player.h"
#include "../Utility/Input.h"
#include "../Utility/Game.h"
#include "../Utility/Map.h"
#include "../Utility/Camera.h"
#include "../Utility/AnimationLoader.h"
#include "GameManager.h"
#include "Chest.h"
#include "DxLib.h"
#include <cassert>

namespace
{
	constexpr float kGravity = 0.30f;					// プレイヤーにかかる重力
	constexpr float kGroundY = 570.0f;				// 床の座標
	constexpr float kJumpPower = -15.0f;				// ジャンプ時の上に上がる力
	constexpr float kMissJumpPower = kJumpPower * 1.01f;	// ミスしたときの上に上がる力
	constexpr float kNormalMoveSpeed = 3.5f;					// 左右に動くスピード
	constexpr float kUpGravityScale = 1.5f;				// 上ボタンを押したときの重力倍率
	constexpr float kDownGravityScale = 0.5f;			// 下ボタンを押したときの重力倍率

	constexpr int	kGraphWidth = 45;					// プレイヤー画像の幅
	constexpr int	kGraphHeight = 45;					// プレイヤー画像の高さ

	constexpr float kGraphScale = 1.40625f;	// 画像の拡大率

	constexpr float kPlayerWidth = 40.0f;				// プレイヤーの実際の幅
	constexpr float kPlayerHeight = 40.0f;				// プレイヤーの実際の高さ

	constexpr int	kChestChipNo = 1;					// マップの宝箱の番号
	constexpr float kMapColMargin = 0.5f;				// マップとの当たり判定のマージン
	constexpr float kChestConfirmRange = 1.0f;			// マップから宝箱を見る範囲

	constexpr int	kEntryTextDispTime = 60;					// 登場テキストを表示する時間
	constexpr float	kEntryMoveSpeed = 0.05f;				// プレイヤー登場のスピード

	constexpr int	kMissFreezeTime = 10;					// ミスしたときに止まるフレーム数
	constexpr int	kMissEndTime = 180;					// ミス処理が終わるフレーム数

	constexpr int	kJumpAddScore = 10;					// ジャンプしたときの加算スコア

	constexpr int	kPowerUpLevelOne = 1;					// パワーアップ1段階目
	constexpr int	kJumpLimitNumLevelOne = 20;					// パワーアップが解除されるまでのジャンプ回数

	constexpr int	kPowerUpLevelMax = 2;					// パワーアップ最大値
	constexpr int	kJumpLimitNumLevelMax = 25;					// パワーアップが解除されるまでのジャンプ回数

	constexpr int	kAttackableTime = 600;				// 攻撃可能時間

	constexpr float kEntryEndXOffset = 100.0f;			// プレイヤー登場終了位置のXオフセット

	// アニメーション関連
	constexpr int kAnimGroundNum = 18;				// 地上アニメーションの枚数
	constexpr int kAnimGroundFrame = 5;				// 地上アニメーションのフレーム数
	//constexpr int kAnimJumpFrame = 4;					// ジャンプアニメーションのフレーム数
}

Player::Player(std::weak_ptr<Map> map, GameManager* gameManager) :
	Actor(Types::ActorType::Player),
	m_direction{},
	m_velocity{},
	m_entryEndPos{},
	m_barrierPos{},
	m_graphHandle(-1),
	m_frameCount(0),
	m_jumpCount(0),
	m_attackCount(0),
	m_level(0),
	m_prevPosY(0.0f),
	m_isGround(false),
	m_isHover(false),
	m_isMiss(false),
	m_isLevelDown(false),
	m_isJumpStart(false),
	m_isFreeze(false),
	m_isAttackable(false),
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
	auto pMap = m_pMap.lock();
	m_pos = pMap->GetStartPosToMap();
	m_entryEndPos = m_pos + Vector2{ kEntryEndXOffset,0.0f };
	m_direction = {};
	m_velocity = {};
	m_barrierPos = {};
	m_graphHandle = LoadGraph(L"data/img/player.png");
	m_colCircle = { m_pos,kPlayerWidth * 0.5f };
	m_colRect = { m_pos,kPlayerWidth,kPlayerHeight };
	m_frameCount = 0;
	m_jumpCount = 0;
	m_attackCount = 0;
	m_level = 0;
	m_isGround = false;
	m_isHover = false;
	m_isMiss = false;
	m_isLevelDown = false;
	m_isJumpStart = false;
	m_isFreeze = false;
	m_isAttackable = false;
	m_update = &Player::EntryUpdate;
	m_draw = &Player::EntryDraw;
	AnimationLoader::LoadAnimationData(L"data/animation/player.csv", m_graphHandle, m_animations);
	
	m_animations["Idle"].SetScale(kGraphScale);
	m_animations["Jump"].SetScale(kGraphScale);
	m_animations["Walk"].SetScale(kGraphScale);

	ChangeAnimation(m_animations["Idle"]);
}

void Player::InitMap(std::weak_ptr<Map> map)
{
	m_pMap = map;
}

void Player::Update(Input& input)
{
	if (m_isFreeze)
	{
		return;
	}

	m_attackCount--;
	if (m_attackCount < 0)
	{
		m_attackCount = 0;
		m_isAttackable = false;
	}

	CheckAnimation();
	m_currentAnim.Update();

	(this->*m_update)(input);
}

void Player::Draw()
{
	(this->*m_draw)();
}

bool Player::IsMiss() const
{
	return (m_update == &Player::MissUpdate);
}

void Player::IsCollision(const Types::CollisionInfo& info)
{
	// 敵と衝突したらミス処理を行う
	if (info.otherType == Types::ActorType::Enemy && !m_isMiss)
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

void Player::MoveOperation(Input& input)
{
	float dx = 0.0f; // X軸の未来の移動量
	// 左右移動の処理
	const bool movingLeft = input.IsPressed("Left");
	const bool movingRight = input.IsPressed("Right");
	
	if (movingLeft) // 左ボタンが押されている時
	{
		dx -= kNormalMoveSpeed; // 左に移動
	}
	if (movingRight) // 右ボタンが押されている時
	{
		dx += kNormalMoveSpeed; // 右に移動
	}

	// 重力の処理
	float gravity = kGravity; // 基本の重力

	if (m_velocity.y < 0.0f) // 上に移動している時
	{
		if (input.IsPressed("Down")) // 下ボタンが押されたとき
		{
			gravity *= kUpGravityScale; // 重力を強くする
		}
		if (input.IsPressed("Up")) // 上ボタンが押されたとき
		{
			gravity *= kDownGravityScale; // 重力を弱くする
		}
	}

	// 重力を速度に加える
	m_velocity.y += gravity;


	float dy = m_velocity.y; // Y軸の未来の移動量
	ContactFrags frags; // 当たり判定の結果を格納する構造体

	// X軸の当たり判定
	Position2 tryPosX = m_pos;
	// 未来の位置を計算
	tryPosX.x += dx;

	// マップを使えるようにlockする
	auto pMap = m_pMap.lock();

	// 当たり判定用の矩形を作成して移動可能範囲を取得
	Rect2D rectX(tryPosX, kPlayerWidth, kPlayerHeight - kMapColMargin);
	Rect2D rangeX = pMap->GetCanMoveRange(rectX);

	// プレイヤーの左右端の座標を取得
	float leftX = tryPosX.x - kPlayerWidth * 0.5f;
	float rightX = tryPosX.x + kPlayerWidth * 0.5f;

	if (leftX < rangeX.GetLeft()) // 左端が移動可能範囲を超えた場合
	{
		// 位置を移動可能範囲内に修正
		tryPosX.x = rangeX.GetLeft() + kPlayerWidth * 0.5f + kMapColMargin;
		frags.isHitLeft = true; // 左に当たったフラグを立てる
		// 速度を抑える(問題があれば0にする)
		dx = tryPosX.x - m_pos.x;
	}
	else if (rightX > rangeX.GetRight()) // 右端が移動可能範囲を超えた場合
	{
		// 位置を移動可能範囲内に修正
		tryPosX.x = rangeX.GetRight() - kPlayerWidth * 0.5f - kMapColMargin;
		frags.isHitRight = true; // 右に当たったフラグを立てる
		// 速度を抑える(問題があれば0にする)
		dx = tryPosX.x - m_pos.x;
	}

	// 修正した座標を実際の座標に反映
	m_pos.x = tryPosX.x;

	// Y軸の当たり判定
	Position2 tryPosY = m_pos;
	// 未来の位置を計算
	tryPosY.y += dy;

	// 当たり判定用の矩形を作成して移動可能範囲を取得
	Rect2D rectY(tryPosY, kPlayerWidth, kPlayerHeight);
	Rect2D rangeY = pMap->GetCanMoveRange(rectY);

	// プレイヤーの上下端の座標を取得
	float topY = tryPosY.y - kPlayerHeight * 0.5f;
	float bottomY = tryPosY.y + kPlayerHeight * 0.5f;

	// 貫通しない床に当たったかどうか
	bool isHitNormalFloor = false;
	float margin = 0.1f; // マージン

	// 落下している時に床に当たった場合
	if (dy > 0.0f && bottomY > rangeY.GetBottom())
	{
		// 位置を移動可能範囲内に修正
		tryPosY.y = rangeY.GetBottom() - kPlayerHeight * 0.5f;
		frags.isHitGround = true; // 床に当たったフラグを立てる
		isHitNormalFloor = true; // 貫通しない床に当たったとする
		m_velocity.y = 0.0f; // Y方向の速度を0にする
		dy = tryPosY.y - m_pos.y; // 移動量を修正
		m_frameCount = 0; // 時間経過をリセット
	}
	else if (dy < 0.0f && topY < rangeY.GetTop()) // 上昇している時に天井に当たった場合
	{
		// 位置を移動可能範囲内に修正
		tryPosY.y = rangeY.GetTop() + kPlayerHeight * 0.5f;
		frags.isHitCeil = true; // 天井に当たったフラグを立てる
		m_velocity.y = 0.0f; // Y方向の速度を0にする
		dy = tryPosY.y - m_pos.y; // 移動量を修正
		// 再びジャンプボタンを押した際と同じ処理をする
		m_isHover = true;
		m_frameCount = 0;
	}

	// 下からのみ貫通できる床との判定
	if (!isHitNormalFloor && dy > 0.0f) // 地面との判定を行わなかった際に落下中なら
	{
		const float oldBottom = m_prevPosY + kPlayerHeight * 0.5f; // 前回の下端のY座標

		const float tileSize = pMap->GetTileSize(); // マップのタイルサイズを取得
		int leftTileX = pMap->WorldPosToMapPos(m_pos.x - kPlayerWidth * 0.5f, tileSize); // プレイヤーの左端のマップ座標X
		int rightTileX = pMap->WorldPosToMapPos(m_pos.x + kPlayerWidth * 0.5f, tileSize); // プレイヤーの右端のマップ座標X
		int footTileY = pMap->WorldPosToMapPos(bottomY + 0.5f, tileSize); // プレイヤーの下端のマップ座標Y

		bool hitOneWay = false; // 片面通行の床に当たったかどうか
		float candidateTop = pMap->GetMapHeight() * tileSize; // 候補となる床の上端のY座標(仮)

		for (int x = leftTileX; x <= rightTileX; x++) // プレイヤーの幅が大きい時を考慮
		{
			int chipNo = pMap->GetMapChipNum(x, footTileY);
			if (pMap->IsOnlyTopTile(chipNo))
			{
				float tileTop = footTileY * tileSize; // タイルの上端のY座標

				if (oldBottom <= tileTop) // 前回の下端が床の上にあった場合
				{
					if (tileTop < candidateTop)
					{
						candidateTop = min(candidateTop, tileTop);
						// 一方通行床に当たったフラグを立てる
						hitOneWay = true;
					}
				}
			}
		}
		if (hitOneWay)
		{
			tryPosY.y = candidateTop - kPlayerHeight * 0.5f;
			frags.isHitGround = true; // 床に当たったフラグを立てる
			m_velocity.y = 0.0f; // Y方向の速度を0にする
			dy = tryPosY.y - m_pos.y; // 移動量を修正
			m_frameCount = 0; // 時間経過をリセット
		}
	}
	

	// 実際の座標に反映
	m_pos.y = tryPosY.y;

	// 状態の更新
	m_isGround = frags.isHitGround;
	if (m_isGround) // 地面についている時
	{
		m_update = &Player::GroundUpdate; // 更新処理を地面についている時に
		m_draw = &Player::GroundDraw; // 描画処理を地面についている時に
	}
	else // 空中にいる時
	{
		m_update = &Player::JumpUpdate; // 更新処理をジャンプ状態に
		m_draw = &Player::JumpDraw; // 描画処理をジャンプ状態に
	}

	if (m_barrierPos.isActive)
	{
		if (m_pos.x >= m_barrierPos.pos.x)
		{
			m_pos.x = m_barrierPos.pos.x;
		}
	}

	m_colCircle.pos = m_pos; // 円の座標更新
	m_colRect.pos = m_pos; // 矩形の座標更新
	m_prevPosY = m_pos.y; // 前回のY座標を更新

	CheckHitToChest(input);
}

void Player::CheckHitToChest(Input& input)
{
	// 宝箱の位置を調べる
	bool isRightSide = false;
	bool isLeftSide = false;
	// 宝箱が見つかった際に見つかった座標を指定できるようにするための変数
	int chestX = -1;
	int chestY = -1;
	
	// マップを使えるようにlockする
	auto pMap = m_pMap.lock();

	float tileSize = pMap->GetTileSize(); // マップのタイルサイズを取得

	const float top = m_pos.y - kPlayerHeight * 0.5f; // プレイヤーの上端のY座標
	const float bottom = m_pos.y + kPlayerHeight * 0.5f; // プレイヤーの下端のY座標
	const float left = m_pos.x - kPlayerWidth * 0.5f; // プレイヤーの左端のX座標
	const float right = m_pos.x + kPlayerWidth * 0.5f; // プレイヤーの右端のX座標

	int topY = pMap->WorldPosToMapPos(top, tileSize); // プレイヤーの上端のマップ座標Y
	int bottomY = pMap->WorldPosToMapPos(bottom, tileSize); // プレイヤーの下端のマップ座標Y
	int leftX = pMap->WorldPosToMapPos(left, tileSize); // プレイヤーの左端のマップ座標X
	int rightX = pMap->WorldPosToMapPos(right, tileSize); // プレイヤーの右端のマップ座標X

	const float leftProbe = left - kChestConfirmRange; // プレイヤーの左端の少し左のX座標
	int leftProbeX = pMap->WorldPosToMapPos(leftProbe, tileSize);
	
	for (int y = topY; y <= bottomY; y++)
	{
		// int chipNo = pMap->GetMapChipNum(leftProbeX,y);
		int chipNo = pMap->GetPositioningData(leftProbeX, y);
		if(pMap->IsChestTile(chipNo))
		{
			isLeftSide = true;
			
#ifdef _DEBUG
			printfDx(L"Player: 宝箱の右側にいる\n");
#endif 
			if(chestX != -1 && chestY != -1)
			{
				// すでに宝箱が見つかっている場合は何もしない
				break;
			}
			chestX = leftProbeX;
			chestY = y;
			if(IsOpenChestX()) // 宝箱が横から開けられる時
			{
				// 宝箱を開ける
				m_pGameManager->OpenChestToPosition(chestX, chestY);
			}
			break;
		}
	}

	const float rightProbe = right + kChestConfirmRange; // プレイヤーの右端の少し右のX座標
	int rightProbeX = pMap->WorldPosToMapPos(rightProbe, tileSize);

	for (int y = topY; y <= bottomY; y++)
	{
		//int chipNo = pMap->GetMapChipNum(rightProbeX, y);
		int chipNo = pMap->GetPositioningData(rightProbeX, y);
		if (pMap->IsChestTile(chipNo))
		{
			isRightSide = true;
			if(chestX != -1 && chestY != -1)
			{
				// すでに宝箱が見つかっている場合は何もしない
				break;
			}
			chestX = rightProbeX;
			chestY = y;
#ifdef _DEBUG
			printfDx(L"Player: 宝箱の左側にいる\n");
#endif 
			if (IsOpenChestX()) // 宝箱が横から開けられる時
			{
				// 宝箱を開ける
				m_pGameManager->OpenChestToPosition(chestX, chestY);
			}
			break;
		}
	}
}

void Player::CheckAnimation()
{
	if (m_isGround) // 地面についている時
	{
		if (m_velocity.x != 0.0f)
		{
			ChangeAnimation(m_animations["Idle"]);
		}
		else
		{
			ChangeAnimation(m_animations["Walk"]);
		}
	}
	else
	{
		ChangeAnimation(m_animations["Jump"]);
	}
	// 他の処理も今後実装
}

void Player::EntryUpdate(Input&)
{
	m_frameCount++;
	if (m_frameCount < kEntryTextDispTime) // テキストを表示している間は動かない
	{
		return;
	}
	// 位置を初期位置から登場終了位置まで線形補完で動かす
	m_pos = Geometry::LerpVec2(m_pos, m_entryEndPos, kEntryMoveSpeed);

	if (m_pos.x >= m_entryEndPos.x - 1.0f) // 終了位置と大体同じになったら終了
	{
		m_pos = m_entryEndPos; // プレイヤーの位置を終了位置に動かす
		m_frameCount = 0; // フレームカウントをリセット
		m_update = &Player::JumpUpdate; // 更新処理をジャンプ状態に
		m_draw = &Player::JumpDraw; // 描画処理をジャンプ状態に
		return;
	}
}

void Player::JumpUpdate(Input& input)
{
	if (m_level > 0) // レベルが上がっている時
	{
		CheckPowerDown(); // プレイヤーのパワーダウンをするかどうか判定する
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

	MoveOperation(input); // 移動処理を行う
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
		Position2Int chestPos;
		if (IsOnChestTop(chestPos)) // 宝箱の上にいる時
		{
			// 宝箱を開ける
			m_pGameManager->OpenChestToPosition(chestPos.x, chestPos.y);
		}
		return;
	}
	else
	{
		m_isJumpStart = false;
	}

	MoveOperation(input); // 移動処理を行う
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
	m_currentAnim.Draw({ drawX, drawY }, false);
	
#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void Player::JumpDraw()
{
	int drawX = static_cast<int>(m_pos.x - m_pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - m_pCamera->scroll.y);
	//DrawBox(static_cast<int>(drawX - kPlayerWidth * 0.5f), static_cast<int>(drawY - kPlayerHeight * 0.5f), static_cast<int>(drawX + kPlayerWidth * 0.5f), static_cast<int>(drawY + kPlayerHeight * 0.5f), 0xaaffff, true);
	//DrawRectRotaGraph(drawX, drawY, kGraphWidth * 3, 0, kGraphWidth, kGraphHeight, 1.0f, 0.0f, m_graphHandle, true, false);
	m_currentAnim.Draw({ drawX, drawY }, false);
#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void Player::GroundDraw()
{
	int drawX = static_cast<int>(m_pos.x - m_pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - m_pCamera->scroll.y);
	//DrawBox(static_cast<int>(drawX - kPlayerWidth * 0.5f), static_cast<int>(drawY - kPlayerHeight * 0.5f), static_cast<int>(drawX + kPlayerWidth * 0.5f), static_cast<int>(drawY + kPlayerHeight * 0.5f), 0x22ff00, true);
	//DrawRectRotaGraph(drawX, drawY, 0, 0, kGraphWidth, kGraphHeight, 1.0f, 0.0f, m_graphHandle, true, false);
	m_currentAnim.Draw({ drawX, drawY }, false);

#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

void Player::MissDraw()
{
	int drawX = static_cast<int>(m_pos.x - m_pCamera->scroll.x);
	int drawY = static_cast<int>(m_pos.y - m_pCamera->scroll.y);
	//DrawBox(static_cast<int>(drawX - kPlayerWidth * 0.5f), static_cast<int>(drawY - kPlayerHeight * 0.5f), static_cast<int>(drawX + kPlayerWidth * 0.5f), static_cast<int>(drawY + kPlayerHeight * 0.5f), 0xff00aa, true);
	//DrawRectRotaGraph(drawX, drawY, kGraphWidth, 0, kGraphWidth, kGraphHeight, 1.0f, 0.0f, m_graphHandle, true, false);
	m_currentAnim.Draw({ drawX, drawY }, false);

	DrawString(Game::kScreenWidth / 2, Game::kScreenHeight / 2, L"Miss!", 0xffffff);
#ifdef _DEBUG
	m_colCircle.Draw(drawX, drawY);
	m_colRect.Draw(drawX, drawY);
#endif
}

bool Player::IsOnChestTop(Position2Int& chestPos)
{
	// 宝箱が見つかった際に見つかった座標を指定できるようにするための変数
	int chestX = -1;
	int chestY = -1;

	// マップを使えるようにlockする
	auto pMap = m_pMap.lock();

	float tileSize = pMap->GetTileSize(); // マップのタイルサイズを取得

	const float bottom = m_pos.y + kPlayerHeight * 0.5f; // プレイヤーの下端のY座標
	const float left = m_pos.x - kPlayerWidth * 0.5f; // プレイヤーの左端のX座標
	const float right = m_pos.x + kPlayerWidth * 0.5f; // プレイヤーの右端のX座標

	int bottomY = pMap->WorldPosToMapPos(bottom, tileSize); // プレイヤーの下端のマップ座標Y
	int leftX = pMap->WorldPosToMapPos(left, tileSize); // プレイヤーの左端のマップ座標X
	int rightX = pMap->WorldPosToMapPos(right, tileSize); // プレイヤーの右端のマップ座標X

	// プレイヤーの下端の少し下のマップチップを取得
	const float foot = bottom + kChestConfirmRange; // プレイヤーの下端の少し下のY座標
	int footY = pMap->WorldPosToMapPos(foot, tileSize); // プレイヤーの下端の少し下のマップ座標Y

	for (int x = leftX; x <= rightX; x++) // プレイヤーの幅が大きい時を考慮
	{
		int chipNo = pMap->GetPositioningData(x, footY);
		if (pMap->IsChestTile(chipNo))
		{
			chestX = x;
			chestY = footY;
			chestPos = { chestX,chestY };
#ifdef _DEBUG
			printfDx(L"Player: 宝箱の上にいる\n");
#endif
			return true;
		}
	}
	return false;
}

bool Player::IsOpenChestX() const
{
	// レベルが1以上なら宝箱が横から開けられる
	return m_level >= kPowerUpLevelOne;
}

bool Player::IsPowerUp()
{
	// レベルが1以上ならパワーアップしている
	return m_level >= kPowerUpLevelOne;
}

void Player::AttackableStart()
{
	m_isAttackable = true;
	m_attackCount = kAttackableTime;
}

void Player::DebugClear(const Position2& pos)
{
	m_pos = pos;
}

void Player::FreezeChange()
{
	m_isFreeze = !m_isFreeze;
}

void Player::SetBarrier(const ActivePosition2& barrier)
{
	m_barrierPos = barrier;
}

void Player::JumpStart()
{
	m_direction = { 0.0f,1.0f }; // ジャンプの方向を上向きにする
	m_velocity = { 0.0f, kJumpPower }; // ジャンプの力を加える
	m_isHover = false; // 空中で浮いたかどうかをリセット
	m_isGround = false; // ジャンプしたので地面についていないとする
	m_isJumpStart = true; // ジャンプ開始フラグをtrueにする
	m_frameCount = 0; // 時間経過をリセット
	m_jumpCount--; // ジャンプ回数を減らす
	m_pGameManager->AddScore(kJumpAddScore); // スコアを加算
	m_update = &Player::JumpUpdate; // 更新処理をジャンプ状態に
	m_draw = &Player::JumpDraw; // 描画処理をジャンプ状態に
}

bool Player::IsEntryEnd() const
{
	// 初期状態がentryなのでそれ以外の時は終わっているとする
	return !(m_update == &Player::EntryUpdate);
}

void Player::MissStart()
{
	m_direction = { 0.0f,1.0f }; // ジャンプの方向を上向きにする
	m_velocity = { 0.0f, kMissJumpPower }; // 普通のジャンプより高い力を加える
	m_isHover = false; // 空中で浮いたかどうかをリセット
	m_isGround = false; // 一応ジャンプするので地面についていないとする
	m_isMiss = true; // ミスフラグをtrueにする
	m_frameCount = 0; // 時間経過をリセット
	m_pGameManager->LifeDown(); // 残機を減らす処理を呼ぶ
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

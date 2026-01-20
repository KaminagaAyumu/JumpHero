#include <vector>
#include <list>
#include <memory>
#include "EffectManager.h"
#include "EffekseerEffect.h"
#include <cassert>
#include <string>
#include "EffekseerForDxLib.h"


namespace
{
	constexpr int kBallonChipNo = 33; // 風船のマップチップ番号

	// エフェクトデータを識別するためのenum
	enum
	{
		kEffectCoinGet, // コイン取得
		kEffectImpact,
		kEffectNum
	};

	// 画像ファイル名の配列
	const std::wstring kGraphFileName[] =
	{
		L"data/effect/coin_get.efk", // コイン取得
		L"data/effect/impact.efk", // 強化メダル
	};

	// エフェクトデータの数が違うときにエラーを出せるようにする
	static_assert(kEffectNum == _countof(kGraphFileName), "エフェクト数の定義が間違っています");
}

EffectManager::EffectManager()
{
	m_effekseerEffects.clear();

	for (int i = 0; i < kEffectNum; i++)
	{
		int effectHandle = LoadEffekseerEffect(kGraphFileName[i].c_str());
		assert(effectHandle != -1 && "エフェクトの読み込みに失敗しました");
		m_effectHandles.push_back(effectHandle);
	}
}

EffectManager::~EffectManager()
{
	for (auto& handle : m_effectHandles)
	{
		DeleteEffekseerEffect(handle);
	}
}

void EffectManager::Update()
{
	if (m_effekseerEffects.empty())
	{
		return;
	}

	for (auto& effect : m_effekseerEffects)
	{
		effect->Update();
	}

	// Effekseerにより再生中のエフェクトを更新する。
	UpdateEffekseer2D();

	// Effekseerのエフェクトの削除処理
	m_effekseerEffects.remove_if(
		[](std::shared_ptr<EffekseerEffect> effect)
		{
			return !effect->IsAlive();
		}
	);
}

void EffectManager::Draw()
{
	// Effekseerにより再生中のエフェクトを描画する。
	DrawEffekseer2D();
}

void EffectManager::CreateEffekseerEffect(Types::EffectType type, const Position2& pos)
{
	auto effect = std::make_shared<EffekseerEffect>(m_effectHandles[(int)type], pos);
	// カメラがセットされている場合
	if (auto camera = m_pCamera.lock())
	{
		effect->SetCamera(camera); // エフェクトにカメラをセットする
	}
	m_effekseerEffects.push_back(effect);
}

#include <map>
#include <string>
#include "EffekseerResourceManager.h"
#include "EffekseerForDXLib.h"
#include <cassert>

namespace
{
	constexpr int kResourceExist = 1; // リソースが存在していることを示す
}

int EffekseerResourceManager::LoadEffect(const std::wstring& path)
{
	// エフェクトのパスがすでにロード済みの場合
	if (m_effekseerResources.contains(path))
	{
		auto ref = m_effekseerResources[path];
		ref.refCounter++; // 参照カウンタを増やす
		return ref.handle;
	}
	else
	{
		int handle = LoadEffekseerEffect(path.c_str()); // ハンドルを作成
		m_effekseerResources[path].handle = handle; // ハンドルを設定
		m_effekseerResources[path].refCounter = kResourceExist; // 参照カウンタを追加
		return handle;
	}
}

int EffekseerResourceManager::DeleteEffect(int handle)
{
	// 引数で受け取った要素のイテレータを返す
	auto it = std::find_if(m_effekseerResources.begin(), m_effekseerResources.end(),
		[handle](const std::pair<std::wstring, EffekseerResourceInfo>& resourceInfo)
		{
			return resourceInfo.second.handle == handle;
		});
	// ハンドルが存在しない場合クラッシュ
	assert(it != m_effekseerResources.end());
	if (--it->second.refCounter <= 0) // 参照カウンタを減らして、最後の参照だったら
	{
		DeleteEffekseerEffect(it->second.handle); // リソースのハンドルを解放
		m_effekseerResources.erase(it); // マップからも削除
		return 0; // 削除成功
	}
	else
	{
		return 1; // 削除しないとする
	}

	// ----------------
	// 前行っていた処理
	// ----------------
	//for (auto& res : m_effekseerResources)
	//{
	//	if (handle == res.second.handle)
	//	{
	//		if (--res.second.refCounter <= 0) // 参照カウンタを減らして、最後の参照だったら
	//		{
	//			return DeleteEffekseerEffect(res.second.handle); // エフェクトハンドルを消去する
	//		}
	//		return kResourceExist; // 参照が存在しているので消去しない
	//	}
	//}
}

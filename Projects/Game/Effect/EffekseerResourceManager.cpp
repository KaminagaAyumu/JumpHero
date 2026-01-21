#include <map>
#include <string>
#include "EffekseerResourceManager.h"
#include "EffekseerForDXLib.h"

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
	for (auto& res : m_effekseerResources)
	{
		if (handle == res.second.handle)
		{
			if (--res.second.refCounter <= 0) // 参照カウンタを減らして、最後の参照だったら
			{
				return DeleteEffekseerEffect(res.second.handle); // エフェクトハンドルを消去する
			}
			return kResourceExist; // 参照が存在しているので消去しない
		}
	}
}

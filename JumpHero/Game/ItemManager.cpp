#include <memory>
#include <vector>
#include "ItemManager.h"
#include "ItemBase.h"
#include "../Input.h"

ItemManager::ItemManager(Camera* camera) : 
	m_pCamera(camera)
{
	m_pItems.clear();
}

void ItemManager::Update(Input& input)
{
	if (m_pItems.empty())
	{
		return;
	}
	for (auto& item : m_pItems)
	{
		item->Update(input);
	}

	// アイテムに存在しないものがある場合削除
	m_pItems.remove_if(
		[](std::shared_ptr<ItemBase> item )
		{ 
			return !item->IsExist();
		}
	);
}

void ItemManager::Draw()
{
	if (m_pItems.empty())
	{
		return;
	}
	for (auto& item : m_pItems)
	{
		item->Draw();
	}
}

void ItemManager::SpawnItem(int x, int y)
{
	auto item = std::make_shared<ItemBase>(x, y);
	item->SetCamera(m_pCamera);
	m_pItems.push_back(item);
}

void ItemManager::PushActors(std::vector<Actor*>& actors)
{
	if (m_pItems.empty())
	{
		return;
	}
	for (auto& item : m_pItems)
	{
		actors.push_back(item.get());
	}
}

size_t ItemManager::GetItemNum()
{
	if (m_pItems.empty())
	{
		return 0;
	}
	return m_pItems.size();
}

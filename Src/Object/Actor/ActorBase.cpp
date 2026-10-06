#include "ActorBase.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../Collider/ColliderBase.h"

ActorBase::ActorBase(void)
	: transform_(Transform())
{
}

ActorBase::~ActorBase(void)
{
}

void ActorBase::Initialize(void)
{
	// 初期化処理
	InitLoad();
	InitTransform();
	InitCollider();
	InitAnimation();
	InitPost();
}

void ActorBase::Draw(void)
{
	// 前描画
	DrawPre();

	if (transform_.modelId != -1)
	{
		MV1DrawModel(transform_.modelId);
	}

	// 後描画
	DrawLate();

	if (SceneManager::GetInstance().GetIsDebugMode())
	{
		// 所有しているコライダの描画
		for (const auto& [type, collider] : ownColliders_)
		{
			collider->Draw();
		}
	}
}

void ActorBase::Release(void)
{
	// 自身のコライダ解放
	for (auto& own : ownColliders_)
	{
		delete own.second;
	}
}

const ColliderBase* ActorBase::GetOwnCollider(int key) const
{
	if (ownColliders_.count(key) == 0)
	{
		return nullptr;
	}
	return ownColliders_.at(key);
}


void ActorBase::AddHitCollider(const ColliderBase* hitCollider)
{
	for (const auto& collider : hitColliders_)
	{
		// 衝突相手の登録
		if (collider == hitCollider) { return; }
	}
	hitColliders_.emplace_back(hitCollider);
}
void ActorBase::ClearHitCollider(void)
{
	hitColliders_.clear();
}

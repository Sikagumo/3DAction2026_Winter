#include "SkyDome.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Utility/UtilityMath.h"
#include "Common/Transform.h"

SkyDome::SkyDome(const Transform& syncTransform)
	: syncTransform_(syncTransform)
	, state_(STATE::NONE)
{
	// 状態管理
	stateChanges_.emplace(STATE::NONE, std::bind(&SkyDome::ChangeStateNone, this));
	stateChanges_.emplace(STATE::STAY, std::bind(&SkyDome::ChangeStateStay, this));
	stateChanges_.emplace(STATE::FOLLOW, std::bind(&SkyDome::ChangeStateFollow, this));
}

SkyDome::~SkyDome(void)
{
}

void SkyDome::Initialize(void)
{
	// モデル制御の基本情報
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_SKYDOME));
	transform_.scl = SCALES;
	transform_.pos = UtilityMath::VECTOR_ZERO;
	transform_.quaRot = Quaternion::Euler(
		0.0f, 
		UtilityMath::Deg2Rad(180.0f),
		0.0f
	);
	transform_.quaRotLocal = Quaternion();
	transform_.Update();

	// Zバッファ無効(突き抜け対策)
	MV1SetUseZBuffer(transform_.modelId, false);
	MV1SetWriteZBuffer(transform_.modelId, false);

	// 状態遷移
	auto sceneId = SceneManager::GetInstance().GetSceneID();
	if(sceneId == SceneManager::SCENE_ID::TITLE)
	{
		ChangeState(STATE::STAY);
	}
	else
	{
		ChangeState(STATE::FOLLOW);
	}
}

void SkyDome::Update(void)
{
	// 更新ステップ
	stateUpdate_();
}

void SkyDome::Draw(void)
{
	MV1DrawModel(transform_.modelId);
}

void SkyDome::ChangeState(STATE state)
{
	// 状態変更
	state_ = state;

	// 各状態遷移の初期処理
	stateChanges_[state_]();
}

void SkyDome::ChangeStateNone(void)
{
	stateUpdate_ = std::bind(&SkyDome::UpdateNone, this);
}

void SkyDome::ChangeStateStay(void)
{
	stateUpdate_ = std::bind(&SkyDome::UpdateStay, this);
}

void SkyDome::ChangeStateFollow(void)
{
	stateUpdate_ = std::bind(&SkyDome::UpdateFollow, this);

	transform_.pos = syncTransform_.pos;
	transform_.Update();
}

void SkyDome::UpdateNone(void)
{
}

void SkyDome::UpdateStay(void)
{
}

void SkyDome::UpdateFollow(void)
{
	transform_.pos = syncTransform_.pos;
	transform_.Update();
}

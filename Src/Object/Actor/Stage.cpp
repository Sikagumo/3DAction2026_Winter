#include "Stage.h"
#include <DxLib.h>
#include "../../Utility/UtilityMath.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/ResourceManager.h"
#include "./Character/Player.h"
#include "../Collider/ColliderBase.h"
#include "../Collider/ColliderModel.h"
#include "../Common/Transform.h"

namespace
{
	constexpr float MODEL_SCALE = 0.85f;
	constexpr float LOCAL_ROT_Y = 0.0f;
	constexpr VECTOR MODEL_POSITION = { 0.0f, 2.5f, 0.0f };
};

Stage::Stage(std::unique_ptr<Player>& player)
	: player_(player)
{
}

void Stage::InitLoad(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_STAGE));

	collisionTransform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_STAGE_COLLISION));
}

void Stage::InitTransform(void)
{
    transform_.InitTransform(MODEL_SCALE
        , Quaternion::Identity(), Quaternion::AngleAxis(UtilityMath::Deg2Rad(LOCAL_ROT_Y), UtilityMath::AXIS_Y)
        , MODEL_POSITION);

    collisionTransform_.InitTransform(MODEL_SCALE
        , Quaternion::Identity(), Quaternion::AngleAxis(UtilityMath::Deg2Rad(LOCAL_ROT_Y), UtilityMath::AXIS_Y)
		, MODEL_POSITION);
}

void Stage::InitCollider(void)
{
    MV1SetupCollInfo(collisionTransform_.modelId);

    ColliderModel* colModel = new ColliderModel(ColliderBase::TAG::STAGE, &collisionTransform_);

    ownColliders_.emplace(0, colModel);
}

void Stage::InitAnimation(void)
{
}

void Stage::InitPost(void)
{
}

void Stage::Update(void)
{
}

void Stage::DrawLate(void)
{
#ifdef _DEBUG
    if (SceneManager::GetInstance().GetIsDebugMode())
    {
        MV1DrawModel(collisionTransform_.modelId);
    }
#endif
}
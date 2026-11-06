#include "CharaBase.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/StatusManager.h"
#include "../../Status/StatusEnemy.h"
#include "../../Common/ActionController.h"
#include "Player.h"
#include <memory>
#include "EnemyBase.h"

EnemyBase::EnemyBase(const std::unique_ptr<Player>& player, StatusEnemy::ENEMY_TYPE enemyType)
	: CharaBase::CharaBase()
	, player_(player)
	, status_(StatusManager::GetInstance().GetEnemyStatus(static_cast<int>(enemyType)).GetEnemyParam())
{
}

void EnemyBase::UpdateProcess(void)
{
	SearchField();

	stanTime_ = ((stanTime_ > 0.0f) ? (stanTime_ - SceneManager::GetInstance().GetDeltaTime()) : stanTime_);

	actionController_->SetIsSlow((stanTime_ > 0.0f));

	UpdateProcessPost();
}

void EnemyBase::DrawLate(void)
{
	CharaBase::DrawLate();

#ifdef _DEBUG
	if (SceneManager::GetInstance().GetIsDebugMode())
	{
		// 索敵範囲
		const float MOVE_RANGE = (ACTIVE_MOVE_RANGE * ACTIVE_MOVE_RANGE);
		DrawSphere3D(transform_.pos, MOVE_RANGE, 16, 0xaaffaa, 0xffffff, false);
	}
#endif
}

void EnemyBase::LookPlayerPos(void)
{
	VECTOR pPos = player_->GetTransform().pos;

	// ワールド座標
	VECTOR ePos = VAdd(transform_.pos, transform_.localPos);

	// プレイヤーと敵の距離
	VECTOR diff = VSub(pPos, ePos);

	// 正規化したプレイヤーの方向を追尾
	moveDir_ = VNorm({ diff.x, 0.0f, diff.z });

	// 向きに反映
	transform_.quaRot = Quaternion::LookRotation(moveDir_, UtilityMath::AXIS_Y);
}

void EnemyBase::SearchField(void)
{
	VECTOR pos = VAdd(transform_.pos, { transform_.localPos.x, 0.0f,transform_.localPos.z });
	VECTOR vec = VSub(player_->GetTransform().pos, pos);

	float diff = ((vec.x * vec.x) + (vec.z * vec.z));

	float range = (ACTIVE_MOVE_RANGE * ACTIVE_MOVE_RANGE);

	// 範囲外にいるとき、移動を有効化
	isMove_ = (diff > range);
}
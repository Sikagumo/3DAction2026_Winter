#include "Boss.h"
#include <unordered_map>
#include <cassert>
#include <functional>
#include "../../../Manager/ResourceManager.h"
#include "../../../Utility/UtilityMath.h"
#include "../../../Utility/UtilityCommon.h"
#include "../../../Utility/MatrixUtility.h"
#include "../../Common/AnimationController.h"
#include "../../Common/ActionController.h"
#include "../../../Manager/InputManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Camera/Camera.h"
#include "../../../Manager/StatusManager.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderSphere.h"
#include "../../Collider/ColliderCapsule.h"

namespace
{
	constexpr float LOCAL_ROT_Y = 180.0f;
	constexpr VECTOR START_POS = { 0.0f, 2.5f, 500.0f };
	const float ATTACK_RADIUS = 300.0f;

	// 衝突判定用線分開始
	constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 80.0f, 0.0f };

	// 衝突判定用線分終了
	constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };

	// 衝突判定用カプセル上部球体
	constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 75.0f, 0.0f };

	// 衝突判定用カプセル下部球体
	constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 50.0f, 0.0f };

	// 攻撃位置ローカル座標
	constexpr VECTOR COL_ATTACK_AREA_LOCAL_POS = { 0.0f, 50.0f, 100.0f };


	// 衝突判定用カプセル球体半径
	constexpr float COL_CAPSULE_RADIUS = 20.0f;

	constexpr float PARRY_TIME = 0.5f;


	/// @brief CSV由来のパラメータを行動コントローラに変換
	ActionController::ActionParam ToControllerParam(
		const std::unordered_map<StatusBoss::ACTION_TYPE, StatusBoss::ActionParam>& map,
		StatusBoss::ACTION_TYPE type)
	{
		const auto it = map.find(type);
		if (it == map.end())
		{
#ifdef _DEBUG
			assert((it == map.end()) + "\n行動が見つかりませんでした。；；\n");
#endif
			return ActionController::ActionParam{};	// 全て0のフォールバック
		}

		const StatusBoss::ActionParam& status = it->second;

		return ActionController::ActionParam{
			.timeActive = status
			.timeActive,
			.timeActionTiming = status.timeTimingActive,
			.timeEnd = status.timeEnd,
		};
	}
};

Boss::Boss(void)
	: CharaBase::CharaBase()
	, status_(StatusManager::GetInstance().GetEnemyStatus(static_cast<int>(StatusEnemy::ENEMY_TYPE::BOSS)).GetEnemyParam())
	, actionStatus_(StatusManager::GetInstance().GetBossStatus().GetBossAction())
{
}

void Boss::InitLoadPost(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS));
}

void Boss::InitTransform(void)
{
	transform_.InitTransform(status_.scale
		, Quaternion::Identity()
		, Quaternion::AngleAxis(UtilityMath::Deg2Rad(LOCAL_ROT_Y), UtilityMath::AXIS_Y)
		, START_POS);
}

void Boss::InitCollider(void)
{
	// 主に地面との衝突で仕様する線分コライダ
	ColliderLine* colLine
		= new ColliderLine(ColliderBase::TAG::ENEMY, &transform_
			, COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule
		= new ColliderCapsule(ColliderBase::TAG::ENEMY, &transform_
			, COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS
			, status_.radius);

	// 攻撃範囲コライダ
	ColliderSphere* colArea
		= new ColliderSphere(ColliderBase::TAG::ENEMY, &transform_
			, COL_ATTACK_AREA_LOCAL_POS, 0.0f);

	// 当たり判定リストに格納
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::ATTACK_AREA), colArea);
}

void Boss::InitAnimationPost(void)
{
	// 立ち上がりアニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::STAND_UP)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIMATION_BOSS_STAND_UP)
		, 30.0f);

	// 待機アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::IDLE)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIMATION_PLAYER_IDLE)
		, 30.0f);

	// 歩行アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::WALK)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIMATION_PLAYER_WALK)
		, 30.0f);

	// 弱攻撃アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::ATTACK_LIGHT)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIMATION_PLAYER_SLASH_LIGHT)
		, 30.0f);

	// 強攻撃アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::ATTACK_HEAVY)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIMATION_PLAYER_SLASH_HEAVY)
		, 30.0f);

	// 被ダメージアニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::DAMAGE)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIMATION_DAMAGE)
		, 30.0f);

	// 撃破アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::DEFEAT)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIMATION_DEFEAT)
		, 30.0f);

	PlayAnimation(ANIMATION_TYPE::STAND_UP, false);
}

void Boss::InitActionPost(void)
{
	const auto& actionMap = StatusManager::GetInstance().GetBossStatus().GetBossAction();

	// 弱攻撃
	actionController_->SetAction(static_cast<int>(ACTION_TYPE::ATTACK_LIGHT)
		, ToControllerParam(actionMap, ACTION_TYPE::ATTACK_LIGHT)
		, MakeActionCommand([this]() { AttackLight(); })
	);

	// 強攻撃
	actionController_->SetAction(static_cast<int>(ACTION_TYPE::ATTACK_HEAVY)
		, ToControllerParam(actionMap, ACTION_TYPE::ATTACK_HEAVY)
		, MakeActionCommand([this]() { AttackHeavy(); })
	);
}

void Boss::InitPost(void)
{
	hp_ = status_.hp;
	stamina_ = status_.stamina;

	constexpr float INIT_ANIMATION_STOP = 0.5f;
	animation_->Stop(INIT_ANIMATION_STOP);
}

void Boss::UpdateProcess(void)
{
	if (hp_ <= 0)
	{
		PlayAnimation(ANIMATION_TYPE::DEFEAT, false);
	}
	else
	{
		if (animation_->GetPlayType() == static_cast<int>(ANIMATION_TYPE::DAMAGE)
			&& animation_->IsEnd())
		{
			// 被ダメージアニメーション終了時、待機に遷移
			PlayAnimation(ANIMATION_TYPE::IDLE);
		}
	}
}

void Boss::CollisionReserve(void)
{
}

void Boss::DrawDebug(void)
{
	DrawFormatString(0, (8 * 4), 0xffffff, "BossHp:%d", hp_);
}

void Boss::SetDamage(int damageHp, int damageStamina)
{
	if (timeInv_ > 0.0f) { return; }

	timeInv_ = DAMAGE_INV_TIME;

	hp_ = (((hp_ - damageHp) <= 0) ? 0 : (hp_ - damageHp));
	stamina_ = (((stamina_ - damageStamina) <= 0) ? 0 : (stamina_ - damageHp));

	if (actionController_->IsEndActionActive())
	{
		// 行動終了時、ダメージアニメーション
		PlayAnimation(ANIMATION_TYPE::DAMAGE, false);
	}
}

void Boss::ProcessMove(void)
{
	InputManager& input = InputManager::GetInstance();

	VECTOR dir = UtilityMath::VECTOR_ZERO;


	if (GetJoypadNum() > 0)
	{
		dir = input.GetDirXZ_LStick();
	}
	else
	{
		if (input.IsNew(InputManager::TYPE::PLAYER_MOVE_BACK)) { dir.z += 1.0f; }
		if (input.IsNew(InputManager::TYPE::PLAYER_MOVE_FRONT)) { dir.z += -1.0f; }
		if (input.IsNew(InputManager::TYPE::PLAYER_MOVE_LEFT)) { dir.x += -1.0f; }
		if (input.IsNew(InputManager::TYPE::PLAYER_MOVE_RIGHT)) { dir.x += 1.0f; }
	}

	if (!UtilityMath::EqualsVZero(dir))
	{
		// 移動を正規化
		const float len = std::sqrtf((dir.x * dir.x) + (dir.z * dir.z));
		dir.x /= len;
		dir.z /= len;

		TimeMove_ += SceneManager::GetInstance().GetDeltaTime();

		// 徐々に最高速度に到達させる
		if (moveSpeed_ < status_.speedMax)
		{
			const float rate = UtilityCommon::EaseInOutQuart(TimeMove_ / status_.speedAcc);
			moveSpeed_ = status_.speedInit + (status_.speedMax - status_.speedInit) * rate;
		}

		// アニメーション再生
		PlayAnimation(ANIMATION_TYPE::WALK);

		// カメラの方向で進行
		Quaternion cameraRot = SceneManager::GetInstance().GetCamera().GetQuaRotY();

		// 移動方向を取得
		moveDir_ = Quaternion::PosAxis(cameraRot, dir);

		// 加速度に割り当て
		movePow_ = VScale(moveDir_, moveSpeed_);
	}
	else
	{
		ResetMove();

		PlayAnimation(ANIMATION_TYPE::IDLE);
	}
}

void Boss::AttackLight(void)
{
	SetColliderAttackArea(actionStatus_.at(StatusBoss::ACTION_TYPE::ATTACK_LIGHT).attackRadius);
}

void Boss::AttackHeavy(void)
{
	SetColliderAttackArea(actionStatus_.at(StatusBoss::ACTION_TYPE::ATTACK_HEAVY).attackRadius);
}

void Boss::PlayAnimation(Boss::ANIMATION_TYPE _type, bool isLoop)
{
	int type = static_cast<int>(_type);

	// 指定したアニメーションが割り当てられているとき、処理終了
	if (type == animation_->GetPlayType() && !animation_->IsEnd()) { return; }

	animation_->Play(type, isLoop);
}
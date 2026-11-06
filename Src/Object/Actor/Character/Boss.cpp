#include "EnemyBase.h"
#include <unordered_map>
#include <cassert>
#include <memory>
#include <functional>
#include "../../../Application.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Utility/UtilityMath.h"
#include "../../../Utility/UtilityCommon.h"
#include "../../../Utility/MatrixUtility.h"
#include "../../../Utility/ModelFrameUtility.h"
#include "../../Common/AnimationController.h"
#include "../../Common/ActionController.h"
#include "../../../Manager/InputManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Camera/Camera.h"
#include "Player.h"
#include "../../../Manager/StatusManager.h"
#include "../../Status/StatusEnemy.h"
#include "../../Status/StatusBoss.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderSphere.h"
#include "../../Collider/ColliderCapsule.h"
#include "Boss.h"

namespace
{
	constexpr float LOCAL_ROT_Y = 180.0f;
	constexpr VECTOR START_POS = { 0.0f, 2.5f, 500.0f };
	const float ATTACK_RADIUS = 300.0f;
	constexpr float SWORD_SCALE = 0.85f;

	static constexpr VECTOR LOCAL_POS_SWORD = { 0.0f, 0.0f, 0.0f };
	static constexpr VECTOR LOCAL_ROT_SWORD = { 175.0f, 0.0f, 0.0f };

	// 剣の位置のフレーム
	constexpr int FRAME_NUM_SWORD_POINT = 21;
	constexpr int FRAME_NUM_SWORD_BOTTOM = 21;

	// 衝突判定用線分開始
	constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 80.0f, 0.0f };

	// 衝突判定用線分終了
	constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };

	// 衝突判定用カプセル上部球体
	constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 75.0f, 0.0f };

	// 衝突判定用カプセル下部球体
	constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 50.0f, 0.0f };

	// 攻撃位置ローカル座標
	constexpr VECTOR COL_ATTACK_AREA_LOCAL_POS = { 0.0f, 50.0f, 0.0f };


	// 衝突判定用カプセル球体半径
	constexpr float COL_CAPSULE_RADIUS = 20.0f;

	constexpr float PARRY_TIME = 0.875f;


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

Boss::Boss(const std::unique_ptr<Player>& player)
	: EnemyBase::EnemyBase(player, StatusEnemy::ENEMY_TYPE::BOSS)
	, actionStatus_(StatusManager::GetInstance().GetBossStatus().GetBossAction())
{
}

void Boss::InitLoadPost(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS));
	sword_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_ENEMY_SWORD));
}

void Boss::InitTransform(void)
{
	transform_.InitTransform(status_.scale
		, Quaternion::Identity()
		, Quaternion::AngleAxis(UtilityMath::Deg2Rad(LOCAL_ROT_Y), UtilityMath::AXIS_Y)
		, START_POS);

	VECTOR posSwordBottom = MV1GetFramePosition(transform_.modelId, FRAME_NUM_SWORD_BOTTOM);

	sword_.InitTransform(SWORD_SCALE
		, Quaternion::Identity()
		, Quaternion::Euler(UtilityMath::Deg2Rad(LOCAL_ROT_SWORD))
		, posSwordBottom, LOCAL_POS_SWORD);

	UpdateSwordParam();
}

void Boss::UpdateSwordParam(void)
{
	VECTOR scl = {};
	ModelFrameUtility::GetFrameWorldMatrix(transform_.modelId, FRAME_NUM_SWORD_BOTTOM, scl, sword_.matRot, sword_.pos);

	ModelFrameUtility::SetFrameWorldMatrix(
		transform_, FRAME_NUM_SWORD_BOTTOM, sword_, LOCAL_POS_SWORD, VGet(0.0f, 0.0f, 0.0f));

	// matRot(プレイヤーの向き+手の回転)を quaRot に変換して反映する
	sword_.quaRot = Quaternion::GetRotation(sword_.matRot);

	// 誤った Mult 行は削除(プレイヤーの向きはフレーム行列に含まれており、二重掛けになる)
	sword_.Update();
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
	constexpr VECTOR ANIMATION_LOCAL_POS = { 0.0f, 50.0f, 0.0f };

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
		, ANIMATION_LOCAL_POS, 30.0f);

	// 弱攻撃アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::ATTACK_LIGHT)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIMATION_PLAYER_SLASH_LIGHT)
		, actionStatus_.at(ACTION_TYPE::ATTACK_LIGHT).animationSpeed);

	// 強攻撃アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::ATTACK_HEAVY)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIMATION_PLAYER_SLASH_HEAVY)
		, actionStatus_.at(ACTION_TYPE::ATTACK_HEAVY).animationSpeed);

	// 被ダメージアニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::DAMAGE)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIMATION_DAMAGE)
		, actionStatus_.at(ACTION_TYPE::DAMAGE).animationSpeed);

	// 撃破アニメーション
	animation_->AddExternal(static_cast<int>(ANIMATION_TYPE::DEFEAT)
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIMATION_DEFEAT)
		, actionStatus_.at(ACTION_TYPE::DEFEAT).animationSpeed);

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

	// 撃破
	actionController_->SetAction(static_cast<int>(ACTION_TYPE::DEFEAT)
		, ToControllerParam(actionMap, ACTION_TYPE::DEFEAT)
		, MakeActionCommand([this]() { ActionNone(); })
	);

}

void Boss::InitPost(void)
{
	hp_ = status_.hp;
	stamina_ = status_.stamina;

	constexpr float INIT_ANIMATION_STOP = 0.5f;
	animation_->Stop(INIT_ANIMATION_STOP);

	curActionNum_ = 0;
}

void Boss::UpdateProcessPost(void)
{
	animation_->SetIsSlow((stanTime_ > 0.0f));

	// 立ち上がりアニメーションが終わるまでは処理終了
	if (animation_->GetPlayType() == static_cast<int>(ANIMATION_TYPE::STAND_UP)
		&& !animation_->IsEnd()
		|| hp_ <= 0 || !isAction_) {
		return;
	}
	if (curActionNum_ == 0)
	{
		// 移動処理
		ProcessMove();

		if (!isMove_)
		{
			curActionNum_++;
			ResetMove();

			PlayAnimation(ANIMATION_TYPE::ATTACK_HEAVY, false, true);
			ActiveAction(ACTION_TYPE::ATTACK_HEAVY);
		}
	}
	if (curActionNum_ == 1)
	{
		// 攻撃
		if (actionController_->IsActionEnd()
			&& animation_->IsEnd())
		{
			if (isMove_)
			{
				// 移動
				curActionNum_ = 0;
			}
			else
			{
				// プレイヤーの向き追従
				LookPlayerPos();

				// 再攻撃
				animation_->ReStart();
				PlayAnimation(ANIMATION_TYPE::ATTACK_HEAVY, false, true);
				ActiveAction(ACTION_TYPE::ATTACK_HEAVY);
			}
		}
	}

	if (animation_->IsEnd())
	{
		if (animation_->GetPlayType() == static_cast<int>(ANIMATION_TYPE::DAMAGE)
			|| animation_->GetPlayType() == static_cast<int>(ANIMATION_TYPE::STAND_UP))
		{
			// 被ダメージアニメーション終了時、待機に遷移
			//PlayAnimation(ANIMATION_TYPE::IDLE);
		}
	}
}

void Boss::CollisionReserve(void)
{
}

void Boss::DrawPre(void)
{
	if (sword_.modelId != -1
		&& animation_->GetPlayType() != static_cast<int>(ANIMATION_TYPE::STAND_UP))
	{
		MV1DrawModel(sword_.modelId);
	}
}

void Boss::DrawDebug(void)
{
#ifdef _DEBUG
	EnemyBase::DrawDebug();
	DrawFormatString(Application::SCREEN_HALF_X + 50, (8 * 2), 0xffffff, ", actionNum:%d", actionController_->GetCurActionNum());
#endif

	DrawFormatString(Application::SCREEN_HALF_X - 50, (8 * 2), 0xff0000, "師範HP:%d", hp_);
}

void Boss::SetDamage(int damageHp, int damageStamina)
{
	if (timeInv_ > 0.0f
		|| animation_->GetPlayType() == static_cast<int>(ANIMATION_TYPE::DEFEAT)) { return; }

	timeInv_ = DAMAGE_INV_TIME;

	hp_ = (((hp_ - damageHp) <= 0) ? 0 : (hp_ - damageHp));
	stamina_ = (((stamina_ - damageStamina) <= 0) ? 0 : (stamina_ - damageHp));

	if (hp_ <= 0)
	{
		// 撃破処理
		ActiveAction(ACTION_TYPE::DEFEAT);
		PlayAnimation(ANIMATION_TYPE::DEFEAT, false);
	}
	else if (actionController_->IsEndActionActive())
	{
		// 行動終了時、ダメージアニメーション
		PlayAnimation(ANIMATION_TYPE::DAMAGE, false, true);
	}
}

void Boss::ProcessMove(void)
{
	InputManager& input = InputManager::GetInstance();

	LookPlayerPos();

	if (!UtilityMath::EqualsVZero(moveDir_))
	{
		VECTOR dir = moveDir_;

		// 移動を正規化
		const float len = std::sqrtf((dir.x * dir.x) + (dir.z * dir.z));
		dir.x /= len;
		dir.z /= len;

		// 徐々に最高速度に到達させる
		if (moveSpeed_ < status_.speedMax)
		{
			timeMove_ += SceneManager::GetInstance().GetDeltaTime();

			const float rate = UtilityCommon::EaseInOutQuart(timeMove_ / status_.speedAcc);
			moveSpeed_ = status_.speedInit + (status_.speedMax - status_.speedInit) * rate;
		}

		// アニメーション再生
		PlayAnimation(ANIMATION_TYPE::WALK);

		// 加速度に割り当て
		movePow_ = VScale(moveDir_, moveSpeed_);
	}
}

void Boss::AttackLight(void)
{
	auto& param = actionStatus_.at(StatusBoss::ACTION_TYPE::ATTACK_LIGHT);
	curAttackPower_ = param.attackPower;
	SetColliderAttackArea(param.attackRadius);
}

void Boss::AttackHeavy(void)
{
	auto& param = actionStatus_.at(StatusBoss::ACTION_TYPE::ATTACK_HEAVY);
	curAttackPower_ = param.attackPower;
	SetColliderAttackArea(param.attackRadius);
}

void Boss::PlayAnimation(Boss::ANIMATION_TYPE _type, bool isLoop, bool isReStart, bool isReversePlay)
{
	int type = static_cast<int>(_type);

	if (isReStart && GetAnimationType() == _type)
	{
		animation_->ReStart();
	}

	// 指定したアニメーションが割り当てられているとき、処理終了
	if (type == animation_->GetPlayType() && !animation_->IsEnd()) { return; }

	animation_->Play(type, isLoop, isReversePlay);
}

Boss::ANIMATION_TYPE Boss::GetAnimationType(void) const
{
	int actionNum = actionController_->GetCurActionNum();
	if (actionNum < 0 || actionNum >= static_cast<int>(ANIMATION_TYPE::MAX))
	{
#ifdef _DEBUG
		OutputDebugString("\nボスのアニメーションの種類が取得できませんでした\n");
#endif
		return ANIMATION_TYPE::NONE;
	}

	return static_cast<ANIMATION_TYPE>(actionNum);
}

void Boss::ActiveAction(Boss::ACTION_TYPE type)
{
	int actionNum = static_cast<int>(type);
	// 指定した行動が割り当てられているとき、処理終了
	if (actionNum == actionController_->GetCurActionNum()
		&& !actionController_->IsActionEnd()) { return; }

	actionController_->Active(actionNum);
}

Boss::ACTION_TYPE Boss::GetActionType(void) const
{
	int actionNum = actionController_->GetCurActionNum();
	if (actionNum < 0 || actionNum >= static_cast<int>(ACTION_TYPE::MAX))
	{
#ifdef _DEBUG
		OutputDebugString("\nボスの行動の種類が取得できませんでした\n");
#endif
		return ACTION_TYPE::NONE;
	}

	return static_cast<ACTION_TYPE>(actionNum);
}

